#include <lvox/voxel/voxel_chunk.hpp>

namespace lvox
{

VoxelChunk:: VoxelChunk()
    : m_hits{s_cell_count, std::allocator<a_u32>{}}
    , m_counts{s_cell_count, std::allocator<a_u32>{}}
    , m_lengths{s_cell_count, std::allocator<a_dbl>{}}
    , m_lengths_variances{s_cell_count, std::allocator<a_dbl>{}}
    , m_pad{s_cell_count, std::allocator<a_dbl>{}}
{
}

auto VoxelChunk::index3d_to_flat_index(const Index3D& idx) -> size_t
{

    // TODO: 😀
}

auto VoxelChunk::register_hit(const Index3D& idx) -> void
{
    const auto cur_idx = index3d_to_flat_index(idx);
    m_hits[cur_idx].fetch_add(1, std::memory_order_relaxed);
}


}
