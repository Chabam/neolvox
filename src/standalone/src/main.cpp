#include <algorithm>
#include <filesystem>
#include <format>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <ranges>
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

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/trajectory.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/h5_exporter.hpp>

constexpr auto g_usage_info =
    R"(Usage: lvox [OPTIONS] FILE
Options:
   ARGS               EXPECTED VALUES     DESCRIPTION
   -t, --trajectory   filename            Path to a trajectory file, required for
                                          computing an MLS scan. [none by default]

   -o, --scan-origin  x y z               Coordinates for the scan position when
                                          computing a TLS scan. [defaults to (0,0,0)]

   -v, --voxel-size   decimal             Size in meters of each voxel elements
                                          of the grid. [defaults to 0.5]

   -g, --grid         filename            Outputs the grid of PAD voxel values to a
                                          hdf5 file. [defaults to out.h5]

   -b, --blanks       none                Wheter or not to use points classified with
                                          flag 0 as reference for "blank shots". These
                                          can used alongside virtual scene to measure
                                          the impact of rays that didn't touch anything
                                          on PAD estimations. [disabled by defaults]

   -p, --profile      filename            Outputs to vertical profile of the voxels
                                          of the grid to a csv file.
                                          [disabled by defaults]

   -j, --jobs         number              A number of parallel jobs to use.
                                          [defaults to the amount of core]

   -m, --method       BL, CF, ULPBL       The PAD estimator to use. Here's the
                                          description of each values:
                                             - BL: Beer-Lambert [default]
                                             - CF: Contact Frequency
                                             - ULPBL: Unequal Length Path
                                               Beer Lambert

   -h, --help                             Prints this message
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
fs::path                   g_file;
std::optional<fs::path>    g_traj_file            = {};
std::optional<fs::path>    g_output_profile_file  = {};
fs::path                   g_grid_file            = "out.h5";


struct PointCloudWithTheoriticalShots
{
    lvox::PointCloudView                m_point_cloud;
    std::optional<lvox::PointCloudView> m_blank_shots;
};

auto load_point_cloud_from_file(
    const std::filesystem::path&                        file,
    std::optional<std::reference_wrapper<lvox::Bounds>> bounds = {}
) -> PointCloudWithTheoriticalShots
{
    using dim    = pdal::Dimension::Id;

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

    sc.setInput(*file_reader);
    sc.prepare(pts_table);

    sc.execute(pts_table);

    return out;
}

auto output_profile_to_csv(
    const std::filesystem::path& path, const lvox::algorithms::PadResult& result
) -> void
{
    std::ofstream fstream{path};

    fstream << "Height in meters,PAD\n";
    for (size_t z = 0; z < result.dim_z(); ++z)
    {
        double sum = 0;
        for (size_t x = 0; x < result.dim_x(); ++x)
            for (size_t y = 0; y < result.dim_y(); ++y)
                sum += result.at(x, y, z);

        fstream << std::format("{},{}\n", z * result.cell_size(), sum);
    }

    fstream.flush();
}

auto read_dot_in_file(const std::filesystem::path& in_file) -> std::vector<lvox::Scan>
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

    return scans |std::views::transform([](std::future<lvox::Scan>& future) -> lvox::Scan {
               return future.get();
           }) |
           std::ranges::to<std::vector<lvox::Scan>>();
}

auto create_scan_using_pdal(
    const std::filesystem::path&         file,
    std::optional<std::filesystem::path> traj_file   = {},
    std::optional<lvox::Point>           scan_origin = {}
) -> std::vector<lvox::Scan>
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

auto main(int argc, char* argv[]) -> int
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
        else if (*arg_it == "-p" || *arg_it == "--profile")
        {
            g_output_profile_file = *++arg_it;
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
            else if (pad_compute_method_str == "ULPBL")
            {
                g_pad_estimator = lvox_pe::UnequalPathLengthBeerLambert{};
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
        .m_compute_theoriticals = g_compute_theoriticals
    };
    const lvox::algorithms::PadResult result =
        lvox::algorithms::compute_pad(scans, compute_options);

    lvox::h5_exporter::export_grid(result, "pad", g_grid_file);

    if (g_output_profile_file)
    {
        output_profile_to_csv(*g_output_profile_file, result);
    }

    // const lvox::Bounds bounds = lvox::algorithms::compute_scene_bounds(scans);
    // lvox::algorithms::ComputeData data{
    //     .m_counts{bounds, compute_options.voxel_size},
    //     .m_lengths{bounds, compute_options.voxel_size},
    //     .m_hits{{bounds, compute_options.voxel_size}},
    // };

    // for (const auto& scan : scans)
    // {
    //     lvox::algorithms::compute_rays_count_and_length(scan, data, compute_options);
    // }

    // logger.info("Writing output HDF5 file");
    // lvox::h5_exporter::export_grid(*data.m_hits, "hits", file);
    // lvox::h5_exporter::export_grid(data.m_counts, "counts", file);
    //     lvox::h5_exporter::export_grid(data.m_lengths, "lengths", file);
}
