#include <atomic>

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
    , m_lengths_variance{std::invoke([&]() -> std::vector<atomic_wa_ptr> {
        if (compute_variance)
            return std::vector<atomic_wa_ptr>{
                m_bounded_grid.m_cell_count, std::allocator<atomic_wa_ptr>{}
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

size_t DenseGrid::index3d_to_flat_idx(const Index3D& voxel_idx) const
{
    const auto& [x, y, z] = voxel_idx;

    return x + y * m_bounded_grid.dim_x() + z * m_bounded_grid.dim_x() * m_bounded_grid.dim_y();
}

void DenseGrid::register_hit(const Index3D& voxel_idx)
{
    auto idx = index3d_to_flat_idx(voxel_idx);
    m_hits[idx].fetch_add(1, std::memory_order_relaxed);
}

void DenseGrid::add_length_and_count(const Index3D& voxel_idx, double length, bool is_hit)
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    m_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    if (is_hit)
        m_hits_lengths[idx].fetch_add(length, std::memory_order_relaxed);

    m_counts[idx].fetch_add(1, std::memory_order_relaxed);
}

void DenseGrid::add_length_count_and_variance(const Index3D& voxel_idx, double length, bool is_hit)
{
    auto idx = index3d_to_flat_idx(voxel_idx);

    add_length_and_count(voxel_idx, length, is_hit);

    auto   existing_aggregate = m_lengths_variance[idx].load(std::memory_order_acquire);
    wa_ptr aggregate;
    do
    {
        // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
        if (existing_aggregate)
        {
            aggregate          = std::make_shared<WelfordAggregate>(*existing_aggregate);
            const double delta = length - aggregate->m_mean;
            aggregate->m_count += 1;
            aggregate->m_mean += delta / aggregate->m_count;
            const double delta_2 = length - aggregate->m_mean;
            aggregate->m_m2 += delta * delta_2;
        }
        else
        {
            aggregate = std::make_shared<WelfordAggregate>(
                1,      // count
                length, // mean
                0       // M2
            );
        }

    } while (m_lengths_variance[idx].compare_exchange_weak(
        existing_aggregate, aggregate, std::memory_order_release, std::memory_order_acquire
    ));
}

} // namespace lvox
