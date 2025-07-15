#include <H5Cpp.h>
#include <H5Fpublic.h>
#include <H5public.h>

#include <filesystem>
#include <ranges>

#include <lvox/voxel/grid.hpp>
#include <lvox/voxel/h5_exporter.hpp>

namespace lvox::h5_exporter
{

template <typename GridType>
auto write_grid_as_coo_matrix_to_h5(
    const std::string& filename, const std::string& dataset_name, const GridType& grid
) -> void
{
    H5::H5File output;
    if (std::filesystem::exists(filename))
    {
        output = H5::H5File{filename, H5F_ACC_RDWR};
    }
    else
    {
        output = H5::H5File{filename, H5F_ACC_TRUNC};
    }

    const auto voxel_size = grid.cell_size();
    auto index_with_data = grid | std::views::enumerate |
                           std::views::filter([&grid](const auto& pair) -> bool {
                               return !grid.is_na(std::get<0>(pair));
                           }) |
                           std::views::elements<0>;
    auto index3d_with_data = index_with_data | std::views::transform([&grid](const Index& index) -> Index3D {
        return grid.index_to_index3d(index);
    });

    const hsize_t voxels_with_data = std::ranges::distance(index_with_data);

    using h5_dimension_t = std::array<hsize_t, 1>;
    const h5_dimension_t voxel_count_dim{voxels_with_data};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, voxel_count_dim.data()};

    using cell_t = contained_type<typename GridType::cell_t>::type;
    static_assert(
        std::is_same_v<cell_t, double> || std::is_same_v<cell_t, uint32_t>,
        "Unsupported grid value type"
    );
    H5::PredType h5_data_t = H5::PredType::PREDTYPE_CONST;
    if constexpr (std::is_same_v<cell_t, double>)
    {
        h5_data_t = H5::PredType::NATIVE_DOUBLE;
    }
    else if constexpr (std::is_same_v<cell_t, uint32_t>)
    {
        h5_data_t = H5::PredType::NATIVE_UINT32;
    }

    H5::Group plot_group = output.createGroup(dataset_name, 6);
    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{1000};
    create_prop_list.setChunk(1, chunk_dims.data());
    create_prop_list.setDeflate(6);
    create_prop_list.setFillValue(h5_data_t, 0);

    // Writing indexes
    const auto h5_size_t = H5::PredType::NATIVE_HSIZE;
    {
        const std::vector<size_t> xs =
            index3d_with_data | std::views::elements<0> | std::ranges::to<std::vector<size_t>>();

        H5::DataSet xs_data   = plot_group.createDataSet("x", h5_size_t, data_space, create_prop_list);
        xs_data.write(xs.data(), h5_size_t);
    }

    {
        const std::vector<size_t> ys =
            index3d_with_data | std::views::elements<1> | std::ranges::to<std::vector<size_t>>();

        H5::DataSet ys_data = plot_group.createDataSet("y", h5_size_t, data_space, create_prop_list);
        ys_data.write(ys.data(), h5_size_t);
    }

    {
        const std::vector<size_t> zs =
            index3d_with_data | std::views::elements<2> | std::ranges::to<std::vector<size_t>>();

        H5::DataSet zs_data = plot_group.createDataSet("z", h5_size_t, data_space, create_prop_list);
        zs_data.write(zs.data(), h5_size_t);
    }

    {
        const std::vector<cell_t> values =
            index_with_data | std::views::transform([&grid](const Index& index) -> cell_t {
                return grid.at(index);
            }) | std::ranges::to<std::vector<cell_t>>();

        H5::DataSet values_data =
            plot_group.createDataSet("values", h5_data_t, data_space, create_prop_list);
        values_data.write(values.data(), h5_data_t);
    }

    // Minimum coordinate attribute
    const std::array<hsize_t, 1> scalar_value_dim{1};
    H5::DataSpace                scalar_data_space{1, scalar_value_dim.data()};
    const std::array<hsize_t, 1> singular_3d_coord_count = {3};
    H5::DataSpace                singular_3d_coord_data_space{1, singular_3d_coord_count.data()};

    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        plot_group.createAttribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &voxel_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::Attribute min_coord_attr = plot_group.createAttribute(
        "Minimal coordinates values", h5_min_coords_t, singular_3d_coord_data_space
    );

    const lvox::Bounds&         bounds     = grid.bounds();
    const std::array<double, 3> min_coords = {bounds.minx, bounds.miny, bounds.minz};
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());

    // Grid dimensions
    const auto    h5_grid_dim_t = H5::PredType::NATIVE_UINT64;
    H5::Attribute grid_dims_attr =
        plot_group.createAttribute("Dimensions", h5_grid_dim_t, singular_3d_coord_data_space);
    const std::array<Index, 3> grid_dims = {grid.dim_x(), grid.dim_y(), grid.dim_z()};
    grid_dims_attr.write(h5_grid_dim_t, grid_dims.data());

    output.close();
}

    auto export_grid(
        const GridD& result, const std::string& dataset, const std::filesystem::path& in_file
    ) -> void
    {

        const std::string filename = in_file.filename().string();
        write_grid_as_coo_matrix_to_h5(
            std::format("{}_lvox_out.h5", filename.substr(0, filename.find_last_of("."))),
            dataset,
            result
        );
    }

    auto export_grid(
        const GridU32& result, const std::string& dataset, const std::filesystem::path& in_file
    ) -> void
    {
        const std::string filename = in_file.filename().string();
        write_grid_as_coo_matrix_to_h5(
            std::format("{}_lvox_out.h5", filename.substr(0, filename.find_last_of("."))),
            dataset,
            result
        );
    }

} // namespace lvox::h5_exporter
