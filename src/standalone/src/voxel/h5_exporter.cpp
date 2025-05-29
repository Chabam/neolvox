#include <H5Fpublic.h>
#include <lvox/voxel/h5_exporter.hpp>

namespace lvox::h5_exporter
{

struct H5GridCreationOptions
{
    std::string  m_filename;
    std::string  m_dataset;
    H5::PredType m_type;
};

template <typename GridType>
auto write_grid_to_h5(const H5GridCreationOptions& grid_creation_options, const GridType& grid)
    -> void
{
    H5::H5File output{grid_creation_options.m_filename, H5F_ACC_CREAT | H5F_ACC_RDWR};

    const auto voxel_size = grid.cell_size();

    using h5_dimension_t = std::array<hsize_t, 3>;
    h5_dimension_t dimensions{grid.dim_x(), grid.dim_y(), grid.dim_z()};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, dimensions.data()};

    const auto h5_data_t = grid_creation_options.m_type;

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{20, 20, 20};
    create_prop_list.setChunk(3, chunk_dims.data());
    create_prop_list.setDeflate(6);
    create_prop_list.setFillValue(h5_data_t, 0);

    H5::DataSet dataset = output.createDataSet(
        grid_creation_options.m_dataset, h5_data_t, data_space, create_prop_list
    );

    const hsize_t        singular_coord_dim[] = {1};
    H5::DataSpace        singular_coord_data_space{1, singular_coord_dim};
    const h5_dimension_t singular_3d_coord_count = {1, 1, 1};

    for (const auto& [idx, pad_value] : grid)
    {
        h5_dimension_t offset = grid.index_to_index3d(idx);
        data_space.selectHyperslab(H5S_SELECT_SET, singular_3d_coord_count.data(), offset.data());

        dataset.write(&pad_value, h5_data_t, singular_coord_data_space, data_space);
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

    const lvox::Bounds&         bounds     = grid.bounds();
    const std::array<double, 3> min_coords = {bounds.minx, bounds.miny, bounds.minz};
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());
    output.close();
}

auto export_grid(
    const GridD& result, const std::string& dataset, const std::filesystem::path& in_file
) -> void
{

    const std::string     filename = in_file.filename().string();
    H5GridCreationOptions grid_creation_options{
        .m_filename = std::format("{}_lvox_out.h5", filename.substr(0, filename.find_last_of("."))),
        .m_dataset  = dataset,
        .m_type     = H5::PredType::NATIVE_DOUBLE
    };

    write_grid_to_h5(grid_creation_options, result);
}

auto export_grid(
    const GridU32& result, const std::string& dataset, const std::filesystem::path& in_file
) -> void
{

    const std::string     filename = in_file.filename().string();
    H5GridCreationOptions grid_creation_options{
        .m_filename = std::format("{}_lvox_out.h5", filename.substr(0, filename.find_last_of("."))),
        .m_dataset  = dataset,
        .m_type     = H5::PredType::NATIVE_INT32
    };

    write_grid_to_h5(grid_creation_options, result);
}

} // namespace lvox::h5_exporter
