#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/trajectory.hpp>

namespace lvox
{

auto Scan::ComputeBeamOrigin::operator()(const Point& scan_origin) -> Point
{
    return scan_origin;
}

auto Scan::ComputeBeamOrigin::operator()(const std::shared_ptr<Trajectory>& trajectory) -> Point
{
    const auto point = trajectory->interpolate_point_from_gps_time(gps_time);

    [[unlikely]]
    if (!point)
    {
        throw std::runtime_error(
            std::format("requested gps time for point not in trajectory {}!", gps_time)
        );
    }

    return *point;
}

} // namespace lvox
