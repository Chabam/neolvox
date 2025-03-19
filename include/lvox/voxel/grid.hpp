#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <cstddef>
#include <cstdint>
#include <pdal/util/Bounds.hpp>
#include <vector>

namespace lvox::voxel
{

template <class T>
class Grid
{
  public:
    using cell_type = T;
    using const_cell_ref = const T&;
    using cell_ref = T&;

    Grid(const pdal::BOX3D& box3d, double cell_size)
        : Grid(cell_size,
               std::ceil((box3d.maxx - box3d.minx) / cell_size),
               std::ceil((box3d.maxy - box3d.miny) / cell_size),
               std::ceil((box3d.maxz - box3d.minz) / cell_size))
    {
    }

    Grid(double cell_size, size_t dim_x, size_t dim_y, size_t dim_z)
        : m_cell_size{cell_size}
        , m_dim_x{dim_x}
        , m_dim_y{dim_y}
        , m_dim_z{dim_z}
        , m_cells{dim_x * dim_y * dim_z, std::allocator<T>{}}
    {
    }

    Grid(Grid&& other)
        : m_cell_size{std::move(other.m_cell_size)}
        , m_dim_x{std::move(other.m_dim_x)}
        , m_dim_y{std::move(other.m_dim_y)}
        , m_dim_z{std::move(other.m_dim_z)}
        , m_cells{std::move(other.m_cells)}
    {
    }

    Grid(const Grid& other)
        : m_cell_size{other.m_cell_size}
        , m_dim_x{other.m_dim_x}
        , m_dim_y{other.m_dim_y}
        , m_dim_z{other.m_dim_z}
        , m_cells{other.m_cells}
    {
    }

    auto operator=(Grid&& other) -> Grid&
    {
        m_cell_size = std::move(other.m_cell_size);
        m_dim_x = std::move(other.m_dim_x);
        m_dim_y = std::move(other.m_dim_y);
        m_dim_z = std::move(other.m_dim_z);
        m_cells = std::move(other.m_cells);

        return *this;
    }

    auto operator=(const Grid& other) -> Grid&
    {
        m_cell_size = other.m_cell_size;
        m_dim_x = other.m_dim_x;
        m_dim_y = other.m_dim_y;
        m_dim_z = other.m_dim_z;
        m_cells = other.m_cells;

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

    Grid() = default;
    ~Grid() = default;

    auto cell_count() -> size_t { return m_cells.size(); }
    auto dim_x() const -> size_t { return m_dim_x; }
    auto dim_y() const -> size_t { return m_dim_y; }
    auto dim_z() const -> size_t { return m_dim_z; }

  private:
    double m_cell_size;
    size_t m_dim_x;
    size_t m_dim_y;
    size_t m_dim_z;
    std::vector<cell_type> m_cells;
};

using GridU32i = Grid<std::uint32_t>;

} // namespace lvox::voxel

#endif // !LVOX_VOXEL_GRID_HPP
