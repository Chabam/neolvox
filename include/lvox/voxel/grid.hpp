#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <atomic>
#include <cmath>
#include <format>
#include <shared_mutex>
#include <mutex>
#include <vector>

#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
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
class ConcreteGrid
{
  public:
    using cell_t           = T;
    using const_cell_ref   = const cell_t&;
    using cell_ref         = cell_t&;
    using container_t      = ContainerT;
    using iterator_t       = ContainerT::iterator;
    using const_iterator_t = ContainerT::const_iterator;

    ConcreteGrid()  = default;
    ~ConcreteGrid() = default;

    ConcreteGrid(const Bounds& bounds, double cell_size)
        : m_cell_size{cell_size}
        , m_dim_x{ConcreteGrid::adjust_dim_to_grid(bounds.maxx - bounds.minx, cell_size)}
        , m_dim_y{ConcreteGrid::adjust_dim_to_grid(bounds.maxy - bounds.miny, cell_size)}
        , m_dim_z{ConcreteGrid::adjust_dim_to_grid(bounds.maxz - bounds.minz, cell_size)}
        , m_cells{ConcreteGrid::intialize_container(m_dim_x * m_dim_y * m_dim_z)}
        , m_bounds{
            bounds.minx,
            bounds.miny,
            bounds.minz,
            ConcreteGrid::adjust_bounds_to_grid(m_dim_x, bounds.minx),
            ConcreteGrid::adjust_bounds_to_grid(m_dim_y, bounds.miny),
            ConcreteGrid::adjust_bounds_to_grid(m_dim_z, bounds.minz)
        }
        , m_cells_access{}
    {

        Logger logger{"ConcreteGrid"};

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

    ConcreteGrid(ConcreteGrid&& other)
        : m_cell_size{std::move(other.m_cell_size)}
        , m_dim_x{std::move(other.m_dim_x)}
        , m_dim_y{std::move(other.m_dim_y)}
        , m_dim_z{std::move(other.m_dim_z)}
        , m_cells{std::move(other.m_cells)}
        , m_bounds{std::move(other.m_bounds)}
        , m_cells_access{}
    {
    }

    ConcreteGrid(const ConcreteGrid& other)
        : m_cell_size{other.m_cell_size}
        , m_dim_x{other.m_dim_x}
        , m_dim_y{other.m_dim_y}
        , m_dim_z{other.m_dim_z}
        , m_cells{other.m_cells}
        , m_bounds{other.m_bounds}
        , m_cells_access{}
    {
    }

    auto operator=(ConcreteGrid&& other) -> ConcreteGrid&
    {
        m_cell_size = std::move(other.m_cell_size);
        m_dim_x     = std::move(other.m_dim_x);
        m_dim_y     = std::move(other.m_dim_y);
        m_dim_z     = std::move(other.m_dim_z);
        m_cells     = std::move(other.m_cells);
        m_bounds    = std::move(other.m_bounds);

        return *this;
    }

    auto operator=(const ConcreteGrid& other) -> ConcreteGrid&
    {
        m_cell_size = other.m_cell_size;
        m_dim_x     = other.m_dim_x;
        m_dim_y     = other.m_dim_y;
        m_dim_z     = other.m_dim_z;
        m_cells     = other.m_cells;
        m_bounds    = other.m_bounds;

        return *this;
    }

    [[nodiscard]]
    auto at(Index index) const -> const_cell_ref
    {
        return m_cells.at(index);
    }

    [[nodiscard]]
    auto at(Index index) -> cell_ref
    {
        if constexpr (is_dense_container<container_t, cell_t>::value)
        {
            return m_cells.at(index);
        }
        else
        {
            std::shared_lock read_lock{m_cells_access};
            if (const auto& it = m_cells.find(index); it != m_cells.end())
            {
                return it->second;
            }
            read_lock.unlock();
            std::unique_lock write_lock{m_cells_access};
            return m_cells.emplace(index, contained_type_t<cell_t>{}).first->second;
        }
    }

    [[nodiscard]]
    auto at(Index x, Index y, Index z) const -> const_cell_ref
    {
        return at(index3d_to_index(x, y, z));
    }

    [[nodiscard]]
    auto at(Index x, Index y, Index z) -> cell_ref
    {
        return at(index3d_to_index(x, y, z));
    }

    // NOTE: no bounds check!
    [[nodiscard]]
    auto voxel_bounds(Index idx_x, Index idx_y, Index idx_z) const -> Bounds
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
    auto voxel_bounds_from_point(const Point& point) -> Bounds
    {
        const auto [idx_x, idx_y, idx_z] = index_of_point(point);
        return voxel_bounds(idx_x, idx_y, idx_z);
    }

    // Return an index tuple of this layout (x, y, z)
    [[nodiscard]]
    auto index_of_point(const Point& point) const -> Index3D
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
    auto cell_size() const -> double
    {
        return m_cell_size;
    }

    [[nodiscard]]
    auto cell_count() const -> Index
    {
        return dim_x() * dim_y() * dim_z();
    }

    [[nodiscard]]
    auto dim_x() const -> Index
    {
        return m_dim_x;
    }

    [[nodiscard]]
    auto dim_y() const -> Index
    {
        return m_dim_y;
    }

    [[nodiscard]]
    auto dim_z() const -> Index
    {
        return m_dim_z;
    }

    [[nodiscard]]
    auto bounds() const -> const Bounds&
    {
        return m_bounds;
    }

    auto is_na(Index i) const -> bool
    {
        if constexpr (is_dense_container<ContainerT, cell_t>::value)
            return at(i) == contained_type_t<ContainerT>{};
        else
            return !m_cells.contains(i);
    };

    auto is_na(Index x, Index y, Index z) const -> bool
    {
        return is_na(index3d_to_index(x, y, z));
    };

    [[nodiscard]]
    auto index3d_to_index(Index x, Index y, Index z) const -> Index
    {
        return x + (y * m_dim_x) + (z * m_dim_x * m_dim_y);
    }

    [[nodiscard]]
    auto index_to_index3d(Index i) const -> Index3D
    {
        const auto z      = i / (m_dim_x * m_dim_y);
        const auto offsetted_i = i - (z * m_dim_x * m_dim_y);
        return {offsetted_i % m_dim_x, offsetted_i / m_dim_x, z};
    }

    auto begin() -> iterator_t { return m_cells.begin(); }

    auto end() -> iterator_t { return m_cells.end(); }

    auto cbegin() const -> const_iterator_t { return m_cells.cbegin(); }

    auto cend() const -> const_iterator_t { return m_cells.cend(); }

    auto begin() const -> const_iterator_t { return m_cells.begin(); }

    auto end() const -> const_iterator_t { return m_cells.end(); }

  private:
    const double            m_cell_size;
    const Index             m_dim_x;
    const Index             m_dim_y;
    const Index             m_dim_z;
    ContainerT              m_cells;
    const Bounds            m_bounds;
    std::shared_mutex       m_cells_access;

    static auto adjust_dim_to_grid(double distance, double cell_size) -> Index
    {
        return static_cast<Index>(std::ceil(distance / cell_size));
    }

    auto adjust_bounds_to_grid(Index dim, double min) const -> double
    {
        return min + dim * m_cell_size;
    }

    static constexpr auto intialize_container(Index size) -> container_t
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
Voxel count: {}
Bounds:
    x: {}, {}
    y: {}, {}
    z: {}, {}
)";
};

// Not really used, only in tests
template <typename T>
using DenseGrid = ConcreteGrid<T, std::vector<T>>;

// NOTE: We use `unordered_map` as a "sparse" array.
// Our keys are literally just array indexes, there's no need to hash them
// since every index is, by definition, unique.
using IndexHash = std::identity;

template <typename T>
using SparseGrid = ConcreteGrid<T, std::unordered_map<Index, T, IndexHash>>;

using GridU32 = SparseGrid<std::atomic_uint32_t>;
using GridD   = SparseGrid<std::atomic<double>>;

} // namespace lvox

#endif // LVOX_VOXEL_GRID_HPP
