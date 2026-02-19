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
    DenseGrid(const Bounds<double>& bounds, double cell_size, bool compute_variance);
    DenseGrid(DenseGrid&& other);

    void register_hit(const Index3D& voxel_idx);
    void add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit);
    void add_length_count_and_variance(const Index3D& voxel_idx, double length, bool is_hit);

    const BoundedGrid& bounded_grid() const { return m_bounded_grid; }

  private:
    struct WelfordAggregate
    {
        double m_count;
        double m_mean;
        double m_m2;
    };
    using wa_ptr        = std::shared_ptr<WelfordAggregate>;
    using atomic_wa_ptr = std::atomic<wa_ptr>;
    using atomic_f64    = std::atomic<double>;

    BoundedGrid                   m_bounded_grid;
    std::vector<std::atomic_uint> m_hits;
    std::vector<std::atomic_uint> m_counts;
    std::vector<atomic_f64>       m_lengths;
    std::vector<atomic_f64>       m_hits_lengths;
    std::vector<atomic_wa_ptr>    m_lengths_variance;
    std::vector<atomic_f64>       m_pad;

    size_t index3d_to_flat_idx(const Index3D& voxel_idx) const;
};

} // namespace lvox

#endif // LVOX_DENSE_GRID_HPP
