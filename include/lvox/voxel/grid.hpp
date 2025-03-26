#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <cstddef>
#include <cstdint>
#include <format>
#include <vector>

#include <pdal/util/Bounds.hpp>

namespace lvox::voxel
{

template <class T>
class Grid
{
  public:
    using cell_type      = T;
    using const_cell_ref = const T&;
    using cell_ref       = T&;

    Grid()  = default;
    ~Grid() = default;

    Grid(const pdal::BOX3D& box3d, double cell_size)
        : m_cell_size{cell_size}
        , m_dim_x{Grid::adjust_dim_to_grid(box3d.maxx - box3d.minx, cell_size)}
        , m_dim_y{Grid::adjust_dim_to_grid(box3d.maxy - box3d.miny, cell_size)}
        , m_dim_z{Grid::adjust_dim_to_grid(box3d.maxz - box3d.minz, cell_size)}
        , m_cells{m_dim_x * m_dim_y * m_dim_z, std::allocator<T>{}}
        , m_bounds{box3d}
    {
        const auto adjust_bounds_to_grid = [cell_size](size_t dim, double min) {
            return min + dim * cell_size;
        };
        m_bounds.grow(
            adjust_bounds_to_grid(m_dim_x, box3d.minx),
            adjust_bounds_to_grid(m_dim_y, box3d.miny),
            adjust_bounds_to_grid(m_dim_z, box3d.minz)
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

    auto at(size_t idx_x, size_t idx_y, size_t idx_z) const -> const_cell_ref
    {
        return m_cells.at(idx_x + (idx_y * m_dim_x) + (idx_z * m_dim_x * m_dim_y));
    }

    auto at(size_t idx_x, size_t idx_y, size_t idx_z) -> cell_ref
    {
        return m_cells.at(idx_x + (idx_y * m_dim_x) + (idx_z * m_dim_x * m_dim_y));
    }

    // NOTE: no bounds check!
    auto voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> pdal::BOX3D
    {
        const double min_x = m_bounds.minx + idx_x * m_cell_size;
        const double min_y = m_bounds.miny + idx_y * m_cell_size;
        const double min_z = m_bounds.minz + idx_z * m_cell_size;
        return pdal::BOX3D{
            //
            min_x,
            min_y,
            min_z,
            min_x + m_cell_size,
            min_y + m_cell_size,
            min_z + m_cell_size
        };
    }

    auto voxel_bounds_from_point(double x, double y, double z) -> pdal::BOX3D
    {
        const auto [idx_x, idx_y, idx_z] = index_of_point(x, y, z);
        return voxel_bounds(idx_x, idx_y, idx_z);
    }

    // Return an index tuple of this layout (x, y, z)
    auto index_of_point(double x, double y, double z) const -> std::tuple<size_t, size_t, size_t>
    {
        if (!m_bounds.contains(x, y, z))
        {
            throw std::runtime_error{std::format("Provided coords are not in the grid: ({}, {}, {})", x, y, z)};
        }

        const auto coords_to_index = [cell_size = m_cell_size](double min, double coord) -> size_t {
            return std::round((coord - min) / cell_size) - 1;
        };

        return {
            //
            coords_to_index(m_bounds.minx, x),
            coords_to_index(m_bounds.miny, y),
            coords_to_index(m_bounds.minz, z)
        };
    }

    auto cell_count() const -> size_t { return m_cells.size(); }
    auto dim_x() const -> size_t { return m_dim_x; }
    auto dim_y() const -> size_t { return m_dim_y; }
    auto dim_z() const -> size_t { return m_dim_z; }
    auto bounds() const -> const pdal::BOX3D& { return m_bounds; }

  private:
    double                 m_cell_size;
    size_t                 m_dim_x;
    size_t                 m_dim_y;
    size_t                 m_dim_z;
    std::vector<cell_type> m_cells;
    pdal::BOX3D            m_bounds;

    static auto adjust_dim_to_grid(double distance, double cell_size) -> size_t
    {
        return static_cast<size_t>(std::ceil(distance / cell_size));
    }
};

using GridU32i = Grid<std::uint32_t>;

} // namespace lvox::voxel

#endif // !LVOX_VOXEL_GRID_HPP
