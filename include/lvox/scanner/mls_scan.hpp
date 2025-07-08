#ifndef LVOX_MLS_SCAN_HPP
#define LVOX_MLS_SCAN_HPP

#include <lvox/scanner/scan.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/scanner/trajectory.hpp>

namespace lvox
{

class MLSScan : public Scan
{
  public:
    MLSScan(const PointCloudView& point_cloud, const std::shared_ptr<Trajectory>& trajectory);

    virtual auto get_points() const -> const PointCloudView& final;
    virtual auto get_beams() const -> std::vector<lvox::Beam> final;
    virtual auto get_scan_position(double gps_time) const -> Point final;

  private:
    PointCloudView                  m_pts_view;
    std::shared_ptr<Trajectory>     m_trajectory;
    std::optional<SphericalScanner> m_scanner;
};

} // namespace lvox

#endif // LVOX_MLS_SCAN_HPP
