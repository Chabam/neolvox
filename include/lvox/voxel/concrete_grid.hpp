#ifndef LVOX_VOXEL_CONCRETE_GRID_HPP
#define LVOX_VOXEL_CONCRETE_GRID_HPP

#include <format>
#include <map>
#include <vector>

#include <lvox/voxel/grid.hpp>

namespace lvox
{

template <typename T, typename ContainerT>
class ConcreteGrid : public Grid
{
  public:
    using cell_t         = T;
    using const_cell_ref = const T&;
    using cell_ref       = T&;
    using container_t    = ContainerT;

    ConcreteGrid()  = default;
    ~ConcreteGrid() = default;

    ConcreteGrid(const bounds_t& box3d, double cell_size)
        : m_cell_size{cell_size}
        , m_dim_x{ConcreteGrid::adjust_dim_to_grid(box3d.maxx - box3d.minx, cell_size)}
        , m_dim_y{ConcreteGrid::adjust_dim_to_grid(box3d.maxy - box3d.miny, cell_size)}
        , m_dim_z{ConcreteGrid::adjust_dim_to_grid(box3d.maxz - box3d.minz, cell_size)}
        , m_cells{}
        , m_bounds{box3d}
    {
        if constexpr (std::is_same<container_t, std::vector<cell_t>>::value)
        {
            m_cells.resize(m_dim_x * m_dim_y * m_dim_z);
        }

        const auto adjust_bounds_to_grid = [cell_size](size_t dim, double min) {
            return min + dim * cell_size;
        };
        m_bounds.grow(
            adjust_bounds_to_grid(m_dim_x, box3d.minx),
            adjust_bounds_to_grid(m_dim_y, box3d.miny),
            adjust_bounds_to_grid(m_dim_z, box3d.minz)
        );
    }

    ConcreteGrid(ConcreteGrid&& other)
        : m_cell_size{std::move(other.m_cell_size)}
        , m_dim_x{std::move(other.m_dim_x)}
        , m_dim_y{std::move(other.m_dim_y)}
        , m_dim_z{std::move(other.m_dim_z)}
        , m_cells{std::move(other.m_cells)}
        , m_bounds{std::move(other.m_bounds)}
    {
    }

    ConcreteGrid(const ConcreteGrid& other)
        : m_cell_size{other.m_cell_size}
        , m_dim_x{other.m_dim_x}
        , m_dim_y{other.m_dim_y}
        , m_dim_z{other.m_dim_z}
        , m_cells{other.m_cells}
        , m_bounds{other.m_bounds}
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

    auto at(size_t idx_x, size_t idx_y, size_t idx_z) const -> const_cell_ref
    {
        return m_cells.at(idx_x + (idx_y * m_dim_x) + (idx_z * m_dim_x * m_dim_y));
    }

    auto at(size_t idx_x, size_t idx_y, size_t idx_z) -> cell_ref
    {
        return m_cells.at(idx_x + (idx_y * m_dim_x) + (idx_z * m_dim_x * m_dim_y));
    }

    // NOTE: no bounds check!
    auto voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> bounds_t final
    {
        const double min_x = m_bounds.minx + idx_x * m_cell_size;
        const double min_y = m_bounds.miny + idx_y * m_cell_size;
        const double min_z = m_bounds.minz + idx_z * m_cell_size;
        return bounds_t{
            //
            min_x,
            min_y,
            min_z,
            min_x + m_cell_size,
            min_y + m_cell_size,
            min_z + m_cell_size
        };
    }

    auto voxel_bounds_from_point(const Eigen::Vector3d& point) -> bounds_t final
    {
        const auto [idx_x, idx_y, idx_z] = index_of_point(point);
        return voxel_bounds(idx_x, idx_y, idx_z);
    }

    // Return an index tuple of this layout (x, y, z)
    auto index_of_point(const Eigen::Vector3d& point) const -> std::array<size_t, 3> final
    {
        const double x = point.x();
        const double y = point.y();
        const double z = point.z();

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

    auto cell_count() const -> size_t final { return dim_x() * dim_y() * dim_z(); }
    auto dim_x() const -> size_t final { return m_dim_x; }
    auto dim_y() const -> size_t final { return m_dim_y; }
    auto dim_z() const -> size_t final { return m_dim_z; }
    auto bounds() const -> const bounds_t& final { return m_bounds; }

  private:
    double      m_cell_size;
    size_t      m_dim_x;
    size_t      m_dim_y;
    size_t      m_dim_z;
    container_t m_cells;
    bounds_t    m_bounds;

    static auto adjust_dim_to_grid(double distance, double cell_size) -> size_t
    {
        return static_cast<size_t>(std::ceil(distance / cell_size));
    }
};

template <typename T>
using DenseGrid = ConcreteGrid<T, std::vector<T>>;

template <typename T>
using SparseGrid = ConcreteGrid<T, std::map<size_t, T>>;

using DenseGridU32i  = DenseGrid<std::uint32_t>;
using SparseGridU32i = SparseGrid<std::uint32_t>;

} // namespace lvox

#endif // LVOX_VOXEL_CONCRETE_GRID_HPP
