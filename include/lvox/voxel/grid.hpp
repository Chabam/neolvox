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
        using a_u32                           = std::atomic_uint32_t;
        using a_dbl                           = std::atomic<double>;
        static constexpr auto s_max_edge_size = 8;
        static constexpr auto s_max_cell_count =
            s_max_edge_size * s_max_edge_size * s_max_edge_size;

        VoxelChunk(size_t starting_idx, unsigned int dim_x, unsigned int dim_y, unsigned int dim_z);

        size_t            m_starting_idx;
        unsigned int       m_dim_x;
        unsigned int       m_dim_y;
        unsigned int       m_dim_z;
        size_t             m_cell_count;
        std::vector<a_u32> m_hits;
        std::vector<a_u32> m_counts;
        std::vector<a_dbl> m_lengths;
        std::vector<a_dbl> m_lengths_variance;
        std::vector<a_dbl> m_pad;
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
    std::atomic_size_t       m_effective_chunk_count;
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

                for (unsigned int i = 0; i < chunk->m_cell_count; ++i)
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

    auto get_or_create_chunk(const Index3D& voxel_idx) -> a_chunk_ptr&;
    auto index3d_to_chunk_idx(const Index3D& voxel_idx) -> size_t;
    auto index3d_to_chunk_flat_idx(const a_chunk_ptr& chunk, const Index3D& voxel_idx) const
        -> unsigned int;
    auto flat_index_to_index3d(const chunk_ptr& chunk, size_t i) const -> Index3D;

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
