#ifndef LVOX_TLS_SCAN_HPP
#define LVOX_TLS_SCAN_HPP

#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>

namespace lvox
{

class TLSScan : public Scan
{
  public:
    TLSScan(
        PointCloudView                         point_cloud,
        const Point&                           scan_origin,
        const std::optional<SphericalScanner>& scanner = {}
    );

    virtual auto get_points() const -> const PointCloudView& final;
    virtual auto get_beams() const -> std::vector<lvox::Beam> final;
    virtual auto get_scan_position(long gps_time) const -> const Point& final;

  private:
    PointCloudView                  m_pts_view;
    Point                           m_scan_origin;
    std::optional<SphericalScanner> m_scanner;
};

} // namespace lvox

#endif // LVOX_TLS_SCAN_HPP
