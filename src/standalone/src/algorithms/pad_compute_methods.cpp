#include <stdexcept>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/pad_compute_methods.hpp>

namespace lvox::algorithms::pad_compute_methods
{

auto BeerLambert::operator()(const ComputeData& data, const Index& index) -> double
{
    const algorithms::CountGrid& hits_grid = *data.m_hits;

    if (hits_grid.is_na(index))
        return 0.;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits            = hits_grid.at(index);
    const double ray_count       = data.m_counts.at(index);
    const double RDI             = hits / static_cast<double>(ray_count);
    const double ray_length      = data.m_lengths.at(index);
    const double mean_ray_length = ray_length / ray_count;

    if (RDI >= 1.)
        return 0.;

    return -(std::log(1. - RDI) / G(mean_ray_length));
}

auto ContactFrequency::operator()(const ComputeData& data, const Index& index) -> double
{
    const algorithms::CountGrid& hits_grid = *data.m_hits;

    if (hits_grid.is_na(index))
        return 0.;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits       = hits_grid.at(index);
    const double ray_count  = data.m_counts.at(index);
    const double RDI        = hits / static_cast<double>(ray_count);
    const double ray_length = data.m_lengths.at(index);

    if (RDI >= 1.)
        return 0.;

    return RDI / G(ray_length);
}

auto UnequalPathLengthBeerLambert::operator()(const ComputeData& data, const Index& index) -> double
{
    const algorithms::CountGrid& hits_grid = *data.m_hits;

    if (hits_grid.is_na(index))
        return 0.;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits                = hits_grid.at(index);
    const double ray_count           = data.m_counts.at(index);
    const double RDI                 = hits / static_cast<double>(ray_count);
    const double ray_length          = data.m_lengths.at(index);
    const double mean_ray_length     = ray_length / ray_count;
    const double ray_length_variance = data.m_lengths_variance->at(index);
    const double unequal_path_ratio  = ray_length_variance / mean_ray_length;

    double attenuation_coeff;

    if (RDI < 1.)
    {
        const double inv_RDI     = 1. - RDI;
        attenuation_coeff = std::log(inv_RDI) - (1. / (2 * ray_count * inv_RDI));
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
attenuation_coeff   = {}
)",
                         hits,
                         ray_count,
                         RDI,
                         ray_length,
                         mean_ray_length,
                         ray_length_variance,
                         unequal_path_ratio,
                         attenuation_coeff
                     )
        << std::endl;
        return 0.;
    }
    return res;
}

} // namespace lvox::algorithms::pad_compute_methods
