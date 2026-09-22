#ifndef LVOX_TRAJECTORY_HPP
#define LVOX_TRAJECTORY_HPP

#include <optional>

#include <lvox/types.hpp>

namespace lvox
{

template <Point PointT, PointCloud<PointT> PointCloudT>
class Trajectory
{
  public:
    Trajectory(const PointCloudT& trajectory_points)
        : m_traj_points{}
    {
        m_traj_points.resize(std::ranges::distance(trajectory_points));

        std::ranges::transform(
            trajectory_points, m_traj_points.begin(), [](const PointT& pt) -> TrajectoryPoint {
                return TrajectoryPoint{pt.x(), pt.y(), pt.z(), pt.gps_time()};
            }
        );

        std::ranges::sort(
            m_traj_points, [](const TrajectoryPoint& lhs, const TrajectoryPoint& rhs) -> bool {
                return lhs.gps_time < rhs.gps_time;
            }
        );
    }

    Vector interpolate_point_from_gps_time(double gps_time) const
    {
        auto upper = std::lower_bound(
            m_traj_points.begin(),
            m_traj_points.end(),
            gps_time,
            [](const TrajectoryPoint pt, double time) {
                return pt.gps_time < time;
            }
        );

        // Returning the first point when the requested gps time is
        // before the first point
        if (upper == m_traj_points.begin())
            return Vector{upper->x, upper->y, upper->z};

        // If it's after, return the last point
        if (upper == m_traj_points.end())
        {
            const auto& last = m_traj_points.back();
            return Vector{last.x, last.y, last.z};
        }

        // Otherwise interpolate the two closests points
        const auto   p1   = *(upper - 1);
        const auto   p2   = *upper;
        const double t1   = p1.gps_time;
        const double t2   = p2.gps_time;
        const double frac = (gps_time - t1) / (t2 - t1);

        return Vector{
            std::lerp(p1.x, p2.x, frac), std::lerp(p1.y, p2.y, frac), std::lerp(p1.z, p2.z, frac)
        };
    }
    const PointCloudT& get_points() const { return m_traj_points; }

  private:
    struct TrajectoryPoint
    {
        double x;
        double y;
        double z;
        double gps_time;
    };
    std::vector<TrajectoryPoint> m_traj_points;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
