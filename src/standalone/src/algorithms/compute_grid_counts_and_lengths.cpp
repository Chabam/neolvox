#include <atomic>

#include <lvox/algorithms/compute_grid_counts_and_lengths.hpp>
#include <lvox/algorithms/pad_compute_methods.hpp>

#include "lvox/algorithms/grid_traversal.hpp"

namespace lvox::algorithms
{

template <typename PadComputeMethodT>
auto increment_counts_and_lengths(PadComputeMethodT& pcm, const VoxelHitInfo& voxel_hit_info)  -> void
{
    const auto [x, y, z] = voxel_hit_info.m_index;
    pcm.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed);
    pcm.m_lengths.at(x, y, z).fetch_add(
        voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed
    );
}

auto ComputeGridCountsAndLengths::operator()(pcm::BeerLambert& bl) const -> void
{
    increment_counts_and_lengths(bl, m_hit_info);
}

auto ComputeGridCountsAndLengths::operator()(pcm::ContactFrequency& cf) const -> void
{
    increment_counts_and_lengths(cf, m_hit_info);
}

auto ComputeGridCountsAndLengths::operator()(pcm::UnequalPathLengthBeerLambert& uplbl) const -> void
{
    const auto [x, y, z]         = m_hit_info.m_index;
    const double length_in_voxel = m_hit_info.m_distance_in_voxel;
    auto&        count_ref       = uplbl.m_counts.at(x, y, z);
    auto&        length_sum_ref  = uplbl.m_effective_lengths.at(x, y, z);
    auto&        variance_ref    = uplbl.m_effective_lengths_variance.at(x, y, z);

    // TODO: make this configurable maybe?
    // So far it's based on Computree's NeedleFromDimension
    constexpr double elem_length   = 0.06;
    constexpr double elem_diameter = 0.02;
    constexpr double attenuation_coeff =
        (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;

    const double attenuated_length =
        -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

    count_ref += 1;
    length_sum_ref += attenuated_length;

    // Best effort for calculating the variance,
    // might be off because this line definitely
    // represents multiple instructions.
    variance_ref +=
        std::pow(attenuated_length - (length_sum_ref / count_ref), 2) / count_ref;
}

} // namespace lvox::algorithms
