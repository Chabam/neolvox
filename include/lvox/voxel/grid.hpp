#ifndef LVOX_VOXEL_GRID_HPP
#define LVOX_VOXEL_GRID_HPP

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
    auto index_of_point(const Point& point) const -> Index3D;

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
    double       m_cell_size;
    unsigned int m_dim_x;
    unsigned int m_dim_y;
    unsigned int m_dim_z;
    size_t       m_cell_count;
    Bounds       m_bounds;

    std::vector<std::atomic<unsigned int>>          m_hits;
    std::vector<std::atomic<unsigned int>>          m_counts;
    std::vector<std::atomic<double>>                m_lengths;
    std::optional<std::vector<std::atomic<double>>> m_lengths_variances;
    std::vector<double>                             m_pad;

    static auto adjust_dim_to_grid(double distance, double cell_size) -> unsigned int;
    auto        adjust_bounds_to_grid(size_t dim, double min) const -> double;
    auto        index3d_to_flat_index(const Index3D& idx) const -> size_t;
    auto        flat_index_to_index3d(size_t idx) const -> Index3D;

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
