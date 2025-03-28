#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <cstddef>
#include <eigen3/Eigen/Eigen>

#include <pdal/util/Bounds.hpp>

namespace lvox
{

class Grid
{
  public:
    using bounds_t = pdal::BOX3D;
    using point_t  = Eigen::Vector3d;

    Grid()          = default;
    virtual ~Grid() = default;

    virtual auto voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> bounds_t = 0;

    virtual auto voxel_bounds_from_point(const point_t& point) -> bounds_t = 0;

    // Return an index tuple of this layout (x, y, z)
    virtual auto index_of_point(const point_t& point) const -> std::array<size_t, 3> = 0;

    virtual auto cell_count() const -> size_t      = 0;
    virtual auto dim_x() const -> size_t           = 0;
    virtual auto dim_y() const -> size_t           = 0;
    virtual auto dim_z() const -> size_t           = 0;
    virtual auto bounds() const -> const bounds_t& = 0;
};

} // namespace lvox

#endif // !LVOX_VOXEL_GRID_HPP
