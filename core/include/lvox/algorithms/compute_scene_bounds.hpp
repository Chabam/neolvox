#ifndef LVOX_COMPUTE_SCENE_BOUNDS_HPP
#define LVOX_COMPUTE_SCENE_BOUNDS_HPP

#include <lvox/voxel/bounds.hpp>

namespace lvox
{

struct Scan;

namespace algorithms
{

lvox::Bounds compute_scene_bounds(const std::vector<lvox::Scan>& scans);

}

} // namespace lvox

#endif // LVOX_COMPUTE_SCENE_BOUNDS_HPP
