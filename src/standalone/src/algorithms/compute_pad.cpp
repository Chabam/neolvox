#include <atomic>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/pad_compute_methods.hpp>

namespace lvox::algorithms
{

ComputePAD::ComputePAD(lvox::Bounds bounds, const ComputeOptions& options)
    : m_res{bounds, options.voxel_size}
{
}

auto ComputePAD::operator()(const pcm::BeerLambert& bl, Index index) -> void
{
    if (bl.m_hits.is_na(index))
        return;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits            = bl.m_hits.at(index);
    const double ray_count       = bl.m_counts.at(index);
    const double RDI             = hits / static_cast<double>(ray_count);
    const double ray_length      = bl.m_lengths.at(index);
    const double mean_ray_length = ray_length / ray_count;

    if (RDI >= 1.)
        return;

    m_res.at(index).fetch_add(
        -(std::log(1. - RDI) / G(mean_ray_length)),
        std::memory_order_relaxed
    );
}

auto ComputePAD::operator()(const pcm::ContactFrequency& cf, Index index) -> void
{
    if (cf.m_hits.is_na(index))
        return;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits       = cf.m_hits.at(index);
    const double ray_count  = cf.m_counts.at(index);
    const double RDI        = hits / static_cast<double>(ray_count);
    const double ray_length = cf.m_lengths.at(index);

    if (RDI >= 1.)
        return;

    m_res.at(index).fetch_add(
        RDI / G(ray_length),
        std::memory_order_relaxed
    );
}

auto ComputePAD::operator()(const pcm::UnequalPathLengthBeerLambert& uplbl, Index index) -> void
{
    if (uplbl.m_hits.is_na(index))
        return;

    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const double hits                = uplbl.m_hits.at(index);
    const double ray_count           = uplbl.m_counts.at(index);
    const double RDI                 = hits / static_cast<double>(ray_count);
    const double ray_length          = uplbl.m_effective_lengths.at(index);
    const double mean_ray_length     = ray_length / ray_count;
    const double ray_length_variance = uplbl.m_effective_lengths_variance.at(index);
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
        return;
    }

    m_res.at(index).fetch_add(res, std::memory_order_relaxed);
}

}
