#ifndef LVOX_VOXEL_COO_GRID_HPP
#define LVOX_VOXEL_COO_GRID_HPP

#include <cstddef>
#include <execution>
#include <filesystem>
#include <iterator>
#include <vector>

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

class ChunkedGrid;
class DenseGrid;

class COOGrid
{
  public:
    COOGrid(const ChunkedGrid& grid);
    COOGrid(const DenseGrid& grid);

    auto export_to_h5(
        const std::string&           dataset_name,
        const std::filesystem::path& filename,
        bool                         include_all_data = false
    ) const -> void;

    struct VoxelView
    {
        unsigned int* x;
        unsigned int* y;
        unsigned int* z;
        unsigned int* count;
        unsigned int* hit;
        double*       pad;
        double*       length;
        double*       hits_length;
        double*       length_variance;
    };

    class VoxelViewIterator
    {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = VoxelView;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;

        VoxelViewIterator();
        VoxelViewIterator(COOGrid&);
        VoxelViewIterator(const VoxelViewIterator&);

        auto operator=(const VoxelViewIterator&) -> VoxelViewIterator&;

        auto operator++() -> VoxelViewIterator&;
        auto operator++(int) -> VoxelViewIterator;

        auto operator--() -> VoxelViewIterator&;
        auto operator--(int) -> VoxelViewIterator;

        auto operator+(difference_type diff) const -> VoxelViewIterator;
        auto operator-(difference_type diff) const -> VoxelViewIterator;

        auto operator+=(difference_type diff) -> VoxelViewIterator&;
        auto operator-=(difference_type diff) -> VoxelViewIterator&;

        auto operator*() -> value_type;
        auto operator->() -> pointer;

        auto operator==(const VoxelViewIterator&) const -> bool;
        auto operator!=(const VoxelViewIterator&) const -> bool;

        auto operator<(const VoxelViewIterator&) const -> bool;
        auto operator>(const VoxelViewIterator&) const -> bool;

        auto operator<=(const VoxelViewIterator&) const -> bool;
        auto operator>=(const VoxelViewIterator&) const -> bool;

        auto operator[](size_t index) -> VoxelViewIterator;

      private:
        auto       update_value() -> void;
        size_t     m_index;
        COOGrid*   m_grid;
        value_type m_value;
    };

    auto compute_pad(algorithms::pad_estimators::BeerLambert) -> void;
    auto compute_pad(algorithms::pad_estimators::ContactFrequency) -> void;
    auto compute_pad(algorithms::pad_estimators::UnequalPathLengthBeerLambert) -> void;

    auto begin() -> VoxelViewIterator { return VoxelViewIterator{*this}; }
    auto end() -> VoxelViewIterator { return VoxelViewIterator{}; }

  private:
    std::vector<unsigned int> m_xs;
    std::vector<unsigned int> m_ys;
    std::vector<unsigned int> m_zs;
    std::vector<unsigned int> m_counts;
    std::vector<unsigned int> m_hits;
    std::vector<double>       m_pads;
    std::vector<double>       m_lengths;
    std::vector<double>       m_hits_lengths;
    std::vector<double>       m_lengths_variance;
    lvox::BoundedGrid         m_bounded_grid;

    template <typename PadFunc>
    auto compute_pad_impl(PadFunc&& func) -> void
    {
        std::vector<size_t> indexes{m_pads.size(), std::allocator<size_t>{}};
        std::iota(indexes.begin(), indexes.end(), 0);
        std::transform(
            std::execution::par_unseq,
            indexes.begin(),
            indexes.end(),
            m_pads.begin(),
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
};

} // namespace lvox

#endif // LVOX_VOXEL_COO_GRID_HPP
