#include <algorithm>
#include <ranges>

#include <lvox/voxel/voxels_metrics.hpp>

#include "lvox/algorithms/pad_estimators.hpp"
#include "lvox/types.hpp"

namespace lvox
{

VoxelsMetrics::VoxelsMetrics(const Grid& grid, bool compute_variance)
    : m_grid{grid}
    , m_chunk_mapping{}
    , m_coords{}
    , m_hits{}
    , m_counts{}
    , m_lengths{}
    , m_lengths_variances{std::invoke([compute_variance]() -> decltype(m_lengths_variances) {
        if (compute_variance)
            return chunked_vector<double>{};
        return {};
    })}
    , m_pad{}
{
}

auto VoxelsMetrics::register_hit(const Index3D& idx) -> void
{
    const auto [chunk_idx, cur_idx] = insert_if_not_found(idx);
    m_hits[chunk_idx][cur_idx] += 1;
}

auto VoxelsMetrics::add_length_and_count(const Index3D& idx, double length) -> void
{
    const auto [chunk_idx, cur_idx] = insert_if_not_found(idx);

    m_counts[chunk_idx][cur_idx] += 1;
    m_lengths[chunk_idx][cur_idx] += length;
}

auto VoxelsMetrics::add_length_count_and_variance(const Index3D& idx, double length) -> void
{
    const auto [chunk_idx, cur_idx] = insert_if_not_found(idx);

    (*m_lengths_variances)[chunk_idx][cur_idx] += length;
    m_counts[chunk_idx][cur_idx] += 1;
    m_lengths[chunk_idx][cur_idx] += length;
}

auto VoxelsMetrics::compute_pad(algorithms::pe::BeerLambert) -> void
{
    compute_pad_impl([this](size_t chunk_idx, size_t pos) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits            = m_hits[chunk_idx][pos];
        const double ray_count       = m_counts[chunk_idx][pos];
        const double RDI             = hits / static_cast<double>(ray_count);
        const double ray_length      = m_lengths[chunk_idx][pos];
        const double mean_ray_length = ray_length / ray_count;

        if (RDI >= 1.)
            return 0.;

        return -(std::log(1. - RDI) / G(mean_ray_length));
    });
}

auto VoxelsMetrics::compute_pad(algorithms::pe::ContactFrequency) -> void
{
    compute_pad_impl([this](size_t chunk_idx, size_t pos) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits       = m_hits[chunk_idx][pos];
        const double ray_count  = m_counts[chunk_idx][pos];
        const double RDI        = hits / static_cast<double>(ray_count);
        const double ray_length = m_lengths[chunk_idx][pos];

        if (RDI >= 1.)
            return 0.;

        return RDI / G(ray_length);
    });
}

