#include <H5Cpp.h>
#include <algorithm>
#include <filesystem>
#include <format>
#include <functional>
#include <future>
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

#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/trajectory.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/coo_grid.hpp>

constexpr auto g_usage_info =
    R"(Usage: lvox [OPTIONS] FILE
Options:
   ARGS                   EXPECTED VALUES     DESCRIPTION
   -t, --trajectory       filename            Path to a trajectory file, required for
                                              computing an MLS scan. [none by default]

   -o, --scan-origin      x y z               Coordinates for the scan position when
                                              computing a TLS scan. [defaults to (0,0,0)]

   -v, --voxel-size       decimal             Size in meters of each voxel elements
                                              of the grid. [defaults to 0.5]

   -g, --grid             filename            Outputs the grid of PAD voxel values to a
                                              hdf5 file. [defaults to out.h5]

   -b, --blanks           none                Whether or not to use points classified with
                                              flag 0 as reference for "blank shots". These
                                              can used alongside virtual scene to measure
                                              the impact of rays that didn't touch anything
                                              on PAD estimations. [disabled by defaults]

   -j, --jobs             number              A number of parallel jobs to use.
                                              [defaults to the amount of core]

   -m, --method           BL, CF, ULPBL,      The PAD estimator to use. Here's the
                          BCMLE               description of each values:
                                                - BL: Beer-Lambert [default]
                                                - CF: Contact Frequency
                                                - UPLBL: Unequal Path Length
                                                  Beer Lambert
                                                - BCMLE: Bias Corrected Maximum
                                                  Likelyhood Estimator

   -a, --all              none                Whether or not to include all the information
                                              from the grid (ray counts, lengths, etc.) in the
                                              exported file. [disabled by default]

   -s, --sparse           none                Whether or not to use sparse grids for computation.
                                              They are slower, but will require less memory.
                                              [disabled by default]

   -r, --required-hits    number              The number of return required for PAD computation, if
                                              the return amount in the voxel is lower than this number
                                              it will be excluded from the estimation [5 by default]

   -l, --log-level        debug, info,        Max log level to display [defaults to info]
                          warning, error

   -h, --help                                 Prints this message
    )";

namespace lvox_pe = lvox::algorithms::pad_estimators;
namespace fs      = std::filesystem;

bool                       g_is_mls               = false;
bool                       g_outputs_profile      = false;
double                     g_voxel_size           = 0.5;
unsigned int               g_job_count            = std::thread::hardware_concurrency();
lvox_pe::PADEstimator      g_pad_estimator        = lvox_pe::BeerLambert{};
bool                       g_compute_theoriticals = false;
std::optional<lvox::Point> g_scan_origin          = {};
std::mutex                 g_print_guard          = {};
std::optional<fs::path>    g_traj_file            = {};
fs::path                   g_grid_file            = "out.h5";
bool                       g_include_all_info     = false;
bool                       g_use_sparse_grids     = false;
unsigned int               g_required_hits        = 5;
fs::path                   g_file;

struct PointCloudWithTheoriticalShots
{
    lvox::PointCloudView                m_point_cloud;
    std::optional<lvox::PointCloudView> m_blank_shots;
};

PointCloudWithTheoriticalShots load_point_cloud_from_file(
    const std::filesystem::path&                        file,
    std::optional<std::reference_wrapper<lvox::Bounds>> bounds = {}
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

    {
        std::lock_guard<std::mutex> lock{g_print_guard};
        logger.info("Loading file {}", file.filename().string());
    }

    PointCloudWithTheoriticalShots out;
    out.m_point_cloud = std::make_unique<lvox::PointCloud>();

    const bool                 calculate_bounds = bounds.has_value();
    pdal::StreamCallbackFilter sc;
    sc.setCallback([calculate_bounds, &bounds, &out](const auto& pt) mutable -> bool {
        const double x    = pt.template getFieldAs<double>(dim::X);
        const double y    = pt.template getFieldAs<double>(dim::Y);
        const double z    = pt.template getFieldAs<double>(dim::Z);
        const int    clss = pt.template getFieldAs<int>(dim::Classification);

        if (g_compute_theoriticals && clss == 0)
        {
            if (!out.m_blank_shots)
            {
                out.m_blank_shots = std::make_unique<lvox::PointCloud>();
            }
            (*out.m_blank_shots)
                ->emplace_back(pt.template getFieldAs<double>(dim::GpsTime), lvox::Point{x, y, z});
        }
        else
        {
            out.m_point_cloud->emplace_back(
                pt.template getFieldAs<double>(dim::GpsTime), lvox::Point{x, y, z}
            );

            if (calculate_bounds)
            {
                bounds->get().grow(x, y, z);
            }
        }

        return true;
    });

    try
    {
        sc.setInput(*file_reader);
        sc.prepare(pts_table);

        sc.execute(pts_table);
    }
    catch (pdal::pdal_error e)
    {
        logger.error("Pdal error: {}", e.what());
        throw e;
    }

    return out;
}

