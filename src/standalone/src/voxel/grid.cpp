#include <H5Cpp.h>
#include <atomic>
#include <execution>
#include <format>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

Grid::Grid(const Bounds& bounds, double cell_size, bool compute_variance)
    : m_cell_size{cell_size}
    , m_dim_x{Grid::adjust_dim_to_grid(bounds.maxx - bounds.minx)}
    , m_dim_y{Grid::adjust_dim_to_grid(bounds.maxy - bounds.miny)}
    , m_dim_z{Grid::adjust_dim_to_grid(bounds.maxz - bounds.minz)}
    , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
    , m_effective_chunk_count{0}
    , m_chunks_x{static_cast<unsigned int>(
        std::ceil(static_cast<float>(m_dim_x) / VoxelChunk::s_max_edge_size)
    )}
    , m_chunks_y{static_cast<unsigned int>(
        std::ceil(static_cast<float>(m_dim_y) / VoxelChunk::s_max_edge_size)
    )}
    , m_chunks_z{static_cast<unsigned int>(
        std::ceil(static_cast<float>(m_dim_z) / VoxelChunk::s_max_edge_size)
    )}
    , m_chunk_count{m_chunks_x * m_chunks_y * m_chunks_z}
    , m_chunks{m_chunk_count}
    , m_bounds{
          bounds.minx,
          bounds.miny,
          bounds.minz,
          Grid::adjust_bounds_to_grid(m_dim_x, bounds.minx),
          Grid::adjust_bounds_to_grid(m_dim_y, bounds.miny),
          Grid::adjust_bounds_to_grid(m_dim_z, bounds.minz)
    }
{

    Logger logger{"Grid"};

    logger.debug(
        g_grid_loginfo,
        m_dim_x,
        m_dim_y,
        m_dim_z,
        m_cell_size,
        cell_count(),
        m_bounds.minx,
        m_bounds.maxx,
        m_bounds.miny,
        m_bounds.maxy,
        m_bounds.minz,
        m_bounds.maxz
    );
}

Grid::Grid(Grid&& other)
    : m_cell_size{std::move(other.m_cell_size)}
    , m_dim_x{std::move(other.m_dim_x)}
    , m_dim_y{std::move(other.m_dim_y)}
    , m_dim_z{std::move(other.m_dim_z)}
    , m_cell_count{std::move(other.m_cell_count)}
    , m_chunks_x{std::move(other.m_chunks_x)}
    , m_chunks_y{std::move(other.m_chunks_y)}
    , m_chunks_z{std::move(other.m_chunks_z)}
    , m_chunk_count{std::move(other.m_chunk_count)}
    , m_chunks{std::move(other.m_chunks)}
    , m_bounds{std::move(other.m_bounds)}
{
}

Grid::VoxelChunk::VoxelChunk(
    unsigned int dim_x, unsigned int dim_y, unsigned int dim_z
)
    : m_dim_x{dim_x}
    , m_dim_y{dim_y}
    , m_dim_z{dim_z}
    , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
    , m_hits{m_cell_count, std::allocator<a_u32>{}}
    , m_counts{m_cell_count, std::allocator<a_u32>{}}
    , m_lengths{m_cell_count, std::allocator<a_dbl>{}}
    , m_lengths_variances{m_cell_count, std::allocator<a_dbl>{}}
    , m_pad{m_cell_count, std::allocator<a_dbl>{}}
{
}

auto Grid::voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> Bounds
{
    const double min_x = m_bounds.minx + idx_x * m_cell_size;
    const double min_y = m_bounds.miny + idx_y * m_cell_size;
    const double min_z = m_bounds.minz + idx_z * m_cell_size;
    return Bounds{
        //
        min_x,
        min_y,
        min_z,
        min_x + m_cell_size,
        min_y + m_cell_size,
        min_z + m_cell_size
    };
}

auto Grid::voxel_bounds_from_point(const Point& point) -> Bounds
{
    const auto [idx_x, idx_y, idx_z] = index3d_of_point(point);
    return voxel_bounds(idx_x, idx_y, idx_z);
}

