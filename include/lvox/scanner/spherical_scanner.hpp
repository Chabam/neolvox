#ifndef LVOX_SPHERICAL_SCANNER_HPP
#define LVOX_SPHERICAL_SCANNER_HPP

#include <vector>

#include <lvox/scanner/beam_generator.hpp>

namespace lvox
{

class SphericalScanner : public BeamGenerator
{
  public:
    using vector_t         = Beam::vector_t;

    SphericalScanner(
        const vector_t& origin,
        double          horizontal_fov,
        double          vertical_fov,
        double          angular_resolution
    );
    SphericalScanner(SphericalScanner&&);
    SphericalScanner(const SphericalScanner&);
    SphericalScanner& operator=(SphericalScanner&&);
    SphericalScanner& operator=(const SphericalScanner&);

    virtual auto get_beams() const -> std::vector<Beam> final { return m_beams; }

  private:
    std::vector<Beam> m_beams;
};

} // namespace lvox

#endif // LVOX_SPHERICAL_SCANNER_HPP
