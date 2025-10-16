#ifndef LVOX_VOXEL_COO_GRID_HPP
#define LVOX_VOXEL_COO_GRID_HPP

#include <cstddef>
#include <filesystem>
#include <iterator>
#include <vector>

#include <lvox/types.hpp>
#include <lvox/voxel/bounded_grid.hpp>

namespace lvox
{

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

    struct VoxelData
    {
        std::vector<unsigned int>::iterator x;
        std::vector<unsigned int>::iterator y;
        std::vector<unsigned int>::iterator z;
        std::vector<unsigned int>::iterator count;
        std::vector<unsigned int>::iterator hit;
        std::vector<double>::iterator       pad;
        std::vector<double>::iterator       lengths;
        std::vector<double>::iterator       hits_length;
        std::vector<double>::iterator       lengths_variance;
    };

    struct ConstVoxelData
    {
        std::vector<unsigned int>::const_iterator x;
        std::vector<unsigned int>::const_iterator y;
        std::vector<unsigned int>::const_iterator z;
        std::vector<unsigned int>::const_iterator count;
        std::vector<unsigned int>::const_iterator hit;
        std::vector<double>::const_iterator       pad;
        std::vector<double>::const_iterator       lengths;
        std::vector<double>::const_iterator       hits_length;
        std::vector<double>::const_iterator       lengths_variance;
    };

    template <typename ViewT, typename GridPtrT>
    class VoxelDataIterator
    {
        friend class COOGrid;
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = ViewT;
        using difference_type   = std::ptrdiff_t;
        using pointer           = value_type*;
        using reference         = value_type&;
        using const_reference   = const value_type&;

        VoxelDataIterator() = default;
        VoxelDataIterator(const VoxelDataIterator&)                    = default;

        auto operator=(const VoxelDataIterator&) -> VoxelDataIterator& = default;

        auto operator++() -> VoxelDataIterator&
        {
            *this += 1;
            return *this;
        }

        auto operator++(int) -> VoxelDataIterator
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        auto operator--() -> VoxelDataIterator&
        {
            *this -= 1;
            return *this;
        }

        auto operator--(int) -> VoxelDataIterator
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        auto operator+(difference_type diff) const -> VoxelDataIterator
        {
            auto tmp = *this;
            tmp.m_index += diff;
            tmp.update_value();
            return tmp;
        }

        friend auto operator+(difference_type other, const VoxelDataIterator& diff)
            -> VoxelDataIterator
        {
            return other + diff;
        }

        auto operator-(difference_type diff) const -> VoxelDataIterator
        {
            auto tmp = *this - diff;
            tmp.m_index -= diff;
            tmp.update_value();
            return tmp;
        }

        auto operator-(const VoxelDataIterator& other) const -> difference_type
        {
            return m_index - other.m_index;
        }

        auto operator+=(difference_type diff) -> VoxelDataIterator&
        {
            m_index += diff;
            update_value();
            return *this;
        }

        auto operator-=(difference_type diff) -> VoxelDataIterator&
        {
            m_index -= diff;
            update_value();
            return *this;
        }

        auto operator*() const -> value_type { return m_value; }
        auto operator->() -> pointer { return &m_value; }

        auto operator==(const VoxelDataIterator& other) const -> bool
        {
            return m_grid == other.m_grid && m_index == other.m_index;
        }

        auto operator!=(const VoxelDataIterator& other) const -> bool { return !(*this == other); }

        auto operator<(const VoxelDataIterator& other) const -> bool
        {
            return m_index < other.m_index;
        }
        auto operator>(const VoxelDataIterator& other) const -> bool
        {
            return m_index > other.m_index;
        }

        auto operator<=(const VoxelDataIterator& other) const -> bool { return !(*this > other); }
        auto operator>=(const VoxelDataIterator& other) const -> bool { return !(*this < other); }

        auto operator[](difference_type diff) const -> value_type { return *(*this + diff); }

      private:
        size_t     m_index;
        GridPtrT   m_grid;
        value_type m_value;

        VoxelDataIterator(GridPtrT grid, size_t idx)
            : m_index{idx}
            , m_grid{grid}
            , m_value{}
        {
            update_value();
        }

        auto update_value() -> void
        {
            if (m_index >= m_grid->m_size || m_index < 0)
            {
                m_value = value_type{};
                return;
            }

            m_value = value_type{
                m_grid->m_xs.begin() + m_index,
                m_grid->m_ys.begin() + m_index,
                m_grid->m_zs.begin() + m_index,
                m_grid->m_counts.begin() + m_index,
                m_grid->m_hits.begin() + m_index,
                m_grid->m_pads.begin() + m_index,
                m_grid->m_lengths.begin() + m_index,
                m_grid->m_hits_lengths.begin() + m_index,
                {}
            };

            if (m_grid->m_uses_variance)
            {
                m_value.lengths_variance = m_grid->m_lengths_variance.begin() + m_index;
            }
        }
    };

    using iterator       = VoxelDataIterator<VoxelData, COOGrid*>;
    using const_iterator = VoxelDataIterator<ConstVoxelData, const COOGrid*>;

    static_assert(std::random_access_iterator<iterator>);
    static_assert(std::random_access_iterator<const_iterator>);

    auto begin() -> iterator { return iterator{this, 0}; }
    auto end() -> iterator { return iterator{this, m_size}; }
    auto cbegin() const -> const_iterator { return const_iterator{this, 0}; }
    auto cend() const -> const_iterator { return const_iterator{this, m_size}; }

    auto size() const -> size_t { return m_size; }

    // These are returned by values to do move operations, hopefully
    // the compiler applies RVO
    auto xs() const -> std::vector<unsigned int> { return m_xs; }
    auto ys() const -> std::vector<unsigned int> { return m_ys; }
    auto zs() const -> std::vector<unsigned int> { return m_zs; }
    auto counts() const -> std::vector<unsigned int> { return m_counts; }
    auto hits() const -> std::vector<unsigned int> { return m_hits; }
    auto pads() const -> std::vector<double> { return m_pads; }
    auto lengths() const -> std::vector<double> { return m_lengths; }
    auto hits_lengths() const -> std::vector<double> { return m_hits_lengths; }
    auto lengths_variance() const -> std::vector<double> { return m_lengths_variance; }
    auto bounds() const -> lvox::BoundedGrid { return m_bounded_grid; }

  private:
    bool                      m_uses_variance;
    size_t                    m_size;
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
};

} // namespace lvox

#endif // LVOX_VOXEL_COO_GRID_HPP
