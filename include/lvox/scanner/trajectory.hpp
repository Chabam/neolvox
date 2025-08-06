#ifndef LVOX_TRAJECTORY_HPP
#define LVOX_TRAJECTORY_HPP

#include <filesystem>

#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>

#include <lvox/types.hpp>

namespace lvox
{
class Trajectory
{
  public:
    // TODO: Make this more flexible with reader options
    Trajectory(PointCloudView&& trajectory_points);

    auto interpolate_point_from_gps_time(double gps_time) const -> std::optional<lvox::Point>;
    auto get_points() const -> const PointCloud& { return *m_traj_points; }

  private:
    PointCloudView m_traj_points;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
