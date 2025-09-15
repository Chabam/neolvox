#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

#include <atomic>
#include <execution>
#include <filesystem>

#include <lvox/types.hpp>

namespace lvox
{

namespace algorithms::pad_estimators
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
} // namespace algorithms::pad_estimators

class Grid
{
  public:
    Grid(const Bounds& bounds, double cell_size, bool compute_variance = false);
    Grid(Grid&& other);

    auto register_hit(const Index3D& idx) -> void;
    auto add_length_and_count(const Index3D& idx, double length) -> void;
    auto add_length_count_and_variance(const Index3D& idx, double length) -> void;

    auto compute_pad(algorithms::pad_estimators::BeerLambert) -> void;
    auto compute_pad(algorithms::pad_estimators::ContactFrequency) -> void;
    auto compute_pad(algorithms::pad_estimators::UnequalPathLengthBeerLambert) -> void;

    // NOTE: no bounds check!
    auto voxel_bounds(size_t idx_x, size_t idx_y, size_t idx_z) const -> Bounds;

    auto voxel_bounds_from_point(const Point& point) -> Bounds;

    // Return an index tuple of this layout (x, y, z)
    auto index3d_of_point(const Point& point) const -> Index3D;

    auto cell_size() const -> double { return m_cell_size; }
    auto cell_count() const -> size_t { return m_cell_count; }

    auto dim_x() const -> unsigned int { return m_dim_x; }
    auto dim_y() const -> unsigned int { return m_dim_y; }
    auto dim_z() const -> unsigned int { return m_dim_z; }

    auto bounds() const -> const Bounds& { return m_bounds; }
    auto export_as_coo_to_h5(
        const std::string&           dataset_name,
        const std::filesystem::path& filename,
        bool                         include_all_data = false
    ) const -> void;

  private:
    struct VoxelChunk
    {
        static constexpr auto s_edge_size = 8;
        static constexpr auto s_edge_bits = 3; // Number of bits for the edge size (log base 2 of 8)
        static constexpr auto s_edge_mask = s_edge_size - 1; // Inverting the size
        static constexpr auto s_cell_count = s_edge_size * s_edge_size * s_edge_size;

        VoxelChunk(bool compute_variance);

        static auto index3d_to_flat_idx(const Index3D& voxel_idx) -> size_t;

        std::vector<std::atomic_uint> m_hits;
        std::vector<std::atomic_uint> m_counts;
        std::vector<std::atomic<double>>       m_lengths;
        std::vector<std::atomic<double>>       m_lengths_variance;
        std::vector<std::atomic<double>>       m_pad;
    };

    using chunk_ptr   = std::shared_ptr<VoxelChunk>;
    using a_chunk_ptr = std::atomic<chunk_ptr>;

    double                   m_cell_size;
    unsigned int             m_dim_x;
    unsigned int             m_dim_y;
    unsigned int             m_dim_z;
    unsigned int             m_chunks_x;
    unsigned int             m_chunks_y;
    unsigned int             m_chunks_z;
    size_t                   m_cell_count;
    size_t                   m_chunk_count;
    bool                     m_compute_variance;
    std::vector<a_chunk_ptr> m_chunks;
    Bounds                   m_bounds;

    template <typename PadFunc>
    auto compute_pad_impl(PadFunc&& func) -> void
    {
        std::for_each(
            std::execution::par_unseq, m_chunks.begin(), m_chunks.end(), [func](auto& a_chunk) {
                auto chunk = a_chunk.load(std::memory_order_relaxed);

                if (!chunk)
                    return;

                for (unsigned int i = 0; i < VoxelChunk::s_cell_count; ++i)
                {
                    // TODO: Make the treshold configurable
                    if (chunk->m_counts[i] < 5)
                    {
                        chunk->m_pad[i] = 0;
                        continue;
                    }

                    chunk->m_pad[i] = func(chunk, i);
                }
            }
        );
    }

    auto get_or_create_chunk(size_t chunk_idx) -> chunk_ptr;
    // Returns a pair of the chunk index in the index of the voxel in the chunk
    auto index3d_to_chunk_idx(const Index3D& voxel_idx) const -> size_t;

    auto adjust_dim_to_grid(double distance) -> unsigned int;
    auto adjust_bounds_to_grid(size_t dim, double min) const -> double;

    static constexpr auto g_grid_loginfo = R"(
Creating grid of dimension: {}x{}x{}
Voxel size: {}
Voxel count: {}
Bounds:
    x: {}, {}
    y: {}, {}
    z: {}, {}
    )";
};

} // namespace lvox

#endif // LVOX_VOXEL_GRID_HPP
