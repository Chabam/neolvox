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
        Vector      m_orientation;
    };

    // TODO: Make this more flexible with reader options
    Trajectory(const std::filesystem::path& trajectory_file);

    auto get_point_from_gps_time(double gps_time) -> std::optional<Point>;

  private:
    pdal::PointViewPtr m_pts_view;
    pdal::PointTable   m_pts_table;
    pdal::PointViewSet m_pts_view_set;
};
} // namespace lvox

#endif // LVOX_TRAJECTORY_HPP
