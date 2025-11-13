#include <algorithm>
#include <filesystem>
#include <ranges>

#include <lvox/scanner/trajectory.hpp>

#include <lvox/types.hpp>

namespace lvox
{

// This code is greatly inspired by PDAL's georeference filter
// Trajectory class found here:
// https://github.com/PDAL/PDAL/blob/6e3d5ed48205dcef884243db6585f7da4631604c/filters/private/georeference/Trajectory.cpp
// This class can be removed if one day it becomes available from the
// cpp interface.
Trajectory::Trajectory(PointCloudView&& trajectory_points)
    : m_traj_points{std::move(trajectory_points)}
{
    std::ranges::sort(*m_traj_points, [](const TimedPoint& lhs, const TimedPoint& rhs) -> bool {
        return lhs.m_gps_time < rhs.m_gps_time;
    });
}

std::optional<lvox::Point> Trajectory::interpolate_point_from_gps_time(double gps_time) const
{
    auto upper  = std::lower_bound(
        m_traj_points->begin(),
        m_traj_points->end(),
        gps_time,
        [](const TimedPoint pt, double time) {
            return pt.m_gps_time < time;
        }
    );

    // gps time is not found in the trajectory
    if (upper == m_traj_points->begin() || upper == m_traj_points->end())
    {
        return {};
    }

    // Returning a point in the trajectory that is the closest to the
    // requested gps time by linear interpolation.
    const auto   p1   = *(upper - 1);
    const auto   p2   = *upper;
    const double t1   = p1.m_gps_time;
    const double t2   = p2.m_gps_time;
    const double frac = (gps_time - t1) / (t2 - t1);

    return lvox::Point{
        std::lerp(p1.m_point.x(), p2.m_point.x(), frac),
        std::lerp(p1.m_point.y(), p2.m_point.y(), frac),
        std::lerp(p1.m_point.z(), p2.m_point.z(), frac)
    };
}

} // namespace lvox
