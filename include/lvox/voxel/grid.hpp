#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <functional>

#include <lvox/lvox_types.hpp>

namespace lvox
{

class Beam;

class Grid
{
  public:
    Grid()          = default;
    virtual ~Grid() = default;

    // NOTE: no bounds check!
    [[nodiscard]]
    virtual auto voxel_bounds(Index idx_x, Index idx_y, Index idx_z) const -> Bounds = 0;
    [[nodiscard]]
    virtual auto voxel_bounds_from_point(const Point& point) -> Bounds = 0;
    // Return an index tuple of this layout (x, y, z)
    [[nodiscard]]
    virtual auto index_of_point(const Point& point) const -> Index3D = 0;
    [[nodiscard]]
    virtual auto cell_size() const -> double = 0;
    [[nodiscard]]
    virtual auto cell_count() const -> Index = 0;
    [[nodiscard]]
    virtual auto dim_x() const -> Index = 0;
    [[nodiscard]]
    virtual auto dim_y() const -> Index = 0;
    [[nodiscard]]
    virtual auto dim_z() const -> Index = 0;
    [[nodiscard]]
    virtual auto bounds() const -> const Bounds& = 0;

    // Woo and Amanatides' fast traversal algorithm

    static auto traversal(
        const Grid&                                        grid,
        const Beam&                                        beam,
        const std::function<void(const Index3D&, double)>& callback,
        const double max_distance = std::numeric_limits<double>::max()
    ) -> void;
};

} // namespace lvox

#endif // !LVOX_VOXEL_GRID_HPP
