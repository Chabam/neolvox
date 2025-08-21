#ifndef LVOX_VOXELS_METRICS_HPP
#define LVOX_VOXELS_METRICS_HPP

#include <execution>
#include <vector>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

class VoxelsMetrics
{
  public:
    VoxelsMetrics(const Grid& grid, bool compute_variance = false);

    auto register_hit(const Index3D& idx) -> void;
    auto add_length_and_count(const Index3D& idx, double length) -> void;
    auto add_length_count_and_variance(const Index3D& idx, double length) -> void;

    auto compute_pad(algorithms::pe::BeerLambert) -> void;
    auto compute_pad(algorithms::pe::ContactFrequency) -> void;
    auto compute_pad(algorithms::pe::UnequalPathLengthBeerLambert) -> void;

    auto merge(VoxelsMetrics&& other) -> void;

  private:
    // To fit the cache line?
    static constexpr auto                 s_bit_count = 64;
    Grid                                  m_grid;
    std::vector<std::bitset<s_bit_count>> m_voxels_presence;
    std::vector<size_t>                   m_voxel_idx_mapping;
    std::vector<size_t>                   m_coords;
    std::vector<size_t>                   m_hits;
    std::vector<size_t>                   m_counts;
    std::vector<double>                   m_lengths;
    std::optional<std::vector<double>>    m_lengths_variances;
    std::vector<double>                   m_pad;

    [[nodiscard]]
    auto index3d_to_flat_index(const Index3D& idx) const -> size_t;
    [[nodiscard]]
    auto insert_if_not_found(size_t idx) -> size_t;
    [[nodiscard]]
    auto insert_if_not_found(const Index3D& idx) -> size_t;

    template <typename PadFunc>
    auto compute_pad_impl(PadFunc&& func) -> void
    {
        const auto voxel_counts = m_coords.size();
        m_pad                   = std::vector<double>{voxel_counts, std::allocator<double>{}};
        std::iota(m_pad.begin(), m_pad.end(), 0);
        std::transform(
            std::execution::par_unseq,
            m_pad.begin(),
            m_pad.end(),
            m_pad.begin(),
            [this, func](size_t idx) -> double {
                // TODO: Make the treshold configurable
                if (m_counts[idx] < 5)
                    return 0;
                return func(idx);
            }
        );
    }
};

} // namespace lvox

#endif // LVOX_VOXELS_METRICS_HPP
