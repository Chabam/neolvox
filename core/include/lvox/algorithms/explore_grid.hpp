#ifndef LVOX_EXPLORE_GRID_HPP
#define LVOX_EXPLORE_GRID_HPP

#include <lvox/voxel/grid.hpp>

namespace lvox
{

struct Scan;

namespace algorithms
{

struct ComputeOptions;

void explore_grid(Grid& grid, const Scan& scan, const ComputeOptions& options);
void explore_grid_theoriticals(Grid& grid, const Scan& scan, const ComputeOptions& options);

} // namespace algorithms

} // namespace lvox

#endif // LVOX_EXPLORE_GRID_HPP