// Return an index tuple of this layout (x, y, z)
auto Grid::index3d_of_point(const Point& point) const -> Index3D
{
    const double x = point.x();
    const double y = point.y();
    const double z = point.z();

    if (!m_bounds.contains(x, y, z))
    {
        throw std::runtime_error{
            std::format("Provided coords are not in the grid: ({}, {}, {})", x, y, z)
        };
    }

    const auto coords_to_index =
        [cell_size = m_cell_size](double min, double max, double coord) -> unsigned int {
        const double result = std::floor((coord - min) / cell_size);
        if (coord == max)
            return result - 1;
        return result;
    };

    return {
        coords_to_index(m_bounds.minx, m_bounds.maxx, x),
        coords_to_index(m_bounds.miny, m_bounds.maxy, y),
        coords_to_index(m_bounds.minz, m_bounds.maxz, z)
    };
}

auto Grid::adjust_dim_to_grid(double distance) -> unsigned int
{
    return static_cast<unsigned int>(std::ceil(distance / m_cell_size));
}

auto Grid::adjust_bounds_to_grid(size_t dim, double min) const -> double
{
    return min + dim * m_cell_size;
}

auto Grid::get_or_create_chunk(const Index3D& voxel_idx) -> a_chunk_ptr&
{
    const auto chunk_idx = index3d_to_chunk_idx(voxel_idx);
    auto& chunk_ref = m_chunks.at(chunk_idx);

    if (chunk_ref.load())
    {
        return chunk_ref;
    }

    const auto& [x, y, z] = voxel_idx;

    const auto chunk_x_idx = x / VoxelChunk::s_max_edge_size;
    const auto chunk_y_idx = y / VoxelChunk::s_max_edge_size;
    const auto chunk_z_idx = z / VoxelChunk::s_max_edge_size;

    constexpr auto max_edge = static_cast<unsigned int>(VoxelChunk::s_max_edge_size);
    const auto chunk_dim_x = std::min(max_edge, m_dim_x - chunk_x_idx);
    const auto chunk_dim_y = std::min(max_edge, m_dim_y - chunk_y_idx);
    const auto chunk_dim_z = std::min(max_edge, m_dim_z - chunk_z_idx);

    auto new_chunk =
        std::make_shared<VoxelChunk>(chunk_dim_x, chunk_dim_y, chunk_dim_z);
    std::shared_ptr<VoxelChunk> empty_chunk;

    // We don't really care wether the value was exchanged or not. We
    // just don't want to override the existing value if it wasn't an
    // empty chunk.
    //
    // TODO: think about what memory order should be used
    chunk_ref.compare_exchange_strong(empty_chunk, new_chunk);

    return chunk_ref;
}

auto Grid::index3d_to_chunk_idx(const Index3D& voxel_idx) -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    constexpr auto max_edge = VoxelChunk::s_max_edge_size;
    const auto chunk_x_idx = x / max_edge;
    const auto chunk_y_idx = y / max_edge;
    const auto chunk_z_idx = z / max_edge;

    return chunk_x_idx + chunk_y_idx * m_chunks_x + chunk_z_idx * m_chunks_x * m_chunks_y;
}

auto Grid::index3d_to_chunk_flat_idx(const a_chunk_ptr& chunk, const Index3D& voxel_idx) const -> unsigned int
{
    const auto& [x, y, z] = voxel_idx;

    const auto chunk_dim_x = chunk.load(std::memory_order_relaxed)->m_dim_x;
    const auto chunk_dim_y = chunk.load(std::memory_order_relaxed)->m_dim_y;
    const auto chunk_dim_z = chunk.load(std::memory_order_relaxed)->m_dim_z;

    constexpr auto max_edge = VoxelChunk::s_max_edge_size;
    const auto chunk_x_idx = x / max_edge;
    const auto chunk_y_idx = y / max_edge;
    const auto chunk_z_idx = z / max_edge;

    const auto rel_chunk_x_idx = (x - chunk_x_idx * max_edge);
    const auto rel_chunk_y_idx = (y - chunk_y_idx * max_edge);
    const auto rel_chunk_z_idx = (z - chunk_z_idx * max_edge);

    return rel_chunk_x_idx + rel_chunk_y_idx * chunk_dim_x + rel_chunk_z_idx * chunk_dim_x * chunk_dim_y;
}

