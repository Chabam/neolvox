#ifndef LVOX_SPHERICAL_SCANNER_HPP
#define LVOX_SPHERICAL_SCANNER_HPP

#include <vector>

#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/beam_generator.hpp>

namespace lvox
{

class SphericalScanner : public BeamGenerator
{
  public:
    SphericalScanner(
        const Point& origin, double horizontal_fov, double vertical_fov, double angular_resolution
    );
    SphericalScanner(SphericalScanner&&);
    SphericalScanner(const SphericalScanner&);
    SphericalScanner& operator=(SphericalScanner&&);
    SphericalScanner& operator=(const SphericalScanner&);

    virtual const std::vector<Beam>& get_beams() const final { return m_beams; }

  private:
    std::vector<Beam> m_beams;
};

} // namespace lvox

#endif // LVOX_SPHERICAL_SCANNER_HPP
