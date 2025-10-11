#include <H5Cpp.h>
#include <atomic>
#include <format>
#include <iterator>
#include <mutex>
#include <ranges>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/chunked_grid.hpp>

namespace lvox
{

ChunkedGrid::ChunkedGrid(const Bounds& bounds, double cell_size, bool compute_variance)
    : m_bounded_grid{bounds, cell_size, VoxelChunk::s_edge_size}
    , m_compute_variance{compute_variance}
    , m_chunks_x{static_cast<unsigned int>(
          std::ceil(static_cast<float>(m_bounded_grid.m_dim_x) / VoxelChunk::s_edge_size)
      )}
    , m_chunks_y{static_cast<unsigned int>(
          std::ceil(static_cast<float>(m_bounded_grid.m_dim_y) / VoxelChunk::s_edge_size)
      )}
    , m_chunks_z{static_cast<unsigned int>(
          std::ceil(static_cast<float>(m_bounded_grid.m_dim_z) / VoxelChunk::s_edge_size)
      )}
    , m_chunk_count{m_chunks_x * m_chunks_y * m_chunks_z}
    , m_chunks{m_chunk_count}
{
}

ChunkedGrid::ChunkedGrid(ChunkedGrid&& other)
    : m_bounded_grid{std::move(other.m_bounded_grid)}
    , m_chunks_x{std::move(other.m_chunks_x)}
    , m_chunks_y{std::move(other.m_chunks_y)}
    , m_chunks_z{std::move(other.m_chunks_z)}
    , m_chunk_count{std::move(other.m_chunk_count)}
    , m_chunks{std::move(other.m_chunks)}
{
}

ChunkedGrid::VoxelChunk::VoxelChunk(bool compute_variance)
    : m_hits{s_cell_count, std::allocator<unsigned int>{}}
    , m_counts{s_cell_count, std::allocator<unsigned int>{}}
    , m_lengths{s_cell_count, std::allocator<double>{}}
    , m_hits_lengths{s_cell_count, std::allocator<double>{}}
    , m_lengths_variance{std::invoke([this, compute_variance]() -> std::vector<double> {
        if (compute_variance)
            return std::vector<double>{s_cell_count, std::allocator<double>{}};
        return {};
    })}
    , m_write_access{}
{
}

auto ChunkedGrid::get_or_create_chunk(size_t chunk_idx) -> chunk_ptr
{
    auto& chunk_ref      = m_chunks[chunk_idx];
    auto  existing_chunk = chunk_ref.load(std::memory_order_acquire);

    if (existing_chunk)
        return existing_chunk;

    auto new_chunk = std::make_shared<VoxelChunk>(m_compute_variance);

    if (chunk_ref.compare_exchange_strong(
            existing_chunk, new_chunk, std::memory_order_release, std::memory_order_acquire
        ))
        return new_chunk;

    return existing_chunk;
}

auto ChunkedGrid::index3d_to_chunk_idx(const Index3D& voxel_idx) const -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    const auto chunk_x_idx = x >> VoxelChunk::s_edge_bits;
    const auto chunk_y_idx = y >> VoxelChunk::s_edge_bits;
    const auto chunk_z_idx = z >> VoxelChunk::s_edge_bits;

    return chunk_x_idx + chunk_y_idx * m_chunks_x + chunk_z_idx * m_chunks_x * m_chunks_y;
}

auto ChunkedGrid::VoxelChunk::index3d_to_flat_idx(const Index3D& voxel_idx) -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    const auto local_x_idx = x & VoxelChunk::s_edge_mask;
    const auto local_y_idx = y & VoxelChunk::s_edge_mask;
    const auto local_z_idx = z & VoxelChunk::s_edge_mask;

    return local_x_idx + local_y_idx * s_edge_size + local_z_idx * s_edge_size * s_edge_size;
}

auto ChunkedGrid::register_hit(const Index3D& idx) -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto       chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk->index3d_to_flat_idx(idx);

    std::lock_guard lock{chunk->m_write_access};
    chunk->m_hits[voxel_idx_in_chunk] += 1;
}

auto ChunkedGrid::add_length_and_count(const Index3D& idx, double length, bool is_hit) -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto       chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk->index3d_to_flat_idx(idx);

    std::lock_guard lock{chunk->m_write_access};
    chunk->m_lengths[voxel_idx_in_chunk] += length;

    if (is_hit)
        chunk->m_hits_lengths[voxel_idx_in_chunk] += length;

    chunk->m_counts[voxel_idx_in_chunk] += 1;
}

auto ChunkedGrid::add_length_count_and_variance(const Index3D& idx, double length, bool is_hit)
    -> void
{
    auto       chunk_idx          = index3d_to_chunk_idx(idx);
    auto       chunk              = get_or_create_chunk(chunk_idx);
    const auto voxel_idx_in_chunk = chunk->index3d_to_flat_idx(idx);

    std::unique_lock lock{chunk->m_write_access};
    const double     previous_lengths = chunk->m_lengths[voxel_idx_in_chunk];
    const double     previous_counts  = chunk->m_counts[voxel_idx_in_chunk];
    chunk->m_lengths[voxel_idx_in_chunk] += length;
    chunk->m_counts[voxel_idx_in_chunk] += 1;
    lock.unlock();

    // No variance possible if the count is not big enough
    if (previous_counts < 2)
        return;

    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
    const double previous_mean = previous_lengths / previous_counts;
    const double delta         = length - previous_mean;
    const double new_count     = previous_counts + 1;
    const double new_mean      = previous_mean + (delta / new_count);
    const double delta_2       = length - new_mean;

    lock.lock();

    // -1 on the count here because we are computing a variance sample.
    chunk->m_lengths_variance[voxel_idx_in_chunk] += (delta * delta_2) / (new_count - 1);
}

} // namespace lvox