auto Grid::register_hit(const Index3D& idx) -> void
{

    auto&      chunk              = get_or_create_chunk(idx);
    const auto voxel_idx_in_chunk = index3d_to_chunk_flat_idx(chunk, idx);

    chunk.load(std::memory_order_relaxed)
        ->m_hits.at(voxel_idx_in_chunk)
        .fetch_add(1, std::memory_order_relaxed);
}

auto Grid::add_length_and_count(const Index3D& idx, double length) -> void
{
    auto&      chunk              = get_or_create_chunk(idx);
    const auto voxel_idx_in_chunk = index3d_to_chunk_flat_idx(chunk, idx);

    chunk.load(std::memory_order_relaxed)
        ->m_lengths[voxel_idx_in_chunk]
        .fetch_add(length, std::memory_order_relaxed);
    chunk.load(std::memory_order_relaxed)
        ->m_counts[voxel_idx_in_chunk]
        .fetch_add(1, std::memory_order_relaxed);
}

auto Grid::add_length_count_and_variance(const Index3D& idx, double length) -> void
{
    auto&      chunk              = get_or_create_chunk(idx);
    const auto voxel_idx_in_chunk = index3d_to_chunk_flat_idx(chunk, idx);

    chunk.load(std::memory_order_relaxed)
        ->m_lengths[voxel_idx_in_chunk]
        .fetch_add(length, std::memory_order_relaxed);

    chunk.load(std::memory_order_relaxed)
        ->m_counts[voxel_idx_in_chunk]
        .fetch_add(1, std::memory_order_relaxed);

    // TODO: Compute variance, will probably require a mutex to ensure order :/
}

auto Grid::compute_pad(algorithms::pe::BeerLambert) -> void
{
    compute_pad_impl(
        [this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double hits            = chunk->m_hits[voxel_idx];
            const double ray_count       = chunk->m_counts[voxel_idx];
            const double RDI             = hits / static_cast<double>(ray_count);
            const double ray_length      = chunk->m_lengths[voxel_idx];
            const double mean_ray_length = ray_length / ray_count;

            if (RDI >= 1.)
                return 0.;

            return -(std::log(1. - RDI) / G(mean_ray_length));
        }
    );
}

auto Grid::compute_pad(algorithms::pe::ContactFrequency) -> void
{
    compute_pad_impl(
        [this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double hits       = chunk->m_hits[voxel_idx];
            const double ray_count  = chunk->m_counts[voxel_idx];
            const double RDI        = hits / static_cast<double>(ray_count);
            const double ray_length = chunk->m_lengths[voxel_idx];

            if (RDI >= 1.)
                return 0.;

            return RDI / G(ray_length);
        }
    );
}

auto Grid::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
{
    compute_pad_impl(
        [this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double hits               = chunk->m_hits[voxel_idx];
            const double ray_count          = chunk->m_counts[voxel_idx];
            const double RDI                = hits / static_cast<double>(ray_count);
            const double length             = chunk->m_lengths[voxel_idx];
            const double variance           = chunk->m_lengths_variances[voxel_idx];
            const double mean_ray_length    = length / ray_count;
            const double unequal_path_ratio = variance / mean_ray_length;

            double attenuation_coeff;

            if (RDI < 1.)
            {
                const double inv_RDI = 1. - RDI;
                attenuation_coeff =
                    (1. / mean_ray_length) * (std::log(inv_RDI) - (1. / (2 * ray_count *
                    inv_RDI)));
            }
            else // RDI == 1.
            {
                attenuation_coeff = std::log(2 * ray_count + 2) / mean_ray_length;
            }

            const double res = (1. / G(attenuation_coeff)) *
                               (1. - std::sqrt(1. - 2 * unequal_path_ratio * attenuation_coeff));

            if (std::isnan(res))
            {
                std::cout << std::format(
                                 R"(
    hits                = {}
    ray_count           = {}
    RDI                 = {}
    ray_length          = {}
    mean_ray_length     = {}
    ray_length_variance = {}
    unequal_path_ratio  = {}
    attenuation_coeff   = {})",
                                 hits,
                                 ray_count,
                                 RDI,
                                 length,
                                 mean_ray_length,
                                 variance,
                                 unequal_path_ratio,
                                 attenuation_coeff
                             )
                          << std::endl;
                return 0.;
            }

            return res;
        }
    );
}

