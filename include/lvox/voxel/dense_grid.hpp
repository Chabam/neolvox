#ifndef LVOX_DENSE_GRID_HPP
#define LVOX_DENSE_GRID_HPP

#include <atomic>

#include <lvox/types.hpp>
#include <lvox/voxel/bounded_grid.hpp>

namespace lvox
{

class DenseGrid
{
    friend class COOGrid;
  public:
    DenseGrid(const Bounds& bounds, double cell_size, bool compute_variance);
    DenseGrid(DenseGrid&& other);

    auto register_hit(const Index3D& voxel_idx) -> void;
    auto add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit) -> void;
    auto add_length_count_and_variance(const Index3D& voxel_idx, double length, bool is_hit) -> void;

    auto bounded_grid() const -> const BoundedGrid& { return m_bounded_grid; }

  private:
    BoundedGrid m_bounded_grid;
    using atomic_f64 = std::atomic<double>;
    std::vector<std::atomic_uint> m_hits;
    std::vector<std::atomic_uint> m_counts;
    std::vector<atomic_f64>       m_lengths;
    std::vector<atomic_f64>       m_hits_lengths;
    std::vector<atomic_f64>       m_lengths_variance;
    std::vector<atomic_f64>       m_pad;

    auto index3d_to_flat_idx(const Index3D& voxel_idx) const -> size_t;
};

} // namespace lvox

#endif // LVOX_DENSE_GRID_HPP
