#include <H5Cpp.h>
#include <H5Fpublic.h>
#include <atomic>
#include <format>
#include <iterator>

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
    const Index3D& origin_idx, unsigned int dim_x, unsigned int dim_y, unsigned int dim_z
)
    : m_origin_idx{origin_idx}
    , m_dim_x{dim_x}
    , m_dim_y{dim_y}
    , m_dim_z{dim_z}
    , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
    , m_hits{m_cell_count, std::allocator<a_u32>{}}
    , m_counts{m_cell_count, std::allocator<a_u32>{}}
    , m_lengths{m_cell_count, std::allocator<a_dbl>{}}
    , m_lengths_variance{m_cell_count, std::allocator<a_dbl>{}}
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

auto Grid::get_or_create_chunk(const Index3D& chunk_idx) -> a_chunk_ptr&
{
    const auto& [chunk_x, chunk_y, chunk_z] = chunk_idx;
    const auto chunk_flat_idx = chunk_x + chunk_y * m_chunks_x + chunk_z * m_chunks_x * m_chunks_z;

    auto& chunk_ref = m_chunks[chunk_flat_idx];

    if (chunk_ref.load(std::memory_order_acquire))
    {
        return chunk_ref;
    }

    constexpr auto max_edge = static_cast<unsigned int>(VoxelChunk::s_max_edge_size);

    const auto chunk_dim_x = std::min(max_edge, m_dim_x - chunk_x);
    const auto chunk_dim_y = std::min(max_edge, m_dim_y - chunk_y);
    const auto chunk_dim_z = std::min(max_edge, m_dim_z - chunk_z);

    auto new_chunk =
        std::make_shared<VoxelChunk>(chunk_idx, chunk_dim_x, chunk_dim_y, chunk_dim_z);
    std::shared_ptr<VoxelChunk> empty_chunk;

    // We don't really care wether the value was exchanged or not. We
    // just don't want to override the existing value if it wasn't an
    // empty chunk.
    chunk_ref.compare_exchange_strong(empty_chunk, new_chunk, std::memory_order_acq_rel);

    return chunk_ref;
}

auto Grid::index3d_to_chunk_idx(const Index3D& voxel_idx) const -> Index3D
{
    const auto& [x, y, z] = voxel_idx;

    constexpr auto max_edge    = VoxelChunk::s_max_edge_size;
    const auto     chunk_x_idx = x / max_edge;
    const auto     chunk_y_idx = y / max_edge;
    const auto     chunk_z_idx = z / max_edge;

    return Index3D{chunk_x_idx, chunk_y_idx, chunk_z_idx};
}

auto Grid::VoxelChunk::index3d_to_flat_idx(const Index3D& voxel_idx) const -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    constexpr auto max_edge = VoxelChunk::s_max_edge_size;

    const auto rel_chunk_x_idx = x % max_edge;
    const auto rel_chunk_y_idx = y % max_edge;
    const auto rel_chunk_z_idx = z % max_edge;

    return rel_chunk_x_idx + rel_chunk_y_idx * m_dim_x + rel_chunk_z_idx * m_dim_x * m_dim_y;
}

auto Grid::register_hit(const Index3D& idx) -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto&      chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk.load(std::memory_order_relaxed)->index3d_to_flat_idx(idx);

    chunk.load(std::memory_order_relaxed)
        ->m_hits[voxel_idx_in_chunk]
        .fetch_add(1, std::memory_order_relaxed);
}

auto Grid::add_length_and_count(const Index3D& idx, double length) -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto&      chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk.load(std::memory_order_relaxed)->index3d_to_flat_idx(idx);

    chunk.load(std::memory_order_relaxed)
        ->m_lengths[voxel_idx_in_chunk]
        .fetch_add(length, std::memory_order_relaxed);

    chunk.load(std::memory_order_relaxed)
        ->m_counts[voxel_idx_in_chunk]
        .fetch_add(1, std::memory_order_relaxed);
}

auto Grid::add_length_count_and_variance(const Index3D& idx, double length) -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto&      chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk.load(std::memory_order_relaxed)->index3d_to_flat_idx(idx);

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
    compute_pad_impl([this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
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
    });
}

auto Grid::compute_pad(algorithms::pe::ContactFrequency) -> void
{
    compute_pad_impl([this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
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
    });
}

