#include <H5Cpp.h>
#include <algorithm>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>

#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/QuickInfo.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/filters/StreamCallbackFilter.hpp>
#include <pdal/io/BufferReader.hpp>
#include <pdal/io/LasReader.hpp>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/logger/progress.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/trajectory.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>
#include <lvox/voxel/coo_grid.hpp>

constexpr auto g_usage_info =
    R"(Usage: lvox [OPTIONS] [-s SCAN_FILE [-o COORDINATE | -t TRAJECTORY_FILE]]...
Scan definitions:
   -s, --scan             filename            Path to a trajectory file, at least one required for
                                              computing PAD. [none by default]

   -t, --trajectory       filename            Path to a trajectory file, required for
                                              computing an MLS scan. [none by default]

   -o, --scan-origin      x y z               Coordinates for the scan position when
                                              computing a TLS scan. Will be matched with the previous
                                              scan input [defaults to (0,0,0)]
Computing options:
   NAME                   EXPECTED VALUES     DESCRIPTION

   -v, --voxel-size       decimal             Size in meters of each voxel elements
                                              of the grid. [defaults to 0.5]

   -m, --method           BL, CF, ULPBL,      The PAD estimator to use. Here's the
                          BCMLE               description of each values:
                                                - BL: Beer-Lambert [default]
                                                - CF: Contact Frequency
                                                - UPLBL: Unequal Path Length
                                                  Beer Lambert
                                                - BCMLE: Bias Corrected Maximum
                                                  Likelyhood Estimator

   -r, --required-counts  number              The number of ray required for PAD computation, if
                                              the amount of rays that entered the voxel is lower
                                              than this number it will be excluded from the estimation
                                              [5 by default]

   -b, --bounds           numbers             The bounds that will be used for creating the voxel grid.
                                              The input format is as followed MINIMAL_COORD,MAXIMAL_COORD,
                                              where MINIMAL_COORD and MAXIMAL_COORD are formatted as such
                                              (0.0,0.0,0.0). So an example of use, would be something like
                                              this (-1.0,-1.0,-1.0)(1.0,1.0,1.0)
                                              [computed automatically by default]

   -u, --unit-surface     number              The surface area of the smallest element of the forest scene.
                                              This value is used in BCMLE and UPBL as a mean of removing bias
                                              for unexplored voxels. If set to 0, it will be ignored.
                                              [defaults to 0]

   -g, --grid             filename            Name of the outputted hdf5 file that will containt the grid
                                              of PAD voxel values. [defaults to out.h5]

   -j, --jobs             number              A number of parallel jobs to use.
                                              [defaults to the amount of core]


   -a, --all              none                Whether or not to include all the information
                                              from the grid (ray counts, lengths, etc.) in the
                                              exported file. [disabled by default]

   -sg, --sparse-grid     none                Whether or not to use sparse grids for computation.
                                              They are slower, but will require less memory.
                                              [disabled by default]

   -l, --log-level        debug, info,        Max log level to display [defaults to info]
                          warning, error

   -h, --help                                 Prints this message
)";

namespace lvox_pe = lvox::algorithms::pad_estimators;
namespace fs      = std::filesystem;

// Type definitions
struct Point
{
    Point(double x, double y, double z, double gps_time = 0)
        : m_x{x}
        , m_y{y}
        , m_z{z}
        , m_gps_time{gps_time}
    {
    }

    double x() const { return m_x; }
    double y() const { return m_y; }
    double z() const { return m_z; }
    double gps_time() const { return m_gps_time; }

    bool operator==(const Point& other) const
    {
        return m_x == other.m_x && m_y == other.m_y && m_z == other.m_z &&
               m_gps_time == other.m_gps_time;
    }

    bool operator!=(const Point& other) const { return !(other == *this); }

    double m_x;
    double m_y;
    double m_z;
    double m_gps_time = 0.0;
};

using PointCloud    = std::vector<Point>;
using Scan          = lvox::Scan<Point, PointCloud>;
using ScannerOrigin = lvox::ScannerOrigin<Point, PointCloud>;
using Trajectory    = lvox::Trajectory<Point, PointCloud>;

