#ifndef LVOX_SPHERICAL_SCANNER_HPP
#define LVOX_SPHERICAL_SCANNER_HPP

#include <vector>

#include <lvox/scanner/beam.hpp>

namespace lvox
{

class SphericalScanner
{
  public:
    SphericalScanner(
        const Vector& origin, double horizontal_fov, double vertical_fov, double angular_resolution
    );
    SphericalScanner(SphericalScanner&& other);
    SphericalScanner(const SphericalScanner& other);

    SphericalScanner& operator=(SphericalScanner&& other);
    SphericalScanner& operator=(const SphericalScanner& other);

    const std::vector<Beam>& get_beams() const { return m_beams; }

  private:
    std::vector<Beam> m_beams;
};

} // namespace lvox

#endif // LVOX_SPHERICAL_SCANNER_HPP
