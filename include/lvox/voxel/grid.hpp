#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <atomic>
#include <cmath>
#include <format>
#include <vector>

#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>

namespace lvox
{

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

template <typename T>
class Grid
{
  public:
    using cell_t           = T;
    using const_cell_ref   = const cell_t&;
    using cell_ref         = cell_t&;
    using iterator_t       = std::vector<cell_t>::iterator;
    using const_iterator_t = std::vector<cell_t>::const_iterator;

    Grid()  = default;
    ~Grid() = default;

    Grid(const Bounds& bounds, double cell_size)
        : m_cell_size{cell_size}
        , m_dim_x{Grid::adjust_dim_to_grid(bounds.maxx - bounds.minx, cell_size)}
        , m_dim_y{Grid::adjust_dim_to_grid(bounds.maxy - bounds.miny, cell_size)}
        , m_dim_z{Grid::adjust_dim_to_grid(bounds.maxz - bounds.minz, cell_size)}
        , m_cells{m_dim_x * m_dim_y * m_dim_z, std::allocator<cell_t>{}}
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

    Grid(Grid&& other)
        : m_cell_size{std::move(other.m_cell_size)}
        , m_dim_x{std::move(other.m_dim_x)}
        , m_dim_y{std::move(other.m_dim_y)}
        , m_dim_z{std::move(other.m_dim_z)}
        , m_cells{std::move(other.m_cells)}
        , m_bounds{std::move(other.m_bounds)}
    {
    }

    Grid(const Grid& other)
        : m_cell_size{other.m_cell_size}
        , m_dim_x{other.m_dim_x}
        , m_dim_y{other.m_dim_y}
        , m_dim_z{other.m_dim_z}
        , m_cells{other.m_cells}
        , m_bounds{other.m_bounds}
    {
    }

    auto operator=(Grid&& other) -> Grid&
    {
        m_cell_size = std::move(other.m_cell_size);
        m_dim_x     = std::move(other.m_dim_x);
        m_dim_y     = std::move(other.m_dim_y);
        m_dim_z     = std::move(other.m_dim_z);
        m_cells     = std::move(other.m_cells);
        m_bounds    = std::move(other.m_bounds);

        return *this;
    }

    auto operator=(const Grid& other) -> Grid&
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
        return m_cells.at(index);
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
        return at(i) == cell_t{};
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
        return
        {
            i % m_dim_x,
            (i / m_dim_x) % m_dim_y,
            (i / m_dim_x) / m_dim_z
        };
    }

    auto begin() -> iterator_t { return m_cells.begin(); }

    auto end() -> iterator_t { return m_cells.end(); }

    auto cbegin() const -> const_iterator_t { return m_cells.cbegin(); }

    auto cend() const -> const_iterator_t { return m_cells.cend(); }

    auto begin() const -> const_iterator_t { return m_cells.begin(); }

    auto end() const -> const_iterator_t { return m_cells.end(); }

  private:
    double                    m_cell_size;
    Index                     m_dim_x;
    Index                     m_dim_y;
    Index                     m_dim_z;
    std::vector<cell_t>       m_cells;
    Bounds                    m_bounds;

    static auto adjust_dim_to_grid(double distance, double cell_size) -> Index
    {
        return static_cast<Index>(std::ceil(distance / cell_size));
    }

    auto adjust_bounds_to_grid(Index dim, double min) const -> double
    {
        return min + dim * m_cell_size;
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

using GridU32 = Grid<std::atomic_uint32_t>;
using GridD   = Grid<std::atomic<double>>;

} // namespace lvox

#endif // LVOX_VOXEL_GRID_HPP
