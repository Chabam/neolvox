#ifndef LVOX_SCAN_HPP
#define LVOX_SCAN_HPP

#include <lvox/types.hpp>

namespace lvox
{

class Beam;

class Scan
{
  public:
    virtual ~Scan()                                                = default;
    virtual auto get_points() const -> const PointCloudView&       = 0;
    virtual auto get_beams() const -> std::vector<lvox::Beam>      = 0;
    virtual auto get_scan_position(double gps_time) const -> Point = 0;
};

} // namespace lvox

#endif // LVOX_SCAN_HPP
