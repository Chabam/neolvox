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

class Grid
{
  public:
    Grid()  = default;
    ~Grid() = default;

    Grid(const Bounds& bounds, double cell_size)
        : m_cell_size{cell_size}
          , m_dim_x{Grid::adjust_dim_to_grid(bounds.maxx - bounds.minx, cell_size)}
          , m_dim_y{Grid::adjust_dim_to_grid(bounds.maxy - bounds.miny, cell_size)}
          , m_dim_z{Grid::adjust_dim_to_grid(bounds.maxz - bounds.minz, cell_size)}
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
          , m_bounds{std::move(other.m_bounds)}
    {
    }

    Grid(const Grid& other)
        : m_cell_size{other.m_cell_size}
          , m_dim_x{other.m_dim_x}
          , m_dim_y{other.m_dim_y}
          , m_dim_z{other.m_dim_z}
          , m_bounds{other.m_bounds}
    {
    }

    auto operator=(Grid&& other) -> Grid&
    {
        m_cell_size = std::move(other.m_cell_size);
        m_dim_x     = std::move(other.m_dim_x);
        m_dim_y     = std::move(other.m_dim_y);
        m_dim_z     = std::move(other.m_dim_z);
        m_bounds    = std::move(other.m_bounds);

        return *this;
    }

    auto operator=(const Grid& other) -> Grid&
    {
        m_cell_size = other.m_cell_size;
        m_dim_x     = other.m_dim_x;
        m_dim_y     = other.m_dim_y;
        m_dim_z     = other.m_dim_z;
        m_bounds    = other.m_bounds;

        return *this;
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

  private:
    double                    m_cell_size;
    Index                     m_dim_x;
    Index                     m_dim_y;
    Index                     m_dim_z;
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

} // namespace lvox

#endif // LVOX_VOXEL_GRID_HPP
