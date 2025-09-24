#include <H5Cpp.h>
#include <H5Fpublic.h>
#include <atomic>
#include <format>
#include <iterator>
#include <numbers>
#include <ranges>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/chunked_grid.hpp>
#include <lvox/voxel/dense_grid.hpp>

namespace lvox
{

DenseGrid::DenseGrid(const Bounds& bounds, double cell_size, bool compute_variance)
    : m_bounded_grid{bounds, cell_size}
    , m_hits{m_bounded_grid.m_cell_count, std::allocator<std::atomic_uint>{}}
    , m_counts{m_bounded_grid.m_cell_count, std::allocator<std::atomic_uint>{}}
    , m_lengths{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
    , m_hits_lengths{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
    , m_lengths_variance{std::invoke([&]() -> std::vector<atomic_f64> {
        if (compute_variance)
            return std::vector<atomic_f64>{
                m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}
            };
        else
            return {};
    })}
    , m_pad{m_bounded_grid.m_cell_count, std::allocator<atomic_f64>{}}
{
}

DenseGrid::DenseGrid(DenseGrid&& other)
    : m_bounded_grid{std::move(other.m_bounded_grid)}
    , m_hits{std::move(other.m_hits)}
    , m_counts{std::move(other.m_counts)}
    , m_lengths{std::move(other.m_lengths)}
    , m_hits_lengths{std::move(other.m_hits_lengths)}
    , m_lengths_variance{std::move(other.m_lengths_variance)}
    , m_pad{std::move(other.m_pad)}

{
}

auto DenseGrid::index3d_to_flat_idx(const Index3D& voxel_idx) const -> size_t
{
    const auto& [x, y, z] = voxel_idx;

    return x + y * m_bounded_grid.dim_x() + z * m_bounded_grid.dim_x() * m_bounded_grid.dim_y();
}

auto DenseGrid::register_hit(const Index3D& voxel_idx) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);
    m_hits[idx].fetch_add(1, std::memory_order_relaxed);
}

auto DenseGrid::add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    m_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    if (is_hit)
        m_hits_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    m_counts[idx].fetch_add(1, std::memory_order_relaxed);
}

// TODO: make this work with memory consistency
auto DenseGrid::add_length_count_and_variance(const Index3D& voxel_idx, double length, bool is_hit) -> void
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    const double previous_lengths = m_lengths[idx].fetch_add(length, std::memory_order_acq_rel);
    const double previous_counts  = m_counts[idx].fetch_add(1, std::memory_order_acq_rel);

    // No variance possible if the count is not big enough
    if (previous_counts < 2)
        return;

    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
    const double previous_mean = previous_lengths / previous_counts;
    const double delta         = length - previous_mean;
    const double new_count     = previous_counts + 1;
    const double new_mean      = previous_mean + (delta / new_count);
    const double delta_2       = length - new_mean;

    // -1 on the count here because we are computing a variance sample.
    m_lengths_variance[idx].fetch_add((delta * delta_2) / (new_count - 1), std::memory_order_acq_rel);
}

} // namespace lvox
