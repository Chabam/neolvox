#ifndef LVOX_BOUNDED_GRID_HPP
#define LVOX_BOUNDED_GRID_HPP

#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>

namespace lvox
{

class BoundedGrid
{
  public:
    BoundedGrid(
        const Bounds& bounds,
        double        cell_size,
        unsigned int  voxel_alignment = 1
    );
    BoundedGrid(const BoundedGrid& other);
    BoundedGrid(BoundedGrid&& other);

    // NOTE: no bounds check!
    auto voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> Bounds;

    auto voxel_bounds_from_point(const Point& point) -> Bounds;

    // Return an index tuple of this layout (x, y, z)
    auto index3d_of_point(const Point& point) const -> Index3D;

    auto cell_size() const -> double { return m_cell_size; }
    auto cell_count() const -> size_t { return m_cell_count; }

    auto dim_x() const -> unsigned int { return m_dim_x; }
    auto dim_y() const -> unsigned int { return m_dim_y; }
    auto dim_z() const -> unsigned int { return m_dim_z; }

    auto bounds() const -> const Bounds& { return m_bounds; }

    double       m_cell_size;
    unsigned int m_dim_x;
    unsigned int m_dim_y;
    unsigned int m_dim_z;
    size_t       m_cell_count;
    Bounds       m_bounds;

    auto adjust_dim_to_grid(double distance, unsigned int voxel_alignment) -> unsigned int;
    auto adjust_bounds_to_grid(size_t dim, double min) const -> double;

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
