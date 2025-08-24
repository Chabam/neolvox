#ifndef LVOX_VOXEL_HPP
#define LVOX_VOXEL_HPP

#include <atomic>
#include <vector>
#include <lvox/types.hpp>

namespace lvox
{

class VoxelChunk
{
  public:
    using a_u32 = std::atomic_uint32_t;
    using a_dbl = std::atomic<double>;
    static constexpr auto s_chunk_size = 256;
    static constexpr auto s_cell_count = s_chunk_size * s_chunk_size * s_chunk_size;

    VoxelChunk();

    auto register_hit(const Index3D& idx) -> void;

  private:
    static auto index3d_to_flat_index(const Index3D& idx) -> size_t;
    std::vector<a_u32> m_hits;
    std::vector<a_u32> m_counts;
    std::vector<a_dbl> m_lengths;
    std::vector<a_dbl> m_lengths_variances;
    std::vector<a_dbl> m_pad;
};

}

#endif // LVOX_VOXEL_HPP
