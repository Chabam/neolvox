#ifndef LVOX_VISITED_VOXELS_HPP
#define LVOX_VISITED_VOXELS_HPP

#include <vector>

#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{
template <typename T>
class VisitedVoxels
{
  public:
    using value_type = T;
    using value_ref  = value_type&;
    using value_pair = std::pair<Index3D, value_type>;
    using container  = std::vector<value_pair>;
    using iterator   = container::iterator;
    using const_iterator   = container::const_iterator;

    VisitedVoxels(const Grid& grid)
        : m_voxels{}
        , m_grid{grid}
    {
    }

    auto reserve(size_t amount) -> void
    {
        m_voxels.resize(amount);
    }

    [[nodiscard]]
    auto try_insert(const value_pair val) -> std::pair<value_ref, bool>
    {
        return try_insert(val.first, val.second);
    }

    [[nodiscard]]
    auto try_insert(const Index3D& idx, const T& value) -> std::pair<value_ref, bool>
    {
        auto lower = std::lower_bound(
            m_voxels.begin(),
            m_voxels.end(),
            idx,
            [this](const auto& lhs, const auto& rhs) -> bool {
                return index3d_to_flat_index(lhs.first) < index3d_to_flat_index(rhs);
            }
        );

        if (lower == m_voxels.end())
        {
            m_voxels.emplace_back(idx, value);
            return {m_voxels.rbegin()->second, true};
        }

        return {lower->second, false};
    }

    auto begin() -> iterator { return m_voxels.begin(); }
    auto end() -> iterator { return m_voxels.end(); }
    auto cbegin() const -> const_iterator { return m_voxels.cbegin(); }
    auto cend() const -> const_iterator { return m_voxels.cend(); }
    auto begin() const -> const_iterator { return m_voxels.begin(); }
    auto end() const -> const_iterator { return m_voxels.end(); }

  private:
    std::vector<std::pair<Index3D, T>> m_voxels;
    Grid                               m_grid;

    auto index3d_to_flat_index(const size_t3D& idx) const -> size_t
    {
        const auto [x, y, z] = idx;
        return x + y * m_grid.dim_x() + z * m_grid.dim_x() * m_grid.dim_y();
    };
};

} // namespace lvox
#endif // LVOX_VISITED_VOXELS_HPP
