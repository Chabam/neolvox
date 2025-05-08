#include <lvox/logger/logger.hpp>
#include <lvox/scanner/spherical_scanner.hpp>

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

SphericalScanner::SphericalScanner(
    const Vector& origin, double horizontal_fov, double vertical_fov, double angular_resolution
)
    : m_beams{}
{
    Logger logger{"SphericalScanner"};

    const auto rays_for_resolution = [angular_resolution](double fov) -> Index {
        return std::ceil(std::fabs(fov / angular_resolution));
    };
    Index nb_horizontal_rays = rays_for_resolution(horizontal_fov);
    Index nb_vertical_rays   = rays_for_resolution(vertical_fov);

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

    for (Index i = 0; i < nb_horizontal_rays; ++i)
    {
        const double phi = start_phi + (i * ang_res_rad);

        const double sinPhi = std::sin(phi);
        const double cosPhi = std::cos(phi);
        for (Index j = 0; j < nb_vertical_rays; ++j)
        {
            // The angle is subtracted so that we "shoot" from down to up.
            const double theta = start_theta - (j * ang_res_rad);

            const double cosTheta = std::cos(theta);
            const double sinTheta = std::sin(theta);

            // The direction is calculated using spherical coordinates
            Eigen::Vector3d direction;
            direction.x() = sinTheta * cosPhi;
            direction.y() = sinTheta * sinPhi;
            direction.z() = cosTheta;
            m_beams.emplace_back(origin, direction);
        }
    }
}

SphericalScanner::SphericalScanner(SphericalScanner&& other)
    : m_beams{std::move(other.m_beams)}
{
}

SphericalScanner::SphericalScanner(const SphericalScanner& other)
    : m_beams{other.m_beams}
{
}

auto SphericalScanner::operator=(SphericalScanner&& other) -> SphericalScanner&
{
    m_beams = std::move(other.m_beams);

    return *this;
}

auto SphericalScanner::operator=(const SphericalScanner& other) -> SphericalScanner&
{
    m_beams = other.m_beams;

    return *this;
}

} // namespace lvox
