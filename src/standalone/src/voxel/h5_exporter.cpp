#include <H5Fpublic.h>
#include <H5public.h>
#include <cstdint>
#include <filesystem>
#include <ranges>

#include <lvox/voxel/h5_exporter.hpp>

#include <lvox/voxel/grid.hpp>

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

    const auto    voxel_size       = grid.cell_size();
    const hsize_t voxels_with_data = std::distance(grid.begin(), grid.end());

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
        h5_data_t = H5::PredType::NATIVE_INT32;
    }

    H5::Group plot_group = output.createGroup(dataset_name, 6);

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{1000};
    create_prop_list.setChunk(1, chunk_dims.data());
    create_prop_list.setDeflate(6);
    create_prop_list.setFillValue(h5_data_t, 0);

    // Writing indexes
    {
        const auto indexes =
            grid | std::views::keys | std::views::transform([&grid](Index index) -> Index3D {
                return grid.index_to_index3d(index);
            });

        const std::vector<size_t> xs =
            indexes | std::views::elements<0> | std::ranges::to<std::vector<size_t>>();
        const std::vector<size_t> ys =
            indexes | std::views::elements<1> | std::ranges::to<std::vector<size_t>>();
        const std::vector<size_t> zs =
            indexes | std::views::elements<2> | std::ranges::to<std::vector<size_t>>();

        const auto  h5_size_t = H5::PredType::NATIVE_HSIZE;
        H5::DataSet xs_data =
            plot_group.createDataSet("x", h5_size_t, data_space, create_prop_list);
        xs_data.write(xs.data(), h5_size_t);

        H5::DataSet ys_data =
            plot_group.createDataSet("y", h5_size_t, data_space, create_prop_list);
        ys_data.write(ys.data(), h5_size_t);

        H5::DataSet zs_data =
            plot_group.createDataSet("z", h5_size_t, data_space, create_prop_list);
        zs_data.write(zs.data(), h5_size_t);
    }

    // Writing values
    {
        const auto values = grid | std::views::values | std::ranges::to<std::vector<cell_t>>();

        H5::DataSet values_data =
            plot_group.createDataSet("values", h5_data_t, data_space, create_prop_list);
        values_data.write(values.data(), h5_data_t);
    }

    const hsize_t                singular_coord_dim[] = {1};
    H5::DataSpace                singular_coord_data_space{1, singular_coord_dim};
    const std::array<hsize_t, 3> singular_3d_coord_count = {1, 1, 1};

    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        plot_group.createAttribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &voxel_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::DataSpace min_coords_data_space{1, singular_coord_dim};
    H5::Attribute min_coord_attr = plot_group.createAttribute(
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
