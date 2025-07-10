#include <filesystem>
#include <format>
#include <iostream>
#include <memory>

#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/QuickInfo.hpp>
#include <pdal/StageFactory.hpp>
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
   -h, --help                             Prints this message
)";

auto main(int argc, char* argv[]) -> int
{
    namespace fs = std::filesystem;
    using dim    = pdal::Dimension::Id;

    std::vector<std::string> args{argv + 1, argv + argc};
    lvox::Logger             logger{"LVOX Standalone"};

    if (args.empty())
    {
        logger.error("Invalid amount of arguments supplied");
        std::cout << g_usage_info << std::endl;
        return 1;
    }

    bool            is_mls      = false;
    double          voxel_size  = 0.1;
    Eigen::Vector3d scan_origin = lvox::Vector::Constant(0.); // (0,0,0)

    fs::path traj_file;
    fs::path file;

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

    if (!fs::exists(file) || !fs::is_regular_file(file))
    {
        logger.error(
            "Provided point cloud file doesn't exists or is not a file: {}", file.string()
        );
        return 1;
    }

    if (is_mls)
    {
        if (!fs::exists(traj_file) || !fs::is_regular_file(traj_file))
        {
            logger.error(
                "Provided trajectory file doesn't exists or is not a file: {}", traj_file.string()
            );
            return 1;
        }

        if (scan_origin != lvox::Vector::Constant(0.))
        {
            logger.warn("Scan origin being set while in MLS mode, ignoring it");
        }
    }

    pdal::Options options;
    options.add("filename", file.string());

    pdal::PointTable pts_table;
    auto layout = pts_table.layout();
    layout->registerDim(dim::X);
    layout->registerDim(dim::Y);
    layout->registerDim(dim::Z);
    layout->registerDim(dim::GpsTime);

    pdal::PointViewPtr view{std::make_shared<pdal::PointView>(pts_table)};
    auto               stage_factory = std::make_unique<pdal::StageFactory>();

    const std::string driver = pdal::StageFactory::inferReaderDriver(file.string());

    if (driver.empty())
    {
        throw std::runtime_error(
            std::format("Cannot determine reader for input file: {}", file.string())
        );
    }

    pdal::Stage* file_reader = stage_factory->createStage(driver);

    file_reader->setOptions(options);
    file_reader->prepare(pts_table);

    logger.info(
        R"(
Loading file        {}
Amount of points    {})",
        file.filename().string(),
        file_reader->preview().m_pointCount
    );

    const auto          pts_view_set = file_reader->execute(pts_table);
    lvox::ScannerOrigin scanner_origin;

    if (is_mls)
    {
        logger.info("Loading trajectory file {}", traj_file.string());
        scanner_origin = std::make_shared<lvox::Trajectory>(traj_file);
    }
    else
    {
        scanner_origin = scan_origin;
    }

    lvox::Scan scan{*pts_view_set.begin(), scanner_origin};

    const lvox::algorithms::ComputeOptions compute_options{.voxel_size = voxel_size};
    // const lvox::algorithms::PadResult result = lvox::algorithms::compute_pad(
    //     {scan},
    //     lvox::algorithms::PADComputeOptions{compute_options}
    // );
    // lvox::h5_exporter::export_grid(result, "pad", file);

    const lvox::Bounds bounds = lvox::algorithms::compute_scene_bounds({scan}, compute_options);
      lvox::algorithms::ComputeData data{
      .m_counts{bounds, compute_options.voxel_size},
      .m_lengths{bounds, compute_options.voxel_size},
      .m_hits{{bounds, compute_options.voxel_size}},
      };
      lvox::algorithms::compute_rays_count_and_length(scan, data, compute_options);

    logger.info("Writing output HDF5 file");
    lvox::h5_exporter::export_grid(*data.m_hits, "hits", file);
    lvox::h5_exporter::export_grid(data.m_counts, "counts", file);
    lvox::h5_exporter::export_grid(data.m_lengths, "lengths", file);
}
