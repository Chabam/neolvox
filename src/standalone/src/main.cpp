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

#include<lvox/algorithms/algorithms.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/tls_scan.hpp>
#include <lvox/voxel/h5_exporter.hpp>
#include "lvox/scanner/trajectory.hpp"

auto main(int argc, char* argv[]) -> int
{
    namespace fs = std::filesystem;
    using dim    = pdal::Dimension::Id;

    std::vector<std::string> args{argv, argv+argc};

    if (args.size() < 2)
    {
        std::cout << "Invalid amount of arguments supplied" << std::endl;
        std::cout << "Usage: lvox POINT_CLOUD_FILE" << std::endl;
        return 1;
    }

    bool is_mls = false;
    fs::path traj_file;
    fs::path file;
    auto arg_it = args.begin();
    while(arg_it != args.end())
    {
        // TODO: handle this better? Or just make it PDAL plugin
        if (*arg_it == "-m")
        {
            is_mls = false;
            traj_file = *(arg_it+1);
        }
        else
        {
            file = *arg_it;
        }
    }

    lvox::Logger logger{"LVOX Standalone"};
    lvox::Trajectory traj(file);

    return 0;

    pdal::Options options;
    options.add("filename", file.c_str());

    pdal::PointTable pts_table;
    pts_table.layout()->registerDim(dim::X);
    pts_table.layout()->registerDim(dim::Y);
    pts_table.layout()->registerDim(dim::Z);
    pts_table.layout()->registerDim(dim::GpsTime);

    pdal::PointViewPtr view{std::make_shared<pdal::PointView>(pts_table)};
    auto stage_factory = std::make_unique<pdal::StageFactory>();

    const std::string driver = pdal::StageFactory::inferReaderDriver(file);

    if (driver.empty())
    {
        throw std::runtime_error(
            std::format("Cannot determine reader for input file: {}", file.c_str())
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

    const auto pts_view_set = file_reader->execute(pts_table);
    const auto voxel_size   = 0.1;

    Eigen::Vector3d scan_origin = lvox::Vector::Constant(0.); // (0,0,0)
    const auto      scans = std::make_shared<lvox::TLSScan>(*pts_view_set.begin(), scan_origin);
    const lvox::algorithms::ComputeOptions compute_options{.voxel_size = 0.1};
    // const lvox::algorithms::PadResult result      = lvox::algorithms::compute_pad(
    //     {scans},
    //     lvox::algorithms::PADComputeOptions{compute_options}
    // );
    const lvox::Bounds bounds = lvox::algorithms::compute_scene_bounds({scans}, compute_options);
    lvox::algorithms::ComputeData data{
        .m_counts{bounds, compute_options.voxel_size},
        .m_lengths{bounds, compute_options.voxel_size},
        .m_hits{{bounds, compute_options.voxel_size}},
    };
    lvox::algorithms::compute_rays_count_and_length(
        *scans, data, compute_options
    );

    logger.info("Writing output HDF5 file");
    lvox::h5_exporter::export_grid(*data.m_hits, "hits", file);
    lvox::h5_exporter::export_grid(data.m_counts, "counts", file);
    lvox::h5_exporter::export_grid(data.m_lengths, "lengths", file);
}
