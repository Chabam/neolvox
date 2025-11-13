#ifndef LVOX_TRAJECTORY_HPP
#define LVOX_TRAJECTORY_HPP

#include <optional>
#include <lvox/types.hpp>

namespace lvox
{
class Trajectory
{
  public:
    Trajectory(PointCloudView&& trajectory_points);

    std::optional<lvox::Point> interpolate_point_from_gps_time(double gps_time) const;
    const PointCloud&          get_points() const { return *m_traj_points; }

  private:
    PointCloudView m_traj_points;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
