#include <lvox/voxel/bounded_grid.hpp>
#include <lvox/logger/logger.hpp>

namespace lvox
{

BoundedGrid::BoundedGrid(const Bounds& bounds, double cell_size, unsigned int voxel_alignment)
    : m_cell_size{cell_size}
    , m_dim_x{BoundedGrid::adjust_dim_to_grid(bounds.m_max_x - bounds.m_min_x, voxel_alignment)}
    , m_dim_y{BoundedGrid::adjust_dim_to_grid(bounds.m_max_y - bounds.m_min_y, voxel_alignment)}
    , m_dim_z{BoundedGrid::adjust_dim_to_grid(bounds.m_max_z - bounds.m_min_z, voxel_alignment)}
    , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
    , m_bounds{
          bounds.m_min_x,
          BoundedGrid::adjust_bounds_to_grid(m_dim_x, bounds.m_min_x),
          bounds.m_min_y,
          BoundedGrid::adjust_bounds_to_grid(m_dim_y, bounds.m_min_y),
          bounds.m_min_z,
          BoundedGrid::adjust_bounds_to_grid(m_dim_z, bounds.m_min_z)
      }
{

    Logger logger{"BoundedGrid"};

    logger.debug(
        g_grid_loginfo,
        m_dim_x,
        m_dim_y,
        m_dim_z,
        m_cell_size,
        cell_count(),
        m_bounds.m_min_x,
        m_bounds.m_max_x,
        m_bounds.m_min_y,
        m_bounds.m_max_y,
        m_bounds.m_min_z,
        m_bounds.m_max_z
    );
}

BoundedGrid::BoundedGrid(const BoundedGrid& other)
    : m_cell_size{other.m_cell_size}
    , m_dim_x{other.m_dim_x}
    , m_dim_y{other.m_dim_y}
    , m_dim_z{other.m_dim_z}
    , m_cell_count{other.m_cell_count}
    , m_bounds{other.m_bounds}
{
}

BoundedGrid::BoundedGrid(BoundedGrid&& other)
    : m_cell_size{std::move(other.m_cell_size)}
    , m_dim_x{std::move(other.m_dim_x)}
    , m_dim_y{std::move(other.m_dim_y)}
    , m_dim_z{std::move(other.m_dim_z)}
    , m_cell_count{std::move(other.m_cell_count)}
    , m_bounds{std::move(other.m_bounds)}
{
}

auto BoundedGrid::voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> Bounds
{
    const double min_x = m_bounds.m_min_x + idx_x * m_cell_size;
    const double min_y = m_bounds.m_min_y + idx_y * m_cell_size;
    const double min_z = m_bounds.m_min_z + idx_z * m_cell_size;
    return Bounds{
        //
        min_x,
        min_x + m_cell_size,
        min_y,
        min_y + m_cell_size,
        min_z,
        min_z + m_cell_size
    };
}

auto BoundedGrid::voxel_bounds_from_point(const Point& point) -> Bounds
{
    const auto [idx_x, idx_y, idx_z] = index3d_of_point(point);
    return voxel_bounds(idx_x, idx_y, idx_z);
}

// Return an index tuple of this layout (x, y, z)
auto BoundedGrid::index3d_of_point(const Point& point) const -> Index3D
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
        coords_to_index(m_bounds.m_min_x, m_bounds.m_max_x, x),
        coords_to_index(m_bounds.m_min_y, m_bounds.m_max_y, y),
        coords_to_index(m_bounds.m_min_z, m_bounds.m_max_z, z)
    };
}

auto BoundedGrid::adjust_dim_to_grid(double distance, unsigned int voxel_alignment) -> unsigned int
{
    const auto     new_dim   = static_cast<unsigned int>(std::ceil(distance / m_cell_size));

    if (new_dim % voxel_alignment == 0)
        return new_dim;

    // Rounding to the nearest chunk dimension.
    return new_dim + voxel_alignment - (new_dim % voxel_alignment);
}

auto BoundedGrid::adjust_bounds_to_grid(size_t dim, double min) const -> double
{
    return min + dim * m_cell_size;
}

} // namespace lvox
