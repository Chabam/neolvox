#ifndef LVOX_DENSE_GRID_HPP
#define LVOX_DENSE_GRID_HPP

#include <algorithm>
#include <atomic>
#include <execution>
#include <filesystem>

#include <lvox/types.hpp>
#include <lvox/voxel/bounded_grid.hpp>

namespace lvox
{

namespace algorithms::pad_estimators
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
} // namespace algorithms::pad_estimators

class DenseGrid
{
  public:
    DenseGrid(const Bounds& bounds, double cell_size, bool compute_variance);
    DenseGrid(DenseGrid&& other);

    auto register_hit(const Index3D& voxel_idx) -> void;
    auto add_length_and_count(const Index3D& voxel_idx, double length) -> void;
    auto add_length_count_and_variance(const Index3D& voxel_idx, double length) -> void;

    auto compute_pad(algorithms::pad_estimators::BeerLambert) -> void;
    auto compute_pad(algorithms::pad_estimators::ContactFrequency) -> void;
    auto compute_pad(algorithms::pad_estimators::UnequalPathLengthBeerLambert) -> void;

    auto export_as_coo_to_h5(
        const std::string&           dataset_name,
        const std::filesystem::path& filename,
        bool                         include_all_data = false
    ) const -> void;

    auto get_bounded_grid() const -> const BoundedGrid& { return m_bounded_grid; }

  private:
    BoundedGrid m_bounded_grid;
    using atomic_f64 = std::atomic<double>;
    std::vector<std::atomic_uint> m_hits;
    std::vector<std::atomic_uint> m_counts;
    std::vector<atomic_f64>       m_lengths;
    std::vector<atomic_f64>       m_lengths_variance;
    std::vector<atomic_f64>       m_pad;

    template <typename PadFunc>
    auto compute_pad_impl(PadFunc&& func) -> void
    {
        std::vector<size_t> indexes{m_pad.size(), std::allocator<size_t>{}};
        std::iota(indexes.begin(), indexes.end(), 0);
        std::transform(
            std::execution::par_unseq,
            indexes.begin(),
            indexes.end(),
            m_pad.begin(),
            [this, func](auto idx) -> double {
                // TODO: Make the treshold configurable
                if (m_counts[idx] < 5)
                {
                    return 0;
                }

                return func(idx);
            }
        );
    }

    auto index3d_to_flat_idx(const Index3D& voxel_idx) const -> size_t;
};

} // namespace lvox

#endif // LVOX_DENSE_GRID_HPP
