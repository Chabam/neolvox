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
    BoundedGrid(const Bounds<double>& bounds, double cell_size, unsigned int voxel_alignment = 1);
    BoundedGrid(const BoundedGrid& other);
    BoundedGrid(BoundedGrid&& other);

    // NOTE: no bounds check!
    Bounds<double> voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const;

    // Return an index tuple of this layout (x, y, z)
    Index3D index3d_of_point(const Vector& point) const;

    // Return an index tuple of this layout (x, y, z)
    Bounds<double> voxel_bounds_from_point(const Vector& point);

    double cell_size() const { return m_cell_size; }
    size_t cell_count() const { return m_cell_count; }

    unsigned int dim_x() const { return m_dim_x; }
    unsigned int dim_y() const { return m_dim_y; }
    unsigned int dim_z() const { return m_dim_z; }

    const Bounds<double>& bounds() const { return m_bounds; }

    double       m_cell_size;
    Bounds<int>  m_index_bounds;
    unsigned int m_dim_x;
    unsigned int m_dim_y;
    unsigned int m_dim_z;

    size_t         m_cell_count;
    Bounds<double> m_bounds;

    int    adjust_dim_to_grid(int distance, unsigned int voxel_alignment);

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
