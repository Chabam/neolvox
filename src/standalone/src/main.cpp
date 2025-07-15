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
#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/voxel/h5_exporter.hpp>

#include "lvox/scanner/trajectory.hpp"
#include "lvox/types.hpp"

constexpr auto g_usage_info =
    R"(Usage: lvox [OPTIONS] FILE
Options:
   ARGS               EXPECTED VALUES     DESCRIPTION
   -t, --trajectory   filename            Path to a trajectory file, used for computing an MLS scan
   -o, --scan-origin  x y z               Coordinates for the scan position when computing a TLS scan
   -v, --voxel-size   decimal             Coordinates for the scan position when computing a TLS scan
   -p, --profile      filename            Outputs to vertical profile of the voxels of the grid to a csv file
   -h, --help                             Prints this message
    )";

std::mutex g_print_guard;

auto load_point_cloud_from_file(
    const std::filesystem::path&                        file,
    std::optional<std::reference_wrapper<lvox::Bounds>> bounds                    = {},
    std::optional<pdal::Options>                        additional_reader_options = {}
) -> lvox::PointCloudView
{
    using dim    = pdal::Dimension::Id;
    namespace fs = std::filesystem;

    lvox::Logger logger{"Point cloud file reader"};
    if (!fs::exists(file) || !fs::is_regular_file(file))
    {
        throw std::runtime_error{std::format(
            "Provided point cloud file doesn't exists or is not a file: {}", file.string()
        )};
    }

    pdal::Options options;
    options.add("filename", file.string());

    if (additional_reader_options)
    {
        options.add(*additional_reader_options);
    }

    pdal::FixedPointTable pts_table{10'000};
    auto                  layout = pts_table.layout();
    layout->registerDim(dim::X);
    layout->registerDim(dim::Y);
    layout->registerDim(dim::Z);
    layout->registerDim(dim::GpsTime);

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

    pdal::StreamCallbackFilter sc;
    auto                       output = std::make_unique<lvox::PointCloud>();

    const bool calculate_bounds = bounds.has_value();
    sc.setCallback([calculate_bounds, &bounds, &output](const auto& pt) mutable -> bool {
        const double x = pt.template getFieldAs<double>(dim::X);
        const double y = pt.template getFieldAs<double>(dim::Y);
        const double z = pt.template getFieldAs<double>(dim::Z);
        output->emplace_back(pt.template getFieldAs<double>(dim::GpsTime), lvox::Point{x, y, z});

        if (calculate_bounds)
        {
            bounds->get().grow(x, y, z);
        }

        return true;
    });
    sc.setInput(*file_reader);
    sc.prepare(pts_table);

    sc.execute(pts_table);

    return output;
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

        scans.emplace_back(std::async(
            [parent_path, point_cloud_file_name = point_cloud_file_name.str(), x, y, z](
            ) -> lvox::Scan {
                lvox::Bounds bounds;
                return lvox::Scan{
                    .m_points = lvox::PointCloudView{load_point_cloud_from_file(
                        fs::path{parent_path / point_cloud_file_name}, {bounds}
                    )},
                    .m_scanner_origin{lvox::Point{x, y, z}},
                    .m_bounds{bounds}
                };
            }
        ));
    }

    return scans |
           std::views::transform([](std::future<lvox::Scan>& future) -> lvox::Scan {
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
        pdal::Options reader_opts;
        reader_opts.add(
            "header",
            "GpsTime X Y Z Q0 Q1 Q2 Q3 Red Green Blue NormalX NormalY NormalZ Pitch Roll "
            "Azimuth"
        );
        reader_opts.add("skip", 1);
        reader_opts.add("separator", " ");

        scanner_origin = std::make_shared<lvox::Trajectory>(
            load_point_cloud_from_file(*traj_file, {}, reader_opts)
        );
    }
    else if (scan_origin)
    {
        scanner_origin = *scan_origin;
    }
    else
    {
        scanner_origin = lvox::Vector::Constant(0.); // (0,0,0)
    }

    scans.emplace_back(std::move(scan_point_cloud), scanner_origin, scan_bounds);

    return scans;
}

auto main(int argc, char* argv[]) -> int
{
    namespace fs = std::filesystem;

    std::vector<std::string> args{argv + 1, argv + argc};
    lvox::Logger             logger{"LVOX"};

    if (args.empty())
    {
        logger.error("Invalid amount of arguments supplied");
        std::cout << g_usage_info << std::endl;
        return 1;
    }

    bool                       is_mls          = false;
    bool                       outputs_profile = false;
    double                     voxel_size      = 0.1;
    std::optional<lvox::Point> scan_origin;

    fs::path                file;
    std::optional<fs::path> traj_file;

    fs::path output_profile_file;

    auto arg_it = args.begin();
    while (arg_it != args.end())
    {
        // TODO: handle this better? Or just make it PDAL plugin
        if (*arg_it == "-t" || *arg_it == "--trajectory")
        {
            is_mls    = true;
            traj_file = *++arg_it;
        }
        else if (*arg_it == "-v" || *arg_it == "--voxel-size")
        {
            voxel_size = std::stod(*++arg_it);
        }
        else if (*arg_it == "-o" || *arg_it == "--scan-origin")
        {
            scan_origin = {std::stod(*++arg_it), std::stod(*++arg_it), std::stod(*++arg_it)};
        }
        else if (*arg_it == "-p" || *arg_it == "--profile")
        {
            outputs_profile     = true;
            output_profile_file = *++arg_it;
        }
        else if (*arg_it == "-h" || *arg_it == "--help")
        {
            std::cout << g_usage_info << std::endl;
            return 0;
        }
        else
        {
            file = *arg_it;
        }
        ++arg_it;
    }

    std::vector<lvox::Scan> scans;

    if (file.extension() == ".in")
    {
        scans = read_dot_in_file(file);
    }
    else
    {
        scans = create_scan_using_pdal(file, traj_file, scan_origin);
    }

    const lvox::algorithms::ComputeOptions compute_options{.voxel_size = voxel_size};
    const lvox::algorithms::PadResult      result =
        lvox::algorithms::compute_pad(scans, lvox::algorithms::PADComputeOptions{compute_options});

    if (outputs_profile)
    {
        output_profile_to_csv(output_profile_file, result);
    }

    // lvox::h5_exporter::export_grid(result, "pad", file);
    // const lvox::Bounds bounds = lvox::algorithms::compute_scene_bounds(scans);
    // lvox::algorithms::ComputeData data{
    //     .m_counts{bounds, compute_options.voxel_size},
    //     .m_lengths{bounds, compute_options.voxel_size},
    //     .m_hits{{bounds, compute_options.voxel_size}},
    // };
    // lvox::algorithms::compute_rays_count_and_length(scans[0], data, compute_options);

    // logger.info("Writing output HDF5 file");
    // lvox::h5_exporter::export_grid(*data.m_hits, "hits", file);
    // lvox::h5_exporter::export_grid(data.m_counts, "counts", file);
    // lvox::h5_exporter::export_grid(data.m_lengths, "lengths", file);
}
