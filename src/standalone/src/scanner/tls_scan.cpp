#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/scanner/beam_generator.hpp>
#include <lvox/scanner/tls_scan.hpp>

namespace lvox
{

TLSScan::TLSScan(
    const pdal::PointViewPtr&                    point_cloud,
    const Point&                                 scan_origin,
    const std::optional<lvox::SphericalScanner>& scanner
)
    : m_pts_view{point_cloud}
    , m_scan_origin{scan_origin}
    , m_scanner{scanner}
{
}

auto TLSScan::get_points() const -> const pdal::PointViewPtr&
{
    return m_pts_view;
}

auto TLSScan::get_beams() const -> std::vector<lvox::Beam>
{
    if (m_scanner)
        return m_scanner->get_beams();

    Logger{"TLS Scanner"}.warn(
        "No virtual scanner provided for current scan, not returning any theoritical beams"
    );

    return {};
}

auto TLSScan::get_scan_position(double) const -> Point
{
    return m_scan_origin;
}

} // namespace lvox
