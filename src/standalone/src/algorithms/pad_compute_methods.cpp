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

    const double                 hits            = hits_grid.at(index);
    const double                 ray_count       = data.m_counts.at(index);
    const double                 RDI             = hits / static_cast<double>(ray_count);
    const double                 ray_length      = data.m_lengths.at(index);
    const double                 mean_ray_length = ray_length / ray_count;

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

    const double                 hits            = hits_grid.at(index);
    const double                 ray_count       = data.m_counts.at(index);
    const double                 RDI             = hits / static_cast<double>(ray_count);
    const double                 ray_length      = data.m_lengths.at(index);

    if (RDI >= 1.)
        return 0.;

    return RDI/G(ray_length);
}

} // namespace lvox::algorithms::pad_compute_methods
