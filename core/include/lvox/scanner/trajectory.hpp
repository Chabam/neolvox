#ifndef LVOX_TRAJECTORY_HPP
#define LVOX_TRAJECTORY_HPP

#include <optional>
#include <lvox/types.hpp>

namespace lvox
{

template <Point PointT, TimedPoint TimedPointT, PointCloud<TimedPointT> PointCloudT>
class Trajectory
{
  public:
    Trajectory(const PointCloudT& trajectory_points)
        : m_traj_points{trajectory_points}
    {
        std::ranges::sort(
            m_traj_points, [](const TimedPointT& lhs, const TimedPointT& rhs) -> bool {
                return lhs.gps_time() < rhs.gps_time();
            }
        );
    }

    std::optional<PointT> interpolate_point_from_gps_time(double gps_time) const
    {
        auto upper = std::lower_bound(
            m_traj_points.begin(),
            m_traj_points.end(),
            gps_time,
            [](const TimedPointT pt, double time) {
                return pt.m_gps_time < time;
            }
        );

        // gps time is not found in the trajectory
        if (upper == m_traj_points.begin() || upper == m_traj_points.end())
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

        return PointT{
            std::lerp(p1.m_point.x(), p2.m_point.x(), frac),
            std::lerp(p1.m_point.y(), p2.m_point.y(), frac),
            std::lerp(p1.m_point.z(), p2.m_point.z(), frac)
        };
    }
    const PointCloudT& get_points() const { return m_traj_points; }

  private:
    PointCloudT m_traj_points;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