bool                                g_outputs_profile       = false;
double                              g_voxel_size            = 0.5;
unsigned int                        g_job_count             = std::thread::hardware_concurrency();
lvox_pe::PADEstimator               g_pad_estimator         = lvox_pe::BeerLambert{};
std::vector<Point>                  g_scan_origins          = {};
std::vector<PointCloud>             g_point_clouds          = {};
std::vector<lvox::Bounds<double>>   g_point_cloud_bounds    = {};
std::mutex                          g_print_guard           = {};
std::vector<Trajectory>             g_scan_trajectories     = {};
fs::path                            g_grid_file             = "out.h5";
bool                                g_include_all_info      = false;
bool                                g_use_sparse_grids      = false;
unsigned int                        g_required_counts       = 5;
std::optional<lvox::Bounds<double>> g_bounds                = {};
double                              g_smallest_element_area = 0.;
fs::path                            g_file;

PointCloud load_point_cloud_from_file(
    const std::filesystem::path&                                file,
    std::optional<std::reference_wrapper<lvox::Bounds<double>>> bounds = {}
)
{
    using dim = pdal::Dimension::Id;

    lvox::Logger logger{"Point cloud file reader"};
    if (!fs::exists(file) || !fs::is_regular_file(file))
    {
        throw std::runtime_error{std::format(
            "Provided point cloud file doesn't exists or is not a file: {}", file.string()
        )};
    }

    pdal::Options options;
    options.add("filename", file.string());

    pdal::FixedPointTable pts_table{10'000};
    auto                  layout = pts_table.layout();
    layout->registerDim(dim::X);
    layout->registerDim(dim::Y);
    layout->registerDim(dim::Z);
    layout->registerDim(dim::GpsTime);
    layout->registerDim(dim::Classification);

    auto stage_factory = std::make_unique<pdal::StageFactory>();

    const std::string driver = pdal::StageFactory::inferReaderDriver(file.string());

    if (driver.empty())
    {
        throw std::runtime_error(
            std::format("Cannot determine reader for input file: {}", file.string())
        );
    }

    pdal::Stage* file_reader = stage_factory->createStage(driver);

    file_reader->setOptions(options);
    auto           quick_info = file_reader->preview();
    lvox::Progress progress{quick_info.m_pointCount};

    {
        std::lock_guard<std::mutex> lock{g_print_guard};
        logger.info("Loading file {}", file.filename().string());
    }

    PointCloud out;
    const bool                 calculate_bounds = bounds.has_value();
    pdal::StreamCallbackFilter sc;
    sc.setCallback([calculate_bounds, &bounds, &out, &progress](const auto& pt) mutable -> bool {
        const double x    = pt.template getFieldAs<double>(dim::X);
        const double y    = pt.template getFieldAs<double>(dim::Y);
        const double z    = pt.template getFieldAs<double>(dim::Z);
        const int    clss = pt.template getFieldAs<int>(dim::Classification);

        out.emplace_back(x, y, z, pt.template getFieldAs<double>(dim::GpsTime));

        if (calculate_bounds)
        {
            bounds->get().grow(x, y, z);
        }

        progress.increase_progression_by(1);
        return true;
    });

    try
    {
        sc.setInput(*file_reader);
        sc.prepare(pts_table);

        std::jthread watch_progress{[&progress]() {
            while (!progress.completed())
                progress.print();
        }};

        sc.execute(pts_table);
        progress.completed();
    }
    catch (pdal::pdal_error e)
    {
        logger.error("Pdal error: {}", e.what());
        throw e;
    }

    return out;
}

void export_to_h5(
    lvox::COOGrid&&              grid,
    const std::string&           dataset_name,
    const std::filesystem::path& filename,
    bool                         include_all_data = false
)
{
    lvox::Logger logger{"Grid HDF5 export"};
    H5::H5File   file;
    if (std::filesystem::exists(filename))
    {
        logger.warn("Removing existing h5 file: {}", filename.string());
        std::filesystem::remove(filename);
    }

    file                                       = H5::H5File{filename.string(), H5F_ACC_TRUNC};
    std::vector<int>          xs               = grid.xs();
    std::vector<int>          ys               = grid.ys();
    std::vector<int>          zs               = grid.zs();
    std::vector<unsigned int> counts           = grid.counts();
    std::vector<unsigned int> hits             = grid.hits();
    std::vector<double>       pads             = grid.pads();
    std::vector<double>       lengths          = grid.lengths();
    std::vector<double>       hits_lengths     = grid.hits_lengths();
    std::vector<double>       lengths_variance = grid.lengths_variance();
    lvox::BoundedGrid         bounded_grid     = grid.bounds();

    const hsize_t voxels_with_data = std::ranges::distance(pads);

    using h5_dimension_t = std::array<hsize_t, 1>;
    const h5_dimension_t voxel_count_dim{voxels_with_data};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, voxel_count_dim.data()};

    H5::Group plot_group;
    if (file.nameExists(dataset_name))
        plot_group = file.openGroup(dataset_name);
    else
        plot_group = file.createGroup(dataset_name, 8);

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{std::min(static_cast<hsize_t>(2 << 13), voxels_with_data)};
    create_prop_list.setChunk(1, chunk_dims.data());

    const auto get_or_create_dataset =
        [&plot_group, &dataset_name, &create_prop_list](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::DataSet {
        if (plot_group.nameExists(name))
        {
            return plot_group.openDataSet(name);
        }

        return plot_group.createDataSet(name, type, dataspace, create_prop_list);
    };

    logger.info("Exporting grid to '{}'", filename.string());

    const H5::PredType h5_index_t = H5::PredType::NATIVE_INT;
    {
        H5::DataSet xs_data = get_or_create_dataset("x", h5_index_t, data_space);
        xs_data.write(xs.data(), h5_index_t);
    }

    {
        H5::DataSet ys_data = get_or_create_dataset("y", h5_index_t, data_space);
        ys_data.write(ys.data(), h5_index_t);
    }

    {
        H5::DataSet zs_data = get_or_create_dataset("z", h5_index_t, data_space);
        zs_data.write(zs.data(), h5_index_t);
    }

    {
        H5::PredType h5_pad_t    = H5::PredType::NATIVE_DOUBLE;
        H5::DataSet  values_data = get_or_create_dataset("pad", h5_pad_t, data_space);
        values_data.write(pads.data(), h5_pad_t);
    }

    if (include_all_data)
    {
        {
            H5::PredType h5_hits_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  hits_data = get_or_create_dataset("hits", h5_hits_t, data_space);
            hits_data.write(hits.data(), h5_hits_t);
        }

        {
            H5::PredType h5_counts_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  counts_data = get_or_create_dataset("counts", h5_counts_t, data_space);
            counts_data.write(counts.data(), h5_counts_t);
        }

        {
            H5::PredType h5_lengths_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  lengths_data = get_or_create_dataset("lengths", h5_lengths_t, data_space);
            lengths_data.write(lengths.data(), h5_lengths_t);
        }

        if (!lengths_variance.empty())
        {
            H5::PredType h5_lengths_var_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  length_variances_data =
                get_or_create_dataset("lengths variance", h5_lengths_var_t, data_space);
            length_variances_data.write(lengths_variance.data(), h5_lengths_var_t);
        }
    }

    // Minimum coordinate attribute
    const std::array<hsize_t, 1> scalar_value_dim{1};
    H5::DataSpace                scalar_data_space{1, scalar_value_dim.data()};
    const std::array<hsize_t, 1> singular_3d_coord_count = {3};
    H5::DataSpace                singular_3d_coord_data_space{1, singular_3d_coord_count.data()};

    const auto get_or_create_attribute =
        [&plot_group](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::Attribute {
        if (plot_group.attrExists(name))
        {
            return plot_group.openAttribute(name);
        }

        return plot_group.createAttribute(name, type, dataspace);
    };
    // Voxel size
    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        get_or_create_attribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &bounded_grid.m_cell_size);

    const auto h5_bounds_coords_t = H5::PredType::NATIVE_DOUBLE;

    // Min coords
    H5::Attribute min_coord_attr = get_or_create_attribute(
        "Minimal coordinates values", h5_bounds_coords_t, singular_3d_coord_data_space
    );

    const std::array<double, 3> min_coords = {
        bounded_grid.bounds().m_min_x, bounded_grid.bounds().m_min_y, bounded_grid.bounds().m_min_z
    };
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());

    // Max coords
    H5::Attribute max_coord_attr = get_or_create_attribute(
        "Maximum coordinates values", h5_bounds_coords_t, singular_3d_coord_data_space
    );

    const std::array<double, 3> max_coords = {
        bounded_grid.bounds().m_max_x, bounded_grid.bounds().m_max_y, bounded_grid.bounds().m_max_z
    };
    min_coord_attr.write(h5_voxel_size_t, max_coords.data());

    const auto h5_bounds_index_t = H5::PredType::NATIVE_INT;

    // Min coords
    H5::Attribute min_index_attr = get_or_create_attribute(
        "Minimal index values", h5_bounds_coords_t, singular_3d_coord_data_space
    );

    const std::array<int, 3> min_indices = {
        bounded_grid.index_bounds().m_min_x,
        bounded_grid.index_bounds().m_min_y,
        bounded_grid.index_bounds().m_min_z
    };
    min_coord_attr.write(h5_voxel_size_t, min_indices.data());

    // Max coords
    H5::Attribute max_index_attr = get_or_create_attribute(
        "Maximum index values", h5_bounds_index_t, singular_3d_coord_data_space
    );

    const std::array<int, 3> max_indices = {
        bounded_grid.index_bounds().m_max_x,
        bounded_grid.index_bounds().m_max_y,
        bounded_grid.index_bounds().m_max_z
    };
    min_coord_attr.write(h5_voxel_size_t, max_indices.data());

    // Grid dimensions
    const auto    h5_grid_dim_t = H5::PredType::NATIVE_UINT64;
    H5::Attribute grid_dims_attr =
        get_or_create_attribute("Dimensions", h5_grid_dim_t, singular_3d_coord_data_space);
    const std::array<size_t, 3> grid_dims = {
        bounded_grid.dim_x(), bounded_grid.dim_y(), bounded_grid.dim_z()
    };
    grid_dims_attr.write(h5_grid_dim_t, grid_dims.data());

    file.close();
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv + 1, argv + argc};
    lvox::Logger             logger{"LVOX"};

    if (args.empty())
    {
        logger.error("Invalid amount of arguments supplied");
        std::cout << g_usage_info << std::endl;
        return 1;
    }

    auto arg_it = args.begin();
    while (arg_it != args.end())
    {
        // TODO: handle this better? Or just make it PDAL plugin
        if (*arg_it == "-t" || *arg_it == "--trajectory")
        {
            fs::path traj_file = *++arg_it;
            if (!g_scan_origins.empty())
            {
                logger.warn("Scan trajectory being set while in TLS mode, ignoring it");
                continue;
            }

            logger.info("Loading trajectory file {}", traj_file.string());

            g_scan_trajectories.emplace_back(load_point_cloud_from_file(traj_file, {}));
        }
        else if (*arg_it == "-v" || *arg_it == "--voxel-size")
        {
            g_voxel_size = std::stod(*++arg_it);
        }
        else if (*arg_it == "-o" || *arg_it == "--scan-origin")
        {
            if (!g_scan_trajectories.empty())
            {
                logger.warn("Scan origin being set while in MLS mode, ignoring it");
                continue;
            }

            g_scan_origins.emplace_back(
                std::stod(*++arg_it), std::stod(*++arg_it), std::stod(*++arg_it), 0
            );
        }
        else if (*arg_it == "-s" || *arg_it == "--scan")
        {
            fs::path point_cloud_file = *++arg_it;
            lvox::Bounds<double>& bounds = g_point_cloud_bounds.emplace_back();

            // TODO: Somehow make it not compute bounds when one is provided, although that's a very neglible optimization.
            g_point_clouds.emplace_back(load_point_cloud_from_file(point_cloud_file, std::ref(bounds)));
        }
        else if (*arg_it == "-g" || *arg_it == "--grid")
        {
            g_grid_file = *++arg_it;
        }
        else if (*arg_it == "-m" || *arg_it == "--method")
        {
            std::string pad_compute_method_str = *++arg_it;
            std::ranges::transform(
                pad_compute_method_str, pad_compute_method_str.begin(), [](const unsigned char c) {
                    return std::toupper(c);
                }
            );

            if (pad_compute_method_str == "BL")
            {
                g_pad_estimator = lvox_pe::BeerLambert{};
            }
            else if (pad_compute_method_str == "CF")
            {
                g_pad_estimator = lvox_pe::ContactFrequency{};
            }
            else if (pad_compute_method_str == "UPLBL")
            {
                g_pad_estimator = lvox_pe::UnequalPathLengthBeerLambert{};
            }
            else if (pad_compute_method_str == "BCMLE")
            {
                g_pad_estimator = lvox_pe::BiasCorrectedMaximumLikelyhoodEstimator{};
            }
            else
            {
                logger.error("Unkown PAD compute method '{}'", pad_compute_method_str);
                std::cout << g_usage_info << std::endl;
                return 1;
            }
        }
        else if (*arg_it == "-j" || *arg_it == "--jobs")
        {
            g_job_count = std::stoi(*++arg_it);
        }
        else if (*arg_it == "-h" || *arg_it == "--help")
        {
            std::cout << g_usage_info << std::endl;
        }
        else if (*arg_it == "-b" || *arg_it == "--bounds")
        {
            std::string bounds_str = *++arg_it;
            std::transform(
                bounds_str.begin(), bounds_str.end(), bounds_str.begin(), [](char c) -> char {
                    if (c == '(' || c == ')' || c == ',')
                        return ' ';

                    return c;
                }
            );

            std::istringstream   iss{bounds_str};
            lvox::Bounds<double> bounds;
            iss >> bounds.m_min_x;
            iss >> bounds.m_min_y;
            iss >> bounds.m_min_z;

            iss >> bounds.m_max_x;
            iss >> bounds.m_max_y;
            iss >> bounds.m_max_z;

            g_bounds = std::move(bounds);

        }
        else if (*arg_it == "-u" || *arg_it == "--unit-surface")
        {
            g_smallest_element_area = std::stod(*++arg_it);
        }
        else if (*arg_it == "-a" || *arg_it == "--all")
        {
            g_include_all_info = true;
        }
        else if (*arg_it == "-sg" || *arg_it == "--sparse-grid")
        {
            g_use_sparse_grids = true;
        }
        else if (*arg_it == "-r" || *arg_it == "--required-hits")
        {
            g_required_counts = std::stoi(*++arg_it);
        }
        else if (*arg_it == "-l" || *arg_it == "--log-level")
        {
            std::string log_level_str = *++arg_it;
            std::ranges::transform(log_level_str, log_level_str.begin(), [](const unsigned char c) {
                return std::toupper(c);
            });

            if (log_level_str == "DEBUG")
            {
                lvox::Logger::set_global_level(lvox::Logger::Level::Debug);
            }
            else if (log_level_str == "INFO")
            {
                lvox::Logger::set_global_level(lvox::Logger::Level::Info);
            }
            else if (log_level_str == "WARNING")
            {
                lvox::Logger::set_global_level(lvox::Logger::Level::Warn);
            }
            else if (log_level_str == "ERROR")
            {
                lvox::Logger::set_global_level(lvox::Logger::Level::Error);
            }
            else
            {
                logger.error("Unkown log level '{}'", log_level_str);
                std::cout << g_usage_info << std::endl;
                return 1;
            }
        }
        else
        {
            g_file = *arg_it;
        }
        ++arg_it;
    }

    std::vector<Scan> scans;
    if (g_scan_trajectories.empty())
    {
        for (size_t i = 0; i < g_point_clouds.size(); ++i)
        {
            scans.emplace_back(Scan{g_point_clouds[i], g_scan_origins[i], g_point_cloud_bounds[i], {}});
        }
    }
    else
    {
        // TODO: check if we want to support MLS mutli-scan?
        scans.emplace_back(Scan{g_point_clouds[0], g_scan_trajectories[0], g_point_cloud_bounds[0], {}});
    }

    if (scans.empty())
    {
        logger.warn("No scans defined, nothing to be done!");
        return 0;
    }

    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size            = g_voxel_size,
        .m_job_limit             = g_job_count,
        .m_pad_estimator         = g_pad_estimator,
        .m_use_sparse_grid       = g_use_sparse_grids,
        .m_required_counts       = g_required_counts,
        .m_smallest_element_area = g_smallest_element_area,
        .m_bounds                = g_bounds,
        .m_log_stream            = std::cout
    };

    lvox::COOGrid result = lvox::algorithms::compute_pad(scans, compute_options);
    export_to_h5(std::move(result), "lvox", g_grid_file, g_include_all_info);
}