auto Grid::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
{
    compute_pad_impl([this](chunk_ptr& chunk, unsigned int voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits               = chunk->m_hits[voxel_idx];
        const double ray_count          = chunk->m_counts[voxel_idx];
        const double RDI                = hits / static_cast<double>(ray_count);
        const double length             = chunk->m_lengths[voxel_idx];
        const double variance           = chunk->m_lengths_variance[voxel_idx];
        const double mean_ray_length    = length / ray_count;
        const double unequal_path_ratio = variance / mean_ray_length;

        double attenuation_coeff;

        if (RDI < 1.)
        {
            const double inv_RDI = 1. - RDI;
            attenuation_coeff =
                (1. / mean_ray_length) * (std::log(inv_RDI) - (1. / (2 * ray_count * inv_RDI)));
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
    });
}


auto Grid::VoxelChunk::flat_idx_to_global_index3d(unsigned int idx) const -> Index3D
{
    auto& [chunk_x, chunk_y, chunk_z] = m_origin_idx;
    return {
        chunk_x * s_max_edge_size + static_cast<unsigned int>(idx % m_dim_x),
        chunk_y * s_max_edge_size + static_cast<unsigned int>((idx / m_dim_x) % m_dim_y),
        chunk_z * s_max_edge_size + static_cast<unsigned int>((idx / m_dim_x) / m_dim_y)
    };
}

auto Grid::export_as_coo_to_h5(
    const std::string& dataset_name, const std::filesystem::path& filename, bool include_all_data
) const -> void
{
    Logger     logger{"Grid HDF5 export"};
    H5::H5File file;
    if (std::filesystem::exists(filename))
    {
        std::filesystem::remove(filename);
    }

    file = H5::H5File{filename.string(), H5F_ACC_TRUNC};

    std::vector<unsigned int> xs;
    std::vector<unsigned int> ys;
    std::vector<unsigned int> zs;
    std::vector<unsigned int> counts;
    std::vector<unsigned int> hits;
    std::vector<double>       pads;
    std::vector<double>       lengths;
    std::vector<double>       lengths_variance;

    for (auto& a_chunk : m_chunks)
    {
        auto chunk = a_chunk.load(std::memory_order_relaxed);

        if (!chunk)
            continue;

        auto index_with_data = chunk->m_counts | std::views::enumerate |
                               std::views::filter([&chunk](const auto& pair) -> bool {
                                   return chunk->m_counts[std::get<0>(pair)] != 0;
                               }) |
                               std::views::elements<0> | std::ranges::to<std::vector>();
        auto index3d_with_data =
            index_with_data | std::views::transform([this, &chunk](unsigned int index) -> Index3D {
                return chunk->flat_idx_to_global_index3d(index);
            }) |
            std::ranges::to<std::vector>();

        // Copying indexes
        const auto h5_size_t = H5::PredType::NATIVE_UINT;
        {
            auto chunk_xs = index3d_with_data | std::views::elements<0>;
            std::copy(chunk_xs.begin(), chunk_xs.end(), std::back_inserter(xs));
        }

        {
            auto chunk_ys = index3d_with_data | std::views::elements<1>;
            std::copy(chunk_ys.begin(), chunk_ys.end(), std::back_inserter(ys));
        }

        {
            auto chunk_zs = index3d_with_data | std::views::elements<2>;
            std::copy(chunk_zs.begin(), chunk_zs.end(), std::back_inserter(zs));
        }

        {
            auto chunk_pads =
                index_with_data | std::views::transform([&chunk](const size_t& index) -> double {
                    return chunk->m_pad[index];
                });
            std::copy(chunk_pads.begin(), chunk_pads.end(), std::back_inserter(pads));
        }

        if (include_all_data)
        {
            {
                auto chunk_hits =
                    index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_hits[index];
                    });
                std::copy(chunk_hits.begin(), chunk_hits.end(), std::back_inserter(hits));
            }

            {
                auto chunk_counts =
                    index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_counts[index];
                    });
                std::copy(chunk_counts.begin(), chunk_counts.end(), std::back_inserter(counts));
            }

            {
                auto chunk_lengths =
                    index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_lengths[index];
                    });
                std::copy(chunk_lengths.begin(), chunk_lengths.end(), std::back_inserter(lengths));
            }

            if (!chunk->m_lengths_variance.empty())
            {
                auto chunk_lengths_variance =
                    index_with_data |
                    std::views::transform([&chunk](const size_t& index) -> unsigned int {
                        return chunk->m_lengths_variance[index];
                    });
                std::copy(
                    chunk_lengths_variance.begin(),
                    chunk_lengths_variance.end(),
                    std::back_inserter(lengths_variance)
                );
            }
        }
    }

    const hsize_t voxels_with_data = std::ranges::distance(pads);

    using h5_dimension_t = std::array<hsize_t, 1>;
    const h5_dimension_t voxel_count_dim{voxels_with_data};

    const H5::DataSpace data_space{std::tuple_size_v<h5_dimension_t>, voxel_count_dim.data()};

    H5::Group plot_group;
    if (file.nameExists(dataset_name))
        plot_group = file.openGroup(dataset_name);
    else
        plot_group = file.createGroup(dataset_name, 6);

    H5::DSetCreatPropList create_prop_list{};
    h5_dimension_t        chunk_dims{std::min(static_cast<hsize_t>(2 << 13), voxels_with_data)};
    create_prop_list.setChunk(1, chunk_dims.data());

    const auto get_or_create_dataset =
        [&plot_group, &dataset_name, &create_prop_list](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::DataSet {
        if (plot_group.nameExists(name))
        {
            return plot_group.openDataSet(name);
        }

        return plot_group.createDataSet(name, type, dataspace, create_prop_list);
    };

    const H5::PredType h5_index_t = H5::PredType::NATIVE_UINT;
    {
        H5::DataSet xs_data = get_or_create_dataset("x", h5_index_t, data_space);
        xs_data.write(xs.data(), h5_index_t);
    }

    {
        H5::DataSet ys_data = get_or_create_dataset("y", h5_index_t, data_space);
        ys_data.write(ys.data(), h5_index_t);
    }

    {
        H5::DataSet zs_data = get_or_create_dataset("z", h5_index_t, data_space);
        zs_data.write(zs.data(), h5_index_t);
    }

    {
        H5::PredType h5_pad_t    = H5::PredType::NATIVE_DOUBLE;
        H5::DataSet  values_data = get_or_create_dataset("pad", h5_pad_t, data_space);
        values_data.write(pads.data(), h5_pad_t);
    }

    if (include_all_data)
    {
        {
            H5::PredType h5_hits_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  hits_data = get_or_create_dataset("hits", h5_hits_t, data_space);
            hits_data.write(hits.data(), h5_hits_t);
        }

        {
            H5::PredType h5_counts_t = H5::PredType::NATIVE_UINT;
            H5::DataSet  counts_data = get_or_create_dataset("counts", h5_counts_t, data_space);
            counts_data.write(counts.data(), h5_counts_t);
        }

        {
            H5::PredType h5_lengths_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  lengths_data = get_or_create_dataset("lengths", h5_lengths_t, data_space);
            lengths_data.write(lengths.data(), h5_lengths_t);
        }

        if (!lengths_variance.empty())
        {
            H5::PredType h5_lengths_var_t = H5::PredType::NATIVE_DOUBLE;
            H5::DataSet  length_variances_data =
                get_or_create_dataset("lengths variance", h5_lengths_var_t, data_space);
            length_variances_data.write(lengths_variance.data(), h5_lengths_var_t);
        }
    }

    // Minimum coordinate attribute
    const std::array<hsize_t, 1> scalar_value_dim{1};
    H5::DataSpace                scalar_data_space{1, scalar_value_dim.data()};
    const std::array<hsize_t, 1> singular_3d_coord_count = {3};
    H5::DataSpace                singular_3d_coord_data_space{1, singular_3d_coord_count.data()};

    const auto get_or_create_attribute =
        [&plot_group](
            const std::string& name, const H5::PredType& type, const H5::DataSpace& dataspace
        ) -> H5::Attribute {
        if (plot_group.attrExists(name))
        {
            return plot_group.openAttribute(name);
        }

        return plot_group.createAttribute(name, type, dataspace);
    };
    const auto    h5_voxel_size_t = H5::PredType::NATIVE_DOUBLE;
    H5::Attribute voxel_size_attr =
        get_or_create_attribute("Voxel size", h5_voxel_size_t, H5::DataSpace{});
    voxel_size_attr.write(h5_voxel_size_t, &m_cell_size);

    const auto h5_min_coords_t = H5::PredType::NATIVE_DOUBLE;

    H5::Attribute min_coord_attr = get_or_create_attribute(
        "Minimal coordinates values", h5_min_coords_t, singular_3d_coord_data_space
    );

    const std::array<double, 3> min_coords = {m_bounds.minx, m_bounds.miny, m_bounds.minz};
    min_coord_attr.write(h5_voxel_size_t, min_coords.data());

    // Grid dimensions
    const auto    h5_grid_dim_t = H5::PredType::NATIVE_UINT64;
    H5::Attribute grid_dims_attr =
        get_or_create_attribute("Dimensions", h5_grid_dim_t, singular_3d_coord_data_space);
    const std::array<size_t, 3> grid_dims = {m_dim_x, m_dim_y, m_dim_z};
    grid_dims_attr.write(h5_grid_dim_t, grid_dims.data());

    file.close();
}

} // namespace lvox