std::vector<lvox::Scan> read_dot_in_file(const std::filesystem::path& in_file)
{
    namespace fs = std::filesystem;
    std::ifstream  fstream{in_file};
    const fs::path parent_path = in_file.parent_path();

    std::vector<std::future<lvox::Scan>> scans;
    while (!fstream.eof())
    {
        std::ostringstream point_cloud_file_name;
        fstream.get(*point_cloud_file_name.rdbuf(), ' ');

        fstream.seekg(std::string{" Z+F "}.size(), std::ios_base::cur);

        double x;
        double y;
        double z;

        fstream >> x;
        fstream >> y;
        fstream >> z;

        fstream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        scans.emplace_back(
            std::async(
                [parent_path, point_cloud_file_name = point_cloud_file_name.str(), x, y, z]()
                    -> lvox::Scan {
                    lvox::Bounds bounds;

                    auto point_cloud_from_file = load_point_cloud_from_file(
                        fs::path{parent_path / point_cloud_file_name}, {bounds}
                    );
                    return lvox::Scan{
                        .m_points = std::move(point_cloud_from_file.m_point_cloud),
                        .m_scanner_origin{lvox::Point{x, y, z}},
                        .m_bounds{bounds},
                        .m_blank_shots{std::move(point_cloud_from_file.m_blank_shots)}
                    };
                }
            )
        );
    }

    std::vector<lvox::Scan> out_scans;
    std::ranges::transform(
        scans, std::back_inserter(out_scans), [](std::future<lvox::Scan>& future) -> lvox::Scan {
            return future.get();
        }
    );
    return out_scans;
}

std::vector<lvox::Scan> create_scan_using_pdal(
    const std::filesystem::path&         file,
    std::optional<std::filesystem::path> traj_file   = {},
    std::optional<lvox::Point>           scan_origin = {}
)
{
    lvox::Logger            logger{"Point cloud loader"};
    lvox::Bounds            scan_bounds;
    auto                    scan_point_cloud = load_point_cloud_from_file(file, {scan_bounds});
    std::vector<lvox::Scan> scans;

    lvox::ScannerOrigin scanner_origin;

    if (traj_file)
    {
        if (scan_origin && *scan_origin != lvox::Vector::Constant(0.))
        {
            logger.warn("Scan origin being set while in MLS mode, ignoring it");
        }

        logger.info("Loading trajectory file {}", traj_file->string());

        scanner_origin = std::make_shared<lvox::Trajectory>(
            load_point_cloud_from_file(*traj_file, {}).m_point_cloud
        );
    }
    else if (scan_origin)
    {
        scanner_origin = *scan_origin;
    }
    else // Defaulting to (0,0,0)
    {
        scanner_origin = lvox::Vector::Constant(0.);
    }

    scans.emplace_back(
        std::move(scan_point_cloud.m_point_cloud),
        scanner_origin,
        scan_bounds,
        std::move(scan_point_cloud.m_blank_shots)
    );

    return scans;
}

void export_to_h5(
    lvox::COOGrid&&              grid,
    const std::string&           dataset_name,
    const std::filesystem::path& filename,
    bool                         include_all_data = false
)
{
    lvox::Logger     logger{"Grid HDF5 export"};
    H5::H5File file;
    if (std::filesystem::exists(filename))
        std::filesystem::remove(filename);

    file = H5::H5File{filename.string(), H5F_ACC_TRUNC};
    std::vector<unsigned int> xs = std::move(grid.xs());
    std::vector<unsigned int> ys = std::move(grid.ys());
    std::vector<unsigned int> zs = std::move(grid.zs());
    std::vector<unsigned int> counts = std::move(grid.counts());
    std::vector<unsigned int> hits = std::move(grid.hits());
    std::vector<double>       pads = std::move(grid.pads());
    std::vector<double>       lengths = std::move(grid.lengths());
    std::vector<double>       hits_lengths = std::move(grid.hits_lengths());
    std::vector<double>       lengths_variance = std::move(grid.lengths_variance());
    lvox::BoundedGrid         bounded_grid     = std::move(grid.bounds());

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

    const H5::PredType h5_index_t = H5::PredType::NATIVE_UINT;
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
    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        get_or_create_attribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &bounded_grid.m_cell_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::Attribute min_coord_attr = get_or_create_attribute(
        "Minimal coordinates values", h5_min_coords_t, singular_3d_coord_data_space
    );

    const std::array<double, 3> min_coords = {
        bounded_grid.bounds().m_min_x,
        bounded_grid.bounds().m_min_y,
        bounded_grid.bounds().m_min_z
    };
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());

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
            g_is_mls    = true;
            g_traj_file = *++arg_it;
        }
        else if (*arg_it == "-v" || *arg_it == "--voxel-size")
        {
            g_voxel_size = std::stod(*++arg_it);
        }
        else if (*arg_it == "-o" || *arg_it == "--scan-origin")
        {
            g_scan_origin = {std::stod(*++arg_it), std::stod(*++arg_it), std::stod(*++arg_it)};
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
            return 0;
        }
        else if (*arg_it == "-b" || *arg_it == "--blanks")
        {
            g_compute_theoriticals = true;
        }
        else if (*arg_it == "-a" || *arg_it == "--all")
        {
            g_include_all_info = true;
        }
        else if (*arg_it == "-s" || *arg_it == "--sparse")
        {
            g_use_sparse_grids = true;
        }
        else if (*arg_it == "-r" || *arg_it == "--required-hits")
        {
            g_required_hits = std::stoi(*++arg_it);
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

    std::vector<lvox::Scan> scans;

    if (g_file.extension() == ".in")
    {
        scans = read_dot_in_file(g_file);
    }
    else
    {
        scans = create_scan_using_pdal(g_file, g_traj_file, g_scan_origin);
    }

    const lvox::algorithms::ComputeOptions compute_options{
        .m_voxel_size           = g_voxel_size,
        .m_job_limit            = g_job_count,
        .m_pad_estimator        = g_pad_estimator,
        .m_compute_theoriticals = g_compute_theoriticals,
        .m_use_sparse_grid      = g_use_sparse_grids,
        .m_required_hits        = g_required_hits
    };
    lvox::COOGrid result = lvox::algorithms::compute_pad(scans, compute_options);
    export_to_h5(std::move(result), "lvox", g_grid_file, g_include_all_info);
}
