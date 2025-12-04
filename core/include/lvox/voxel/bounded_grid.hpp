#ifndef LVOX_BOUNDED_GRID_HPP
#define LVOX_BOUNDED_GRID_HPP

#include <format>

#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>

namespace lvox
{

class BoundedGrid
{
  public:
    BoundedGrid(const Bounds& bounds, double cell_size, unsigned int voxel_alignment = 1);
    BoundedGrid(const BoundedGrid& other);
    BoundedGrid(BoundedGrid&& other);

    // NOTE: no bounds check!
    Bounds voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const;

    // Return an index tuple of this layout (x, y, z)
    template <Point PointT>
    Index3D index3d_of_point(const PointT& point) const
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

    // Return an index tuple of this layout (x, y, z)
    template <Point PointT>
    Bounds voxel_bounds_from_point(const PointT& point)
    {
        const auto [idx_x, idx_y, idx_z] = index3d_of_point(point);
        return voxel_bounds(idx_x, idx_y, idx_z);
    }

    double cell_size() const { return m_cell_size; }
    size_t cell_count() const { return m_cell_count; }

    unsigned int dim_x() const { return m_dim_x; }
    unsigned int dim_y() const { return m_dim_y; }
    unsigned int dim_z() const { return m_dim_z; }

    const Bounds& bounds() const { return m_bounds; }

    double       m_cell_size;
    unsigned int m_dim_x;
    unsigned int m_dim_y;
    unsigned int m_dim_z;
    size_t       m_cell_count;
    Bounds       m_bounds;

    unsigned int adjust_dim_to_grid(double distance, unsigned int voxel_alignment);
    double       adjust_bounds_to_grid(size_t dim, double min) const;

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

#endif // LVOX_BOUNDED_GRID_HPP
