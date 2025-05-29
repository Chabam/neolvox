#include <H5Cpp.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <ranges>

#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/QuickInfo.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>
#include <pdal/io/LasReader.hpp>

#include <lvox/algorithms.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/tls_scan.hpp>

auto print_usage() -> void
{
    std::cout << "Usage: lvox POINT_CLOUD_FILE" << std::endl;
}

auto create_out_h5_file(const lvox::algorithms::PadResult& result, const std::filesystem::path& in_file) -> void
{
    const std::string filename  = in_file.filename().string();
    const std::string plot_name = filename.substr(0, filename.find_last_of("."));

    std::string h5_file_out = std::format("{}_lvox_out.h5", plot_name);
    H5::H5File  output{h5_file_out, H5F_ACC_TRUNC};
    const auto  voxel_size = result.cell_size();

    using h5_dimension_t = std::array<hsize_t, 3>;
    h5_dimension_t dimensions{result.dim_x(), result.dim_y(), result.dim_z()};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, dimensions.data()};

    const auto h5_pad_t = H5::PredType::NATIVE_DOUBLE;

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{20, 20, 20};
    create_prop_list.setChunk(3, chunk_dims.data());
    create_prop_list.setDeflate(6);
    create_prop_list.setFillValue(h5_pad_t, 0);

    H5::DataSet dataset = output.createDataSet("pad", h5_pad_t, data_space, create_prop_list);

    const hsize_t        singular_coord_dim[] = {1};
    H5::DataSpace        singular_coord_data_space{1, singular_coord_dim};
    const h5_dimension_t singular_3d_coord_count = {1, 1, 1};

    for (const auto& [idx, pad_value] : result)
    {
        h5_dimension_t offset = result.index_to_index3d(idx);
        data_space.selectHyperslab(H5S_SELECT_SET, singular_3d_coord_count.data(), offset.data());

        dataset.write(&pad_value, h5_pad_t, singular_coord_data_space, data_space);
    }

    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        dataset.createAttribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &voxel_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::DataSpace min_coords_data_space{1, singular_coord_dim};
    H5::Attribute min_coord_attr = dataset.createAttribute(
        "Minimal coordinates values", h5_min_coords_t, min_coords_data_space
    );

    const lvox::Bounds&         bounds     = result.bounds();
    const std::array<double, 3> min_coords = {bounds.minx, bounds.miny, bounds.minz};
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());
    output.close();
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

    Eigen::Vector3d                   scan_origin = lvox::Vector::Constant(0.); // (0,0,0)
    const lvox::algorithms::PadResult result      = lvox::algorithms::compute_pad(
        {std::make_shared<lvox::TLSScan>(*pts_view_set.begin(), scan_origin)},
        lvox::algorithms::PADComputeOptions{{.voxel_size = voxel_size}}
    );

    struct MinMax
    {
        double m_min{std::numeric_limits<double>::max()};
        double m_max{std::numeric_limits<double>::min()};
    } min_max;

    for (const auto& value : std::views::values(result))
    {
        min_max.m_min = std::min(min_max.m_min, value.load());
        min_max.m_max = std::max(min_max.m_max, value.load());
    }
    logger.debug("Min {} Max {}", min_max.m_min, min_max.m_max);

    logger.info("Writing output HDF5 file");
    create_out_h5_file(result, file);
}
