#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/pad_compute_methods.hpp>

namespace lvox::algorithms::pad_compute_methods
{

auto BeerLambert::operator()(const ComputeData& data, const Index& index) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const algorithms::CountGrid& hits_grid       = *data.m_hits;
    const auto       hits            = hits_grid.is_na(index) ? 0 : hits_grid.at(index).load();
    const auto       ray_count       = data.m_counts.at(index).load();
    const double     RDI             = hits / static_cast<double>(ray_count);
    const double     mean_ray_length = data.m_lengths.at(index).load() / ray_count;

    return -(std::log(1. - RDI) / G(mean_ray_length));
}

}
