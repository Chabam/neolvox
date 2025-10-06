#ifndef LVOX_VOXEL_CHUNKED_GRID_HPP
#define LVOX_VOXEL_CHUNKED_GRID_HPP

#include <atomic>

#include <lvox/types.hpp>
#include <lvox/voxel/bounded_grid.hpp>

namespace lvox
{

class ChunkedGrid
{
    friend class COOGrid;
  public:
    ChunkedGrid(const Bounds& bounds, double cell_size, bool compute_variance);
    ChunkedGrid(ChunkedGrid&& other);

    auto register_hit(const Index3D& idx) -> void;
    auto add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit) -> void;
    auto add_length_count_and_variance(const Index3D& idx, double length, bool is_hit) -> void;

    auto bounded_grid() const -> const BoundedGrid& { return m_bounded_grid; }

  private:
    struct VoxelChunk
    {
        static constexpr auto s_edge_size = 8;
        static constexpr auto s_edge_bits = 3; // Number of bits for the edge size (log base 2 of 8)
        static constexpr auto s_edge_mask = s_edge_size - 1; // Inverting the size
        static constexpr auto s_cell_count = s_edge_size * s_edge_size * s_edge_size;

        VoxelChunk(bool compute_variance);

        static auto index3d_to_flat_idx(const Index3D& voxel_idx) -> size_t;

        std::vector<unsigned int> m_hits;
        std::vector<unsigned int> m_counts;
        std::vector<double>       m_lengths;
        std::vector<double>       m_hits_lengths;
        std::vector<double>       m_lengths_variance;
        std::mutex                m_write_access;
    };

    using chunk_ptr   = std::shared_ptr<VoxelChunk>;
    using a_chunk_ptr = std::atomic<chunk_ptr>;

    BoundedGrid              m_bounded_grid;
    bool                     m_compute_variance;
    unsigned int             m_chunks_x;
    unsigned int             m_chunks_y;
    unsigned int             m_chunks_z;
    size_t                   m_chunk_count;
    std::vector<a_chunk_ptr> m_chunks;

    auto get_or_create_chunk(size_t chunk_idx) -> chunk_ptr;
    // Returns a pair of the chunk index in the index of the voxel in the chunk
    auto index3d_to_chunk_idx(const Index3D& voxel_idx) const -> size_t;
};

} // namespace lvox

#endif // LVOX_VOXEL_CHUNKED_GRID_HPP
