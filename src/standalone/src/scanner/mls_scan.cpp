#include <stdexcept>
#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/scanner/beam_generator.hpp>
#include <lvox/scanner/mls_scan.hpp>

namespace lvox
{

MLSScan::MLSScan(
    const pdal::PointViewPtr& point_cloud,
    const std::shared_ptr<Trajectory>& trajectory
)
    : m_pts_view{point_cloud}
    , m_trajectory{trajectory}
{
}

auto MLSScan::get_points() const -> const pdal::PointViewPtr&
{
    return m_pts_view;
}

auto MLSScan::get_beams() const -> std::vector<lvox::Beam>
{
    if (m_scanner)
        return m_scanner->get_beams();

    Logger{"MLS Scanner"}.warn(
        "No virtual scanner provided for current scan, not returning any theoritical beams"
    );

    return {};
}

auto MLSScan::get_scan_position(double gps_time) const -> Point
{
    const std::optional<lvox::Trajectory::Point> point =
        m_trajectory->get_point_from_gps_time(gps_time);

    [[unlikely]]
    if (!point)
    {
        throw std::runtime_error(std::format("requested gps time for point not in trajectory {}!", gps_time));
    }

    return point->m_coord;
}

} // namespace lvox
