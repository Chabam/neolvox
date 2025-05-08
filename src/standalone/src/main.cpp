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

#include <lvox/lvox.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/tls_scan.hpp>

auto print_usage() -> void
{

    std::cout << "Usage: lvox POINT_CLOUD_FILE" << std::endl;
}

auto print_pts_cloud_metadata(const std::filesystem::path& filename, const pdal::QuickInfo& info)
    -> void
{
    std::cout << std::format(
                     "Loading:\t\t{}\nAmount of points:\t{}",
                     filename.filename().string(),
                     info.m_pointCount
                 )
              << std::endl;
}

auto main(int argc, char* argv[]) -> int
{
    if (argc < 2)
    {
        std::cout << "Invalid amount of arguments supplied" << std::endl;
        print_usage();
        return 1;
    }

    namespace fs = std::filesystem;
    fs::path filename{argv[1]};

    pdal::Options options;
    options.add("filename", filename.c_str());

    pdal::PointTable pts_table;
    pts_table.layout()->registerDim(pdal::Dimension::Id::X);
    pts_table.layout()->registerDim(pdal::Dimension::Id::Y);
    pts_table.layout()->registerDim(pdal::Dimension::Id::Z);

    pdal::PointViewPtr view{std::make_shared<pdal::PointView>(pts_table)};

    std::unique_ptr<pdal::Reader> file_reader;
    const std::string             extension = filename.extension().string().substr(1);
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

    print_pts_cloud_metadata(filename, file_reader->preview());

    const auto pts_view_set = file_reader->execute(pts_table);

    Eigen::Vector3d scan_origin = lvox::Vector::Constant(0.); // (0,0,0)
    lvox::compute_pad(
        {std::make_shared<lvox::TLSScan>(*pts_view_set.begin(), scan_origin)},
        lvox::LvoxOptions{
            .voxel_size = .1

        }
    );
}
