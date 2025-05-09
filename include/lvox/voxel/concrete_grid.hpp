#ifndef LVOX_VOXEL_CONCRETE_GRID_HPP
#define LVOX_VOXEL_CONCRETE_GRID_HPP

#include <atomic>
#include <cmath>
#include <format>
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
        , m_cells{ConcreteGrid::intialize_container(m_dim_x * m_dim_y * m_dim_z)}
        , m_bounds{bounds}
        , m_new_value_creation_guard{}
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
        , m_cells{std::move(other.m_cells)}
        , m_bounds{std::move(other.m_bounds)}
        , m_new_value_creation_guard{}
    {
    }

    ConcreteGrid(const ConcreteGrid& other)
        : m_cell_size{other.m_cell_size}
        , m_dim_x{other.m_dim_x}
        , m_dim_y{other.m_dim_y}
        , m_dim_z{other.m_dim_z}
        , m_cells{other.m_cells}
        , m_bounds{other.m_bounds}
        , m_new_value_creation_guard{}
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
            if (const auto& it = m_cells.find(index); it != m_cells.end())
            {
                return it->second;
            }
            std::lock_guard<std::mutex> lock{m_new_value_creation_guard};
            return m_cells.emplace(index, contained_type_t<cell_t>{}).first->second;
        }
    }

    [[nodiscard]]
    auto at(Index x, Index y, Index z) const -> const_cell_ref
    {
        return at(coords_to_index(x, y, z));
    }

    [[nodiscard]]
    auto at(Index x, Index y, Index z) -> cell_ref
    {
        return at(coords_to_index(x, y, z));
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
        return dim_x() * dim_y() * dim_z();
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

    auto begin() -> ContainerT::iterator { return m_cells.begin(); }

    auto end() -> ContainerT::iterator { return m_cells.end(); }

    auto cbegin() const -> ContainerT::const_iterator { return m_cells.cbegin(); }

    auto cend() const -> ContainerT::const_iterator { return m_cells.cend(); }

    auto begin() const -> ContainerT::const_iterator { return m_cells.begin(); }

    auto end() const -> ContainerT::const_iterator { return m_cells.end(); }

  private:
    double     m_cell_size;
    Index      m_dim_x;
    Index      m_dim_y;
    Index      m_dim_z;
    ContainerT m_cells;
    Bounds     m_bounds;
    std::mutex m_new_value_creation_guard;

    auto coords_to_index(Index x, Index y, Index z) const -> Index
    {
        return x + (y * m_dim_x) + (z * m_dim_x * m_dim_y);
    }

    static auto adjust_dim_to_grid(double distance, double cell_size) -> Index
    {
        return static_cast<Index>(std::ceil(distance / cell_size));
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

#endif // LVOX_VOXEL_CONCRETE_GRID_HPP
