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
    struct Point
    {
        double      m_gps_time;
        lvox::Point m_coord;
    };

    // TODO: Make this more flexible with reader options
    Trajectory(const std::filesystem::path& trajectory_file);

    auto interpolate_point_from_gps_time(double gps_time) const -> std::optional<lvox::Point>;
    auto get_points() const -> const std::vector<Point>& { return m_traj_points; }

  private:
    std::vector<Point> m_traj_points;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
