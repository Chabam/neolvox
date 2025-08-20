#ifndef LVOX_VOXELS_METRICS_HPP
#define LVOX_VOXELS_METRICS_HPP

#include <vector>
#include <ranges>
#include <execution>

#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/algorithms/pad_estimators.hpp>

namespace lvox
{

class VoxelsMetrics
{
  public:
    // This chunk size was decided based on the size of the cache line
    static constexpr auto s_chunk_size = (sizeof(char) * 64) / sizeof(double);

    template <typename T>
    using chunked_vector = std::vector<std::vector<T>>;

    VoxelsMetrics(const Grid& grid, bool compute_variance = false);

    auto register_hit(const Index3D& idx) -> size_t;
    auto add_length_and_count(const Index3D& idx, double length) -> size_t;
    auto add_length_count_and_variance(const Index3D& idx, double length) -> size_t;

    auto compute_pad(algorithms::pe::BeerLambert) -> void;
    auto compute_pad(algorithms::pe::ContactFrequency) -> void;
    auto compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void;

    auto merge(VoxelsMetrics&& other) -> void;

  private:
    Grid                   m_grid;
    size_t                 m_chunk_count;
    chunked_vector<Index>  m_coords;
    chunked_vector<size_t> m_hits;
    chunked_vector<size_t> m_counts;
    chunked_vector<double> m_lengths;
    std::optional<chunked_vector<double>> m_lengths_variances;
    chunked_vector<double> m_pad;

    auto get_rel_chunk_idx(Index index) const -> size_t;
    auto get_rel_chunk_idx(const Index3D& index) const -> size_t;

    [[nodiscard]]
    auto index3d_to_flat_index(const Index3D& idx) const -> Index;
    [[nodiscard]]
    auto insert_if_not_found(Index idx) -> size_t;
    [[nodiscard]]
    auto insert_if_not_found(const Index3D& idx) -> size_t;

    template <typename PadFunc>
    auto compute_pad_impl(PadFunc&& func) -> void
    {
        m_pad = chunked_vector<double>{m_chunk_count};
        for (size_t chunk_id = 0; chunk_id < m_chunk_count; ++chunk_id)
        {
            const auto chunk_size = m_coords[chunk_id].size();
            m_pad[chunk_id].resize(chunk_size);
            for (size_t i = 0; i < chunk_size; ++i)
            {
                // TODO: Make the treshold configurable
                if (m_counts[chunk_id][i] < 5)
                {
                    m_pad[chunk_id][i] = 0.;
                    continue;
                }
                m_pad[chunk_id][i] = func(chunk_id, i);

            }
        }

    }
};

} // namespace lvox

#endif // LVOX_VOXELS_METRICS_HPP
