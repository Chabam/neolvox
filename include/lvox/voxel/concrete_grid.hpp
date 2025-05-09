#ifndef LVOX_VOXEL_CONCRETE_GRID_HPP
#define LVOX_VOXEL_CONCRETE_GRID_HPP

#include <atomic>
#include <cmath>
#include <format>
#include <mutex>
#include <vector>

#include <lvox/logger/logger.hpp>
#include <lvox/lvox_types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

template <std::ranges::range ContainerT, typename T>
struct is_dense_container : std::is_same<ContainerT, std::vector<T>>
{
};

template <typename T>
struct contained_type
{
    using type = T;
};

template <typename T>
struct contained_type<std::atomic<T>>
{
    using type = T;
};

template <typename T>
using contained_type_t = contained_type<T>::type;

template <typename T, std::ranges::range ContainerT>
class ConcreteGrid : public Grid
{
  public:
    using cell_t         = T;
    using const_cell_ref = const cell_t&;
    using cell_ref       = cell_t&;
    using container_t    = ContainerT;

    ConcreteGrid()  = default;
    ~ConcreteGrid() = default;

    ConcreteGrid(const Bounds& bounds, double cell_size)
        : m_cell_size{cell_size}
        , m_dim_x{ConcreteGrid::adjust_dim_to_grid(bounds.maxx - bounds.minx, cell_size)}
        , m_dim_y{ConcreteGrid::adjust_dim_to_grid(bounds.maxy - bounds.miny, cell_size)}
        , m_dim_z{ConcreteGrid::adjust_dim_to_grid(bounds.maxz - bounds.minz, cell_size)}
        , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
        , m_shards{initialize_shards(m_cell_count)}
        , m_bounds{bounds}
    {

        Logger logger{"ConcreteGrid"};

        logger.debug(
            g_grid_loginfo,
            m_dim_x,
            m_dim_y,
            m_dim_z,
            m_cell_size,
            m_bounds.minx,
            m_bounds.maxx,
            m_bounds.miny,
            m_bounds.maxy,
            m_bounds.minz,
            m_bounds.maxz
        );

        const auto adjust_bounds_to_grid = [cell_size](Index dim, double min) -> double {
            return min + dim * cell_size;
        };

        m_bounds.grow(
            adjust_bounds_to_grid(m_dim_x, bounds.minx),
            adjust_bounds_to_grid(m_dim_y, bounds.miny),
            adjust_bounds_to_grid(m_dim_z, bounds.minz)
        );
    }

    ConcreteGrid(ConcreteGrid&& other)
        : m_cell_size{std::move(other.m_cell_size)}
        , m_dim_x{std::move(other.m_dim_x)}
        , m_dim_y{std::move(other.m_dim_y)}
        , m_dim_z{std::move(other.m_dim_z)}
        , m_shards{std::move(other.m_shards)}
        , m_bounds{std::move(other.m_bounds)}
    {
    }

    ConcreteGrid(const ConcreteGrid& other) = delete;

    auto operator=(ConcreteGrid&& other) -> ConcreteGrid&
    {
        m_cell_size = std::move(other.m_cell_size);
        m_dim_x     = std::move(other.m_dim_x);
        m_dim_y     = std::move(other.m_dim_y);
        m_dim_z     = std::move(other.m_dim_z);
        m_shards    = std::move(other.m_shards);
        m_bounds    = std::move(other.m_bounds);

        return *this;
    }

    auto operator=(const ConcreteGrid& other) -> ConcreteGrid& = delete;

    auto set_or_append(Index x, Index y, Index z, contained_type_t<cell_t> val) -> void
    {
        return set_or_append(coords_to_index(x, y, z), val);
    }

    auto set_or_append(Index index, contained_type_t<cell_t> val) -> void
    {
        std::unique_ptr<Shard>& shard = m_shards[get_shard_index(index)];
        if constexpr (is_dense_container<container_t, cell_t>::value)
        {
            shard->m_cells.at(index) += val;
        }
        else
        {
            if (const auto& it = shard->m_cells.find(index); it != shard->m_cells.end())
            {
                it->second += val;
            }

            std::scoped_lock<std::mutex> lock{shard->m_guard};
            shard->m_cells.emplace(index, val);
        }
    }

    [[nodiscard]]
    auto get(Index x, Index y, Index z) const -> const_cell_ref
    {
        return get(coords_to_index(x, y, z));
    }

    [[nodiscard]]
    auto get(Index index) const -> const_cell_ref
    {
        return m_shards[get_shard_index(index)]->m_cells.at(index);
    }

