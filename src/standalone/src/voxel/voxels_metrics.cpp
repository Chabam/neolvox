#include <algorithm>
#include <ranges>

#include <lvox/voxel/voxels_metrics.hpp>

#include "lvox/algorithms/pad_estimators.hpp"
#include "lvox/types.hpp"

namespace lvox
{

VoxelsMetrics::VoxelsMetrics(const Grid& grid, bool compute_variance)
    : m_grid{grid}
    , m_voxels_presence{grid.cell_count() / s_bit_count, std::allocator<std::bitset<s_bit_count>>{}}
    , m_voxel_idx_mapping{}
    , m_coords{}
    , m_hits{}
    , m_counts{}
    , m_lengths{}
    , m_lengths_variances{std::invoke([compute_variance]() -> decltype(m_lengths_variances) {
           if (compute_variance)
              return std::vector<double>{};
          return {};
      })}
      , m_pad{}
{
}

auto VoxelsMetrics::register_hit(const Index3D& idx) -> void
{
    const auto cur_idx = insert_if_not_found(idx);
    m_hits[cur_idx] += 1;
}

auto VoxelsMetrics::add_length_and_count(const Index3D& idx, double length) -> void
{
    const auto cur_idx = insert_if_not_found(idx);

    m_counts[cur_idx] += 1;
    m_lengths[cur_idx] += length;
}

auto VoxelsMetrics::add_length_count_and_variance(const Index3D& idx, double length) -> void
{
    const auto cur_idx = insert_if_not_found(idx);

    (*m_lengths_variances)[cur_idx] += length;
    m_counts[cur_idx] += 1;
    m_lengths[cur_idx] += length;
}

auto VoxelsMetrics::compute_pad(algorithms::pe::BeerLambert) -> void
{
    compute_pad_impl([this](size_t voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits            = m_hits[voxel_idx];
        const double ray_count       = m_counts[voxel_idx];
        const double RDI             = hits / static_cast<double>(ray_count);
        const double ray_length      = m_lengths[voxel_idx];
        const double mean_ray_length = ray_length / ray_count;

        if (RDI >= 1.)
            return 0.;

        return -(std::log(1. - RDI) / G(mean_ray_length));
    });
}

auto VoxelsMetrics::compute_pad(algorithms::pe::ContactFrequency) -> void
{
    compute_pad_impl([this](size_t voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits       = m_hits[voxel_idx];
        const double ray_count  = m_counts[voxel_idx];
        const double RDI        = hits / static_cast<double>(ray_count);
        const double ray_length = m_lengths[voxel_idx];

        if (RDI >= 1.)
            return 0.;

        return RDI / G(ray_length);
    });
}

auto VoxelsMetrics::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
{
    compute_pad_impl([this](size_t voxel_idx) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits               = m_hits[voxel_idx];
        const double ray_count          = m_counts[voxel_idx];
        const double RDI                = hits / static_cast<double>(ray_count);
        const double length             = m_lengths[voxel_idx];
        const double variance           = (*m_lengths_variances)[voxel_idx];
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
    });
}

auto VoxelsMetrics::merge(VoxelsMetrics&& other) -> void
{
    const bool merge_variance = m_lengths_variances.has_value();
    for (size_t bitset_idx = 0; bitset_idx < m_voxels_presence.size(); ++bitset_idx)
    {
        auto& voxels_presence = m_voxels_presence[bitset_idx];
        auto& other_voxels_presence = other.m_voxels_presence[bitset_idx];
        const auto voxels_to_merge = other_voxels_presence.count();
        auto       merged_voxels         = 0;
        auto       voxel_in_bitset_idx = 0;


        while (merged_voxels != voxels_to_merge)
        {
            const auto voxel_idx = bitset_idx * s_bit_count + voxel_in_bitset_idx;
            // Nothing to copy
            if (!other_voxels_presence.test(voxel_in_bitset_idx))
            {
                ++voxel_in_bitset_idx;
                continue;
            }

            // Voxel exists in both, sum them
            if (voxels_presence.test(voxel_in_bitset_idx))
            {
                const auto cur_idx = m_voxel_idx_mapping[voxel_idx];
                const auto other_idx = other.m_voxel_idx_mapping[voxel_idx];
                m_hits[cur_idx] += other.m_hits[other_idx];
                m_counts[cur_idx] += other.m_counts[other_idx];
                m_lengths[cur_idx] += other.m_lengths[other_idx];

                if (merge_variance)
                    (*m_lengths_variances)[cur_idx] += (*other.m_lengths_variances)[other_idx];

                ++voxel_in_bitset_idx;
                ++merged_voxels;
                continue;
            }

            // Just add the existing values at the end of the vectors
            const auto new_idx = m_coords.size();
            if ((voxel_idx + 1) > m_voxel_idx_mapping.size())
                m_voxel_idx_mapping.resize(voxel_idx + 1);
            m_voxel_idx_mapping[voxel_idx] = new_idx;

            const auto other_idx = other.m_voxel_idx_mapping[voxel_idx];
            m_coords.push_back(std::move(other.m_coords[other_idx]));
            m_hits.push_back(std::move(other.m_hits[other_idx]));
            m_counts.push_back(std::move(other.m_counts[other_idx]));
            m_lengths.push_back(std::move(other.m_lengths[other_idx]));

            if (merge_variance)
                m_lengths_variances->push_back(std::move((*other.m_lengths_variances)[other_idx]));

            ++voxel_in_bitset_idx;
            ++merged_voxels;
        }
    }
}

    auto VoxelsMetrics::index3d_to_flat_index(const Index3D& idx) const -> size_t
    {
        const auto [x, y, z] = idx;
        return x + y * m_grid.dim_x() + z * m_grid.dim_x() * m_grid.dim_y();
    }

    auto VoxelsMetrics::insert_if_not_found(const Index3D& idx) -> size_t
    {
        return insert_if_not_found(index3d_to_flat_index(idx));
    }

    auto VoxelsMetrics::insert_if_not_found(size_t idx) -> size_t
    {
        const auto bitset_idx = idx / s_bit_count;
        const auto idx_in_bitset = idx % s_bit_count;

        if (m_voxels_presence[bitset_idx].test(idx_in_bitset))
        {
            return m_voxel_idx_mapping[idx];
        }

        auto new_idx = m_coords.size();

        if ((idx + 1) > m_voxel_idx_mapping.size())
            m_voxel_idx_mapping.resize(idx + 1);

        m_voxel_idx_mapping[idx] = new_idx;

        m_coords.push_back(idx);
        m_hits.emplace_back();
        m_counts.emplace_back();
        m_lengths.emplace_back();

        if (m_lengths_variances)
            m_lengths_variances->emplace_back();

        m_voxels_presence[bitset_idx].flip(idx_in_bitset);

        return new_idx;
    }

} // namespace lvox
