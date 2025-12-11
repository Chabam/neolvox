#ifndef LVOX_SPHERICAL_SCANNER_HPP
#define LVOX_SPHERICAL_SCANNER_HPP

#include <vector>

#include <lvox/scanner/beam.hpp>
#include <lvox/logger/logger.hpp>

namespace lvox
{

constexpr auto g_spherical_scanner_info = R"(
FOV
    horizontal {}°
    vertical   {}°
Angular resolution {}°
Ray count
    horizontal {}
    vertical   {}
)";

template <Point PointT>
class SphericalScanner
{
  public:
    SphericalScanner(
        const PointT& origin, double horizontal_fov, double vertical_fov, double angular_resolution
    )
        : m_beams{}
    {
        Logger logger{"SphericalScanner"};

        const auto rays_for_resolution = [angular_resolution](double fov) -> size_t {
            return std::ceil(std::fabs(fov / angular_resolution));
        };
        size_t nb_horizontal_rays = rays_for_resolution(horizontal_fov);
        size_t nb_vertical_rays   = rays_for_resolution(vertical_fov);

        logger.debug(
            g_spherical_scanner_info,
            horizontal_fov,
            vertical_fov,
            angular_resolution,
            nb_horizontal_rays,
            nb_vertical_rays
        );

        m_beams.reserve(nb_horizontal_rays * nb_vertical_rays);
        const auto to_rad = [](const double deg) {
            return deg / (180. / std::numbers::pi);
        };

        // NOTE: Using the physics convention of spherical coordinates where :
        // * Theta is the polar angle (z axis)
        // * Phi is the azimutal angle (x axis)
        // * All beams are unit vector so r is always 1
        // All angles are expressed in radians (except inputs which are degrees)

        // Starting at 0, this is to represent the "front" of the scanner.
        const double start_phi   = 0.;
        // In spherical coordinates, 0 theta means 90 degrees up the z axis in
        // horizontal coordinates. The values range from 0 to Pi (0 to 180 degrees).
        // We start the first beam shooting downward at the start of the vertical fov.
        const double start_theta = to_rad(180. - (180. - vertical_fov / 2.));
        const double ang_res_rad = to_rad(angular_resolution);

        for (size_t i = 0; i < nb_horizontal_rays; ++i)
        {
            const double phi = start_phi + (i * ang_res_rad);

            const double sinPhi = std::sin(phi);
            const double cosPhi = std::cos(phi);
            for (size_t j = 0; j < nb_vertical_rays; ++j)
            {
                // The angle is subtracted so that we "shoot" from down to up.
                const double theta = start_theta - (j * ang_res_rad);

                const double cosTheta = std::cos(theta);
                const double sinTheta = std::sin(theta);

                // The direction is calculated using spherical coordinates
                Vector direction;
                direction.x() = sinTheta * cosPhi;
                direction.y() = sinTheta * sinPhi;
                direction.z() = cosTheta;
                m_beams.emplace_back(origin, direction);
            }
        }
    }

    SphericalScanner(SphericalScanner&& other)
        : m_beams{std::move(other.m_beams)}
    {
    }

    SphericalScanner(const SphericalScanner& other)
        : m_beams{other.m_beams}
    {
    }

    SphericalScanner& operator=(SphericalScanner&& other)
    {
        m_beams = std::move(other.m_beams);

        return *this;
    }

    SphericalScanner& operator=(const SphericalScanner& other)
    {
        m_beams = other.m_beams;

        return *this;
    }

    const std::vector<Beam<PointT>>& get_beams() const { return m_beams; }

  private:
    std::vector<Beam<PointT>> m_beams;
};

} // namespace lvox

#endif // LVOX_SPHERICAL_SCANNER_HPP
