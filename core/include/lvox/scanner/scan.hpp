#ifndef LVOX_SCAN_HPP
#define LVOX_SCAN_HPP

#include <optional>
#include <variant>

#include <lvox/types.hpp>
#include <lvox/voxel/bounds.hpp>

namespace lvox
{

class Trajectory;

using ScannerOrigin = std::variant<Point, std::shared_ptr<Trajectory>>;

struct Scan
{
    PointCloudView                m_points;
    ScannerOrigin                 m_scanner_origin;
    Bounds                        m_bounds;
    std::optional<PointCloudView> m_blank_shots;

    struct ComputeBeamOrigin
    {
        const double gps_time;
        Point        operator()(const Point& scan_origin);
        Point        operator()(const std::shared_ptr<Trajectory>& trajectory);
    };
};

} // namespace lvox

#endif // LVOX_SCAN_HPP
