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

Bounds BoundedGrid::voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const
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

unsigned int BoundedGrid::adjust_dim_to_grid(double distance, unsigned int voxel_alignment)
{
    const auto     new_dim   = static_cast<unsigned int>(std::ceil(distance / m_cell_size));

    if (new_dim % voxel_alignment == 0)
        return new_dim;

    // Rounding to the nearest chunk dimension.
    return new_dim + voxel_alignment - (new_dim % voxel_alignment);
}

double BoundedGrid::adjust_bounds_to_grid(size_t dim, double min) const
{
    return min + dim * m_cell_size;
}

} // namespace lvox
