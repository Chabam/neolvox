#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/pad_estimators.hpp>

namespace lvox::algorithms::pad_estimators
{

auto beer_lambert(const PadComputeData& data) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits            = data.m_hits;
    const double ray_count       = data.m_counts;
    const double RDI             = hits / static_cast<double>(ray_count);
    const double ray_length      = std::get<double>(data.m_lengths);
    const double mean_ray_length = ray_length / ray_count;

    if (RDI >= 1.)
        return 0.;

    return -(std::log(1. - RDI) / G(mean_ray_length));
}

auto contact_frequency(const PadComputeData& data) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits       = data.m_hits;
    const double ray_count  = data.m_counts;
    const double RDI        = hits / static_cast<double>(ray_count);
    const double ray_length = std::get<double>(data.m_lengths);

    if (RDI >= 1.)
        return 0.;

    return RDI / G(ray_length);
}

auto unequal_path_length_beer_lambert(const PadComputeData& data) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits               = data.m_hits;
    const double ray_count          = data.m_counts;
    const double RDI                = hits / static_cast<double>(ray_count);
    const auto [length, variance]   = std::get<LengthWithVariance>(data.m_lengths);
    const double mean_ray_length    = length / ray_count;
    const double unequal_path_ratio = variance / mean_ray_length;

    double attenuation_coeff;

    if (RDI < 1.)
    {
        const double inv_RDI = 1. - RDI;
        attenuation_coeff =
            (1. / mean_ray_length) * (std::log(inv_RDI) - (1. / (2 * ray_count * inv_RDI)));
    }
    else // RDI == 1.
    {
        attenuation_coeff = std::log(2 * ray_count + 2) / mean_ray_length;
    }

    const double res = (1. / G(attenuation_coeff)) *
                       (1. - std::sqrt(1. - 2 * unequal_path_ratio * attenuation_coeff));

    if (std::isnan(res))
    {
        std::cout << std::format(
                         R"(
hits                = {}
ray_count           = {}
RDI                 = {}
ray_length          = {}
mean_ray_length     = {}
ray_length_variance = {}
unequal_path_ratio  = {}
attenuation_coeff   = {})",
                         hits,
                         ray_count,
                         RDI,
                         length,
                         mean_ray_length,
                         variance,
                         unequal_path_ratio,
                         attenuation_coeff
                     )
                  << std::endl;
        return 0.;
    }

    return res;
}

} // namespace lvox::algorithms::pad_estimators
