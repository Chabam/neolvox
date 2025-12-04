#ifndef LVOX_COMPUTE_SCENE_BOUNDS_HPP
#define LVOX_COMPUTE_SCENE_BOUNDS_HPP

#include <lvox/voxel/bounds.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox
{

namespace algorithms
{

template <Point PointT, TimedPoint TimedPointT, PointCloud<TimedPointT> PointCloudT>
lvox::Bounds compute_scene_bounds(const std::vector<Scan<PointT, TimedPointT, PointCloudT>>& scans)
{
    Bounds total_bounds;

    for (const auto& scan : scans)
        total_bounds.grow(scan.m_bounds);

    return total_bounds;
}

} // namespace algorithms

} // namespace lvox

#endif // LVOX_COMPUTE_SCENE_BOUNDS_HPP