    // NOTE: no bounds check!
    [[nodiscard]]
    auto voxel_bounds(Index idx_x, Index idx_y, Index idx_z) const -> Bounds final
    {
        const double min_x = m_bounds.minx + idx_x * m_cell_size;
        const double min_y = m_bounds.miny + idx_y * m_cell_size;
        const double min_z = m_bounds.minz + idx_z * m_cell_size;
        return Bounds{//
                      min_x,
                      min_y,
                      min_z,
                      min_x + m_cell_size,
                      min_y + m_cell_size,
                      min_z + m_cell_size
        };
    }

    [[nodiscard]]
    auto voxel_bounds_from_point(const Point& point) -> Bounds final
    {
        const auto [idx_x, idx_y, idx_z] = index_of_point(point);
        return voxel_bounds(idx_x, idx_y, idx_z);
    }

    // Return an index tuple of this layout (x, y, z)
    [[nodiscard]]
    auto index_of_point(const Point& point) const -> Index3D final
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
            [cell_size = m_cell_size](double min, double max, double coord) -> Index {
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

    [[nodiscard]]
    auto cell_size() const -> double final
    {
        return m_cell_size;
    }

    [[nodiscard]]
    auto cell_count() const -> Index final
    {
        return m_cell_count;
    }

    [[nodiscard]]
    auto dim_x() const -> Index final
    {
        return m_dim_x;
    }

    [[nodiscard]]
    auto dim_y() const -> Index final
    {
        return m_dim_y;
    }

    [[nodiscard]]
    auto dim_z() const -> Index final
    {
        return m_dim_z;
    }

    [[nodiscard]]
    auto bounds() const -> const Bounds& final
    {
        return m_bounds;
    }

  private:
    struct Shard
    {
        std::mutex m_guard;
        ContainerT m_cells;

        Shard(Index cell_count)
            : m_guard{}
            , m_cells{ConcreteGrid::initialize_container(cell_count)}
        {
        }
    };

    using Shards = std::vector<std::unique_ptr<Shard>>;

    using ShardIndex = unsigned int;
    auto get_shard_index(Index idx) const -> ShardIndex
    {
        return std::hash<Index>{}(idx) % m_shards.size();
    }

    double m_cell_size;
    Index  m_dim_x;
    Index  m_dim_y;
    Index  m_dim_z;
    Index  m_cell_count;
    Shards m_shards;
    Bounds m_bounds;

    auto coords_to_index(Index x, Index y, Index z) const -> Index
    {
        return x + (y * m_dim_x) + (z * m_dim_x * m_dim_y);
    }

    static auto adjust_dim_to_grid(double distance, double cell_size) -> Index
    {
        return static_cast<Index>(std::ceil(distance / cell_size));
    }

    static auto initialize_shards(Index cell_count) -> Shards
    {
        const ShardIndex shard_count              = std::max(1UL, cell_count / 1'000'000);
        const auto       max_cell_count_per_shard = cell_count / shard_count;
        Index            assigned_cells           = 0;

        Shards shards{shard_count};
        for (ShardIndex i = 0; i < shard_count; ++i)
        {
            const Index cells_to_assign =
                std::max(assigned_cells + max_cell_count_per_shard, cell_count - assigned_cells);
            shards[i] = std::make_unique<Shard>(cells_to_assign);

            assigned_cells += cells_to_assign;
        }

        return shards;
    }

    static constexpr auto initialize_container(Index size) -> container_t
    {
        if constexpr (is_dense_container<container_t, cell_t>::value)
        {
            return container_t{size, std::allocator<cell_t>{}};
        }
        else
        {
            return container_t{};
        }
    }

    static constexpr auto g_grid_loginfo = R"(
Creating grid of dimension: {}x{}x{}
Voxel size: {}
Bounds:
    x: {}, {}
    y: {}, {}
    z: {}, {}
)";
};

template <typename T>
using DenseGrid = ConcreteGrid<T, std::vector<T>>;

// NOTE: We use `unordered_map` as a "sparse" array.
// Our keys are literally just array indexes, there's no need to hash them
// since every index is, by definition, unique.
using IndexHash = std::identity;

template <typename T>
using SparseGrid = ConcreteGrid<T, std::unordered_map<Index, T, IndexHash>>;

using GridU32 = SparseGrid<std::atomic_uint32_t>;

} // namespace lvox

#endif // LVOX_VOXEL_CONCRETE_GRID_HPP
