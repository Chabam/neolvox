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

auto print_usage() -> void
{
    std::cout << "Usage: lvox POINT_CLOUD_FILE" << std::endl;
}

auto main(int argc, char* argv[]) -> int
{
    if (argc < 2)
    {
        std::cout << "Invalid amount of arguments supplied" << std::endl;
        print_usage();
        return 1;
    }
    lvox::Logger logger{"LVOX Standalone"};

    namespace fs = std::filesystem;
    fs::path file{argv[1]};

    pdal::Options options;
    options.add("filename", file.c_str());

    pdal::PointTable pts_table;
    pts_table.layout()->registerDim(pdal::Dimension::Id::X);
    pts_table.layout()->registerDim(pdal::Dimension::Id::Y);
    pts_table.layout()->registerDim(pdal::Dimension::Id::Z);

    pdal::PointViewPtr view{std::make_shared<pdal::PointView>(pts_table)};

    std::unique_ptr<pdal::Reader> file_reader;
    const std::string             extension = file.extension().string().substr(1);
    if (extension == "las" or extension == "laz")
    {
        file_reader = std::make_unique<pdal::LasReader>();
    }
    else
    {
        std::cout << std::format("Unsupported file extension: '{}'", extension) << std::endl;
        return 1;
    }

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
        scans->get_points(), scans->get_scan_position({}), data, compute_options
    );

    logger.info("Writing output HDF5 file");
    lvox::h5_exporter::export_grid(*data.m_hits, "hits", file);
    lvox::h5_exporter::export_grid(data.m_counts, "counts", file);
    lvox::h5_exporter::export_grid(data.m_lengths, "lengths", file);
}