// auto Grid::flat_index_to_index3d(size_t i) const -> Index3D
// {
//     return {
//         static_cast<unsigned int>(i % m_dim_x),
//         static_cast<unsigned int>((i / m_dim_x) % m_dim_y),
//         static_cast<unsigned int>((i / m_dim_x) / m_dim_y)
//     };
// }

auto Grid::export_as_coo_to_h5(
    const std::string& dataset_name, const std::filesystem::path& filename, bool include_all_data
) const -> void
{
    // H5::H5File file;
    // if (std::filesystem::exists(filename))
    // {
    //     file = H5::H5File{filename.string(), H5F_ACC_RDWR};
    // }
    // else
    // {
    //     file = H5::H5File{filename.string(), H5F_ACC_TRUNC};
    // }

    // const auto voxel_size      = m_cell_size;
    // auto       index_with_data = m_counts | std::views::enumerate |
    //                              std::views::filter([this](const auto& pair) -> bool {
    //                                  return m_counts[std::get<0>(pair)] == 0;
    //                              }) |
    //                              std::views::elements<0>;
    // auto index3d_with_data =
    //     index_with_data | std::views::transform([this](const size_t& index) -> Index3D {
    //         return flat_index_to_index3d(index);
    //     });

    // const hsize_t voxels_with_data = std::ranges::distance(index_with_data);

    // using h5_dimension_t = std::array<hsize_t, 1>;
    // const h5_dimension_t voxel_count_dim{voxels_with_data};

    // const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, voxel_count_dim.data()};

    // H5::Group plot_group;
    // if (file.nameExists(dataset_name))
    // {
    //     plot_group = file.openGroup(dataset_name);
    // }
    // else
    // {
    //     plot_group = file.createGroup(dataset_name, 6);
    // }

    // H5::DSetCreatPropList create_prop_list{};
    // h5_dimension_t        chunk_dims{std::min(static_cast<hsize_t>(2 << 13), voxels_with_data)};
    // create_prop_list.setChunk(1, chunk_dims.data());

    // const auto get_or_create_dataset =
    //     [&plot_group, &dataset_name, &create_prop_list](
    //         const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
    //     ) -> H5::DataSet {
    //         if (plot_group.nameExists(name))
    //         {
    //             return plot_group.openDataSet(name);
    //         }

    //         return plot_group.createDataSet(name, type, dataspace, create_prop_list);
    //     };

    // // Writing indexes
    // const auto h5_size_t = H5::PredType::NATIVE_HSIZE;
    // {
    //     const std::vector<size_t> xs =
    //         index3d_with_data | std::views::elements<0> | std::ranges::to<std::vector<size_t>>();

    //     H5::DataSet xs_data = get_or_create_dataset("x", h5_size_t, data_space);
    //     xs_data.write(xs.data(), h5_size_t);
    // }

    // {
    //     const std::vector<size_t> ys =
    //         index3d_with_data | std::views::elements<1> | std::ranges::to<std::vector<size_t>>();

    //     H5::DataSet ys_data = get_or_create_dataset("y", h5_size_t, data_space);
    //     ys_data.write(ys.data(), h5_size_t);
    // }

    // {
    //     const std::vector<size_t> zs =
    //         index3d_with_data | std::views::elements<2> | std::ranges::to<std::vector<size_t>>();

    //     H5::DataSet zs_data = get_or_create_dataset("z", h5_size_t, data_space);
    //     zs_data.write(zs.data(), h5_size_t);
    // }

    // {
    //     H5::PredType h5_pad_t =  H5::PredType::NATIVE_DOUBLE;
    //     const std::vector<double> pad_values =
    //         index_with_data | std::views::transform([this](const size_t& index) -> double {
    //             return m_pad[index];
    //         }) |
    //         std::ranges::to<std::vector<double>>();

    //     H5::DataSet values_data = get_or_create_dataset("pad", h5_pad_t, data_space);
    //     values_data.write(pad_values.data(), h5_pad_t);
    // }

    // if (include_all_data)
    // {
    //     H5::PredType h5_counts_t =  H5::PredType::NATIVE_UINT;
    //     const std::vector<double> counts =
    //         index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
    //             return m_counts[index];
    //         }) |
    //         std::ranges::to<std::vector<double>>();

    //     H5::DataSet counts_data = get_or_create_dataset("counts", h5_counts_t, data_space);
    //     counts_data.write(counts.data(), h5_counts_t);

    //     H5::PredType h5_lengths_t =  H5::PredType::NATIVE_DOUBLE;
    //     const std::vector<double> lengths =
    //         index_with_data | std::views::transform([this](const size_t& index) -> unsigned int {
    //             return m_lengths[index];
    //         }) |
    //         std::ranges::to<std::vector<double>>();

    //     H5::DataSet lengths_data = get_or_create_dataset("lengths", h5_lengths_t, data_space);
    //     lengths_data.write(lengths.data(), h5_lengths_t);

    //     if (m_lengths_variances)
    //     {
    //         H5::PredType h5_lengths_var_t =  H5::PredType::NATIVE_DOUBLE;
    //         const std::vector<double> lengths_variance =
    //             index_with_data | std::views::transform([this](const size_t& index) -> unsigned
    //             int {
    //                 return m_lengths[index];
    //             }) |
    //             std::ranges::to<std::vector<double>>();

    //         H5::DataSet lengths_variance_data = get_or_create_dataset("lengths variance",
    //         h5_lengths_var_t, data_space); lengths_variance_data.write(lengths_variance.data(),
    //         h5_lengths_var_t);

    //     }
    // }

    // // Minimum coordinate attribute
    // const std::array<hsize_t, 1> scalar_value_dim{1};
    // H5::DataSpace                scalar_data_space{1, scalar_value_dim.data()};
    // const std::array<hsize_t, 1> singular_3d_coord_count = {3};
    // H5::DataSpace                singular_3d_coord_data_space{1, singular_3d_coord_count.data()};

    // const auto get_or_create_attribute =
    //     [&plot_group](
    //         const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
    //     ) -> H5::Attribute {
    //         if (plot_group.attrExists(name))
    //         {
    //             return plot_group.openAttribute(name);
    //         }

    //         return plot_group.createAttribute(name, type, dataspace);
    //     };
    // const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    // H5::Attribute voxel_size_attr =
    //     get_or_create_attribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    // voxel_size_attr.write(h5_voxel_size_t, &voxel_size);

    // const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    // H5::Attribute min_coord_attr = get_or_create_attribute(
    //     "Minimal coordinates values", h5_min_coords_t, singular_3d_coord_data_space
    // );

    // const std::array<double, 3> min_coords = {m_bounds.minx, m_bounds.miny, m_bounds.minz};
    // min_coord_attr.write(h5_voxel_size_t, min_coords.data());

    // // Grid dimensions
    // const auto    h5_grid_dim_t = H5::PredType::NATIVE_UINT64;
    // H5::Attribute grid_dims_attr =
    //     get_or_create_attribute("Dimensions", h5_grid_dim_t, singular_3d_coord_data_space);
    // const std::array<size_t, 3> grid_dims = {m_dim_x, m_dim_y, m_dim_z};
    // grid_dims_attr.write(h5_grid_dim_t, grid_dims.data());

    // file.close();
}

} // namespace lvox