auto VoxelsMetrics::compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void
{
    compute_pad_impl([this](size_t chunk_idx, size_t pos) -> double {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double hits               = m_hits[chunk_idx][pos];
        const double ray_count          = m_counts[chunk_idx][pos];
        const double RDI                = hits / static_cast<double>(ray_count);
        const double length             = m_lengths[chunk_idx][pos];
        const double variance           = (*m_lengths_variances)[chunk_idx][pos];
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
    for (auto&& [chunk_idx, mapped_idx] : std::move(other.m_chunk_mapping))
    {
        if (!m_chunk_mapping.contains(chunk_idx))
        {
            const auto new_chunk_idx   = m_coords.size();
            m_chunk_mapping[chunk_idx] = new_chunk_idx;
            m_coords.push_back(std::move(other.m_coords[mapped_idx]));
            m_hits.push_back(std::move(other.m_hits[mapped_idx]));
            m_counts.push_back(std::move(other.m_counts[mapped_idx]));
            m_lengths.push_back(std::move(other.m_lengths[mapped_idx]));
            if (merge_variance)
                m_lengths_variances->push_back(std::move((*other.m_lengths_variances)[mapped_idx]));

            continue;
        }

        // If chunk is contained, we must merge it.
        auto&& other_coords  = std::move(other.m_coords[mapped_idx]);
        auto&& other_hits    = std::move(other.m_hits[mapped_idx]);
        auto&& other_counts  = std::move(other.m_counts[mapped_idx]);
        auto&& other_lengths = std::move(other.m_lengths[mapped_idx]);

        const auto current_mapping = m_chunk_mapping[chunk_idx];

        auto& coords  = m_coords[current_mapping];
        auto& hits    = m_hits[current_mapping];
        auto& counts  = m_counts[current_mapping];
        auto& lengths = m_lengths[current_mapping];

        const auto coords_size = coords.size();
        for (size_t i = 0; i < other_coords.size(); ++i)
        {
            if (i >= coords_size)
            {
                coords.push_back(other_coords[i]);
                hits.push_back(other_hits[i]);
                counts.push_back(other_counts[i]);
                lengths.push_back(other_lengths[i]);

                if (merge_variance)
                    m_lengths_variances->push_back((*other.m_lengths_variances)[i]);

                continue;
            }

            if (other_coords[i] == coords[i])
            {
                hits[i] += other_hits[i];
                counts[i] += other_counts[i];
                lengths[i] += other_lengths[i];

                if (merge_variance)
                    (*m_lengths_variances)[current_mapping][i] +=
                        (*other.m_lengths_variances)[mapped_idx][i];
            }
            else if (other_coords[i] < coords[i])
            {
                coords.insert(coords.begin() + i, other_coords[i]);
                hits.insert(hits.begin() + i, other_hits[i]);
                counts.insert(counts.begin() + i, other_counts[i]);
                lengths.insert(lengths.begin() + i, other_lengths[i]);

                if (merge_variance)
                    (*m_lengths_variances)[current_mapping].insert(
                        (*other.m_lengths_variances)[mapped_idx].begin() + i,
                        (*other.m_lengths_variances)[mapped_idx][i]
                    );
            }
        }
    }
}

auto VoxelsMetrics::get_rel_chunk_idx(const Index3D& index) const -> size_t
{
    return get_rel_chunk_idx(index3d_to_flat_index(index));
}

auto VoxelsMetrics::get_rel_chunk_idx(Index index) const -> size_t
{
    return m_chunk_mapping.at(index / s_chunk_size);
}

auto VoxelsMetrics::index3d_to_flat_index(const Index3D& idx) const -> Index
{
    const auto [x, y, z] = idx;
    return x + y * m_grid.dim_x() + z * m_grid.dim_x() * m_grid.dim_y();
}

auto VoxelsMetrics::insert_if_not_found(const Index3D& idx) -> std::pair<size_t, size_t>
{
    return insert_if_not_found(index3d_to_flat_index(idx));
}

auto VoxelsMetrics::insert_if_not_found(Index idx) -> std::pair<size_t, size_t>
{
    const auto idx_to_chunk = idx / s_chunk_size;
    auto       it           = m_chunk_mapping.find(idx_to_chunk);

    if (it == m_chunk_mapping.end())
    {
        const auto new_chunk_idx      = m_coords.size();
        m_chunk_mapping[idx_to_chunk] = new_chunk_idx;
        m_coords.push_back({idx});
        m_hits.emplace_back(1);
        m_counts.emplace_back(1);
        m_lengths.emplace_back(1);

        if (m_lengths_variances)
            (*m_lengths_variances).emplace_back(1);

        return {new_chunk_idx, 0};
    }

    const size_t chunk_idx     = it->second;
    auto&        hits_chunk    = m_hits[chunk_idx];
    auto&        coords_chunk  = m_coords[chunk_idx];
    auto&        counts_chunk  = m_counts[chunk_idx];
    auto&        lengths_chunk = m_lengths[chunk_idx];

    auto lower = std::lower_bound(coords_chunk.begin(), coords_chunk.end(), idx);

    if (lower == coords_chunk.end())
    {
        coords_chunk.push_back(idx);
        hits_chunk.emplace_back();
        counts_chunk.emplace_back();
        lengths_chunk.emplace_back();

        if (m_lengths_variances)
            (*m_lengths_variances)[chunk_idx].emplace_back();

        return {chunk_idx, coords_chunk.size() - 1};
    }

    if (*lower == idx)
    {
        return {chunk_idx, std::distance(coords_chunk.begin(), lower)};
    }

    const auto inserted_idx = std::distance(coords_chunk.begin(), coords_chunk.insert(lower, idx));

    hits_chunk.emplace(hits_chunk.begin() + inserted_idx);
    counts_chunk.emplace(counts_chunk.begin() + inserted_idx);
    lengths_chunk.emplace(lengths_chunk.begin() + inserted_idx);

    if (m_lengths_variances)
    {
        auto& lengths_variances_chunk = (*m_lengths_variances)[chunk_idx];
        lengths_variances_chunk.emplace(lengths_variances_chunk.begin() + inserted_idx);
    }

    return {chunk_idx, inserted_idx};
}

} // namespace lvox
