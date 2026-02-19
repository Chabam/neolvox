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

    struct VoxelData
    {
        std::vector<int>::iterator          x;
        std::vector<int>::iterator          y;
        std::vector<int>::iterator          z;
        std::vector<unsigned int>::iterator count;
        std::vector<unsigned int>::iterator hit;
        std::vector<double>::iterator       pad;
        std::vector<double>::iterator       lengths;
        std::vector<double>::iterator       hits_length;
        std::vector<double>::iterator       lengths_variance;
    };

    struct ConstVoxelData
    {
        std::vector<int>::const_iterator          x;
        std::vector<int>::const_iterator          y;
        std::vector<int>::const_iterator          z;
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

        VoxelDataIterator()                         = default;
        VoxelDataIterator(const VoxelDataIterator&) = default;

        VoxelDataIterator& operator=(const VoxelDataIterator&) = default;

        VoxelDataIterator& operator++()
        {
            *this += 1;
            return *this;
        }

        VoxelDataIterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        VoxelDataIterator& operator--()
        {
            *this -= 1;
            return *this;
        }

        VoxelDataIterator operator--(int)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        VoxelDataIterator operator+(difference_type diff) const
        {
            auto tmp = *this;
            tmp.m_index += diff;
            tmp.update_value();
            return tmp;
        }

        friend VoxelDataIterator operator+(difference_type other, const VoxelDataIterator& diff)
        {
            return other + diff;
        }

        VoxelDataIterator operator-(difference_type diff) const
        {
            auto tmp = *this - diff;
            tmp.m_index -= diff;
            tmp.update_value();
            return tmp;
        }

        difference_type operator-(const VoxelDataIterator& other) const
        {
            return m_index - other.m_index;
        }

        VoxelDataIterator& operator+=(difference_type diff)
        {
            m_index += diff;
            update_value();
            return *this;
        }

        VoxelDataIterator& operator-=(difference_type diff)
        {
            m_index -= diff;
            update_value();
            return *this;
        }

        value_type operator*() const { return m_value; }
        pointer    operator->() { return &m_value; }

        bool operator==(const VoxelDataIterator& other) const
        {
            return m_grid == other.m_grid && m_index == other.m_index;
        }

        bool operator!=(const VoxelDataIterator& other) const { return !(*this == other); }

        bool operator<(const VoxelDataIterator& other) const { return m_index < other.m_index; }
        bool operator>(const VoxelDataIterator& other) const { return m_index > other.m_index; }

        bool operator<=(const VoxelDataIterator& other) const { return !(*this > other); }
        bool operator>=(const VoxelDataIterator& other) const { return !(*this < other); }

        value_type operator[](difference_type diff) const { return *(*this + diff); }

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

        void update_value()
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

    iterator       begin() { return iterator{this, 0}; }
    iterator       end() { return iterator{this, m_size}; }
    const_iterator cbegin() const { return const_iterator{this, 0}; }
    const_iterator cend() const { return const_iterator{this, m_size}; }

    size_t size() const { return m_size; }

    const std::vector<int>&          xs() const { return m_xs; }
    const std::vector<int>&          ys() const { return m_ys; }
    const std::vector<int>&          zs() const { return m_zs; }
    const std::vector<unsigned int>& counts() const { return m_counts; }
    const std::vector<unsigned int>& hits() const { return m_hits; }
    const std::vector<double>&       pads() const { return m_pads; }
    const std::vector<double>&       lengths() const { return m_lengths; }
    const std::vector<double>&       hits_lengths() const { return m_hits_lengths; }
    const std::vector<double>&       lengths_variance() const { return m_lengths_variance; }
    const lvox::BoundedGrid&         bounds() const { return m_bounded_grid; }

  private:
    bool                      m_uses_variance;
    size_t                    m_size;
    std::vector<int>          m_xs;
    std::vector<int>          m_ys;
    std::vector<int>          m_zs;
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
