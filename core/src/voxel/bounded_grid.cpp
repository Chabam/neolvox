#include <lvox/voxel/bounded_grid.hpp>
#include <lvox/logger/logger.hpp>

namespace lvox
{

BoundedGrid::BoundedGrid(
    const Bounds<double>& bounds, double cell_size, unsigned int voxel_alignment
)
    : m_cell_size{cell_size}
    , m_index_bounds{
        BoundedGrid::adjust_dim_to_grid(std::floor(bounds.m_min_x / m_cell_size), voxel_alignment),
        BoundedGrid::adjust_dim_to_grid(std::ceil(bounds.m_max_x / m_cell_size), voxel_alignment),
        BoundedGrid::adjust_dim_to_grid(std::floor(bounds.m_min_y / m_cell_size), voxel_alignment),
        BoundedGrid::adjust_dim_to_grid(std::ceil(bounds.m_max_y / m_cell_size), voxel_alignment),
        BoundedGrid::adjust_dim_to_grid(std::floor(bounds.m_min_z / m_cell_size), voxel_alignment),
        BoundedGrid::adjust_dim_to_grid(std::ceil(bounds.m_max_z / m_cell_size), voxel_alignment),
    }
    , m_dim_x{static_cast<unsigned int>(std::abs(m_index_bounds.m_max_x - m_index_bounds.m_min_x))}
    , m_dim_y{static_cast<unsigned int>(std::abs(m_index_bounds.m_max_y - m_index_bounds.m_min_y))}
    , m_dim_z{static_cast<unsigned int>(std::abs(m_index_bounds.m_max_z - m_index_bounds.m_min_z))}
    , m_cell_count{m_dim_x * m_dim_y * m_dim_z}
    , m_bounds{
          m_index_bounds.m_min_x * m_cell_size,
          m_index_bounds.m_max_x * m_cell_size,
          m_index_bounds.m_min_y * m_cell_size,
          m_index_bounds.m_max_y * m_cell_size,
          m_index_bounds.m_min_z * m_cell_size,
          m_index_bounds.m_max_z * m_cell_size
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
    , m_index_bounds{other.m_index_bounds}
    , m_dim_x{other.m_dim_x}
    , m_dim_y{other.m_dim_y}
    , m_dim_z{other.m_dim_z}
    , m_cell_count{other.m_cell_count}
    , m_bounds{other.m_bounds}
{
}

BoundedGrid::BoundedGrid(BoundedGrid&& other)
    : m_cell_size{std::move(other.m_cell_size)}
    , m_index_bounds{std::move(other.m_index_bounds)}
    , m_dim_x{std::move(other.m_dim_x)}
    , m_dim_y{std::move(other.m_dim_y)}
    , m_dim_z{std::move(other.m_dim_z)}
    , m_cell_count{std::move(other.m_cell_count)}
    , m_bounds{std::move(other.m_bounds)}
{
}

Index3D BoundedGrid::index3d_of_point(const Vector& point) const
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

    return {
        static_cast<int>(std::floor(x / m_cell_size)),
        static_cast<int>(std::floor(y / m_cell_size)),
        static_cast<int>(std::floor(z / m_cell_size))
    };
}

Bounds<double> BoundedGrid::voxel_bounds_from_point(const Vector& point)
{
    const auto [idx_x, idx_y, idx_z] = index3d_of_point(point);
    return voxel_bounds(idx_x, idx_y, idx_z);
}

Bounds<double> BoundedGrid::voxel_bounds(int idx_x, int idx_y, int idx_z) const
{
    const double min_x = idx_x * m_cell_size;
    const double min_y = idx_y * m_cell_size;
    const double min_z = idx_z * m_cell_size;
    return Bounds<double>{
        //
        min_x,
        min_x + m_cell_size,
        min_y,
        min_y + m_cell_size,
        min_z,
        min_z + m_cell_size
    };
}

int BoundedGrid::adjust_dim_to_grid(int distance, unsigned int voxel_alignment)
{
    if (distance % voxel_alignment == 0)
        return distance;

    int offset = voxel_alignment - (distance % voxel_alignment);
    if (distance < 0)
        offset = -offset;

    // Rounding to the nearest chunk dimension.
    return distance + offset;
}

} // namespace lvox
