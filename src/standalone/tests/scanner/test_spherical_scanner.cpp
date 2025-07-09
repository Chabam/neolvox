#include <cmath>
#include <numbers>
#include <ranges>

#include <gtest/gtest.h>

#include <lvox/scanner/spherical_scanner.hpp>

TEST(SphericalScannerTests, creation_all_6_axis)
{

    // Perfect sphere with a ray going at every 90 degrees, so
    // all axis directions (positive and negative) should be seen.
    const lvox::Point origin{0., 0., 0.};
    const double      h_fov = 360.;
    const double      v_fov = 360.;
    const double      res   = 90.;

    const lvox::SphericalScanner scanner{origin, h_fov, v_fov, res};
    EXPECT_EQ(16, scanner.get_beams().size());
    const std::set<std::array<double, 3>> beams =
        scanner.get_beams() |
        std::ranges::views::transform([](const lvox::Beam& beam) -> std::array<double, 3> {
            const auto dir = beam.direction();

            // Rounding the values because of floating point errors.
            return {std::round(dir.x()), std::round(dir.y()), std::round(dir.z())};
        }) |
        // To remove duplicates
        std::ranges::to<std::set>();

    // clang-format off
    const std::set<std::array<double, 3>> all_axis{
        // X
        {-1., 0., 0.},
        {1., 0., 0.},
        // Y
        {0., -1., 0.},
        {0., 1., 0.},
        // Z
        {0., 0., -1.},
        {0., 0., 1.}
    };
    // clang-format on

    ASSERT_EQ(beams, all_axis);
}

TEST(SphericalScannerTests, verify_angle_resolution)
{

    const lvox::Point origin{0., 1., 0.};
    const double      h_fov        = 360.;
    const double      v_fov        = 300.;
    const double      res          = 1.;
    const lvox::Index      nb_vert_rays = std::ceil(v_fov / res);
    const lvox::Index      nb_hor_rays  = std::ceil(h_fov / res);
    const double      angle_vert   = v_fov / nb_vert_rays;
    const double      angle_hor    = h_fov / nb_hor_rays;
    const double      epsilon      = 0.000001;

    const lvox::SphericalScanner scanner{origin, h_fov, v_fov, res};
    // Less or equal beacause of rounding
    ASSERT_LE(nb_vert_rays * nb_hor_rays, scanner.get_beams().size());
    const std::vector<lvox::Beam> beams      = scanner.get_beams();
    const auto                    to_degrees = [](const double val) {
        return val * 180. / std::numbers::pi;
    };

    // Check for vertical resolution
    for (lvox::Index i = 1; i < nb_vert_rays; ++i)
    {
        const lvox::Beam cur_beam  = beams[i];
        const lvox::Beam prev_beam = beams[i - 1];
        ASSERT_EQ(origin, cur_beam.origin());
        ASSERT_EQ(origin, prev_beam.origin());
        ASSERT_LE(
            std::abs(
                angle_vert - to_degrees(std::acos(prev_beam.direction().dot(cur_beam.direction())))
            ),
            epsilon
        );
    }

    // Check for horizontal resolution (+60 beams to end up flat on the x/y axis)
    for (lvox::Index i = nb_vert_rays + 60; i < scanner.get_beams().size(); i += nb_vert_rays)
    {
        const lvox::Beam cur_beam  = beams[i];
        const lvox::Beam prev_beam = beams[i - nb_vert_rays];
        ASSERT_EQ(origin, cur_beam.origin());
        ASSERT_EQ(origin, prev_beam.origin());
        ASSERT_LE(
            std::abs(
                angle_hor - to_degrees(std::acos(prev_beam.direction().dot(cur_beam.direction())))
            ),
            epsilon
        );
    }
}
