#ifndef LVOX_SCAN_HPP
#define LVOX_SCAN_HPP

#include <variant>
#include <optional>

#include <lvox/types.hpp>

namespace lvox
{

class Trajectory;

using ScannerOrigin  = std::variant<Point, std::shared_ptr<Trajectory>>;

struct Scan
{
    PointCloudView                m_points;
    ScannerOrigin                 m_scanner_origin;
    Bounds                        m_bounds;
    std::optional<PointCloudView> m_blank_shots;

    struct ComputeBeamOrigin
    {
        const double gps_time;
        auto   operator()(const Point& scan_origin) -> Point;
        auto   operator()(const std::shared_ptr<Trajectory>& trajectory) -> Point;
    };
};

} // namespace lvox

#endif // LVOX_SCAN_HPP
