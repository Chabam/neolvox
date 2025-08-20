#include <future>
#include <iterator>
#include <limits>
#include <ranges>
#include <variant>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/grid_traversal.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/voxel/voxels_metrics.hpp>

namespace lvox::algorithms
{


template <std::ranges::range R, typename F, typename VisitedVoxelsT>
auto parallel_compute_then_merge(
    const R& objects, unsigned int job_count, const F& func, VisitedVoxelsT& out
) -> void
{
    const size_t object_count     = std::distance(std::begin(objects), std::end(objects));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / job_count);

    std::vector<std::future<VisitedVoxelsT>> all_res;
    for (auto chunk : objects | std::views::chunk(objects_per_core))
    {
        all_res.emplace_back(std::async(func, std::move(chunk)));
    }

    for (auto&& res : std::move(all_res))
        out.merge(res.get());
}

// auto compute_lengths_variance(
//     double              length_in_voxel,
//     LengthGrid::cell_t& variance_ref,
//     double              previous_lengths,
//     unsigned int        previous_count
// ) -> void
// {
//     // TODO: make this configurable maybe?
//     // So far it's based on Computree's NeedleFromDimension
//     constexpr double elem_length       = 0.06;
//     constexpr double elem_diameter     = 0.02;
//     constexpr double attenuation_coeff = (2. * std::numbers::pi * elem_length * elem_diameter)
//     / 4.;

//     const double attenuated_length =
//         -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

//     // No variance possible if the count is not big enough
//     if (previous_count < 2)
//         return;

//     //
//     https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
//     const double previous_mean = previous_lengths / previous_count;
//     const double delta         = length_in_voxel - previous_mean;
//     const double new_count     = previous_count + 1;
//     const double new_mean      = previous_mean + (delta / new_count);
//     const double delta_2       = length_in_voxel - new_mean;

//     // -1 on the count here because we are computing a variance sample.
//     variance_ref.fetch_add((delta * delta_2) / (new_count - 1), std::memory_order_relaxed);
// }

template <bool limit_ray_length, bool compute_hits, typename PadEstimator>
auto compute_rays_count_and_length_impl(
    const Grid& grid, const Scan& scan, const ComputeOptions& options, Logger logger
) -> VoxelsMetrics
{
    auto grid_traversal = std::invoke([&grid]() {
        if constexpr (pe::is_uplbl<PadEstimator>::value)
            return GridTraversalExactDistance{grid};
        else
            return GridTraversalVoxelRounding{grid};
    });

    const auto ray_trace = [&](auto&& points) -> VoxelsMetrics {
        VoxelsMetrics visited_voxels{grid};
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            const Vector beam_to_point{pt - scan_origin};

            double max_distance = std::numeric_limits<double>::infinity();

            if constexpr (compute_hits)
            {
                const auto idx = grid.index_of_point(timed_point.m_point);
                visited_voxels.register_hit(idx);
            }

            if constexpr (limit_ray_length)
            {
                max_distance = beam_to_point.norm();
            }

            const auto traced_voxels =
                grid_traversal(Beam{scan_origin, beam_to_point}, max_distance);

            for (const VoxelHitInfo& voxel : traced_voxels)
            {
                if constexpr (pe::is_uplbl<PadEstimator>::value)
                    visited_voxels.add_length_count_and_variance(voxel.m_index, voxel.m_distance_in_voxel);
                else
                    visited_voxels.add_length_and_count(voxel.m_index, voxel.m_distance_in_voxel);
            }
        }

        logger.debug("thread finished");
        return visited_voxels;
    };

    VoxelsMetrics counts_and_lengths{grid};
    parallel_compute_then_merge(*scan.m_points, options.m_job_limit, ray_trace, counts_and_lengths);
    return counts_and_lengths;
}

auto compute_rays_count_and_length(
    const Grid& grid, const Scan& scan, const ComputeOptions& options
) -> VoxelsMetrics
{
    constexpr bool limit_ray_length = true;
    constexpr bool compute_hits = true;
    Logger         logger{"Compute Rays count and length"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VoxelsMetrics {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_theoriticals(const Grid& grid, const Scan& scan, const ComputeOptions& options)
    -> VoxelsMetrics
{
    constexpr bool limit_ray_length = false;
    constexpr bool compute_hits = false;
    Logger         logger{"Compute theoriticals"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VoxelsMetrics {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> VoxelsMetrics
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Grid grid{compute_scene_bounds(scans), options.m_voxel_size};

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    VoxelsMetrics pad_result{grid};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {
        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(grid, scan, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());
        VoxelsMetrics voxels = compute_rays_count_and_length(grid, scan, options);

        std::visit(
            [&voxels](auto&& chosen_estimator) -> void {
                voxels.compute_pad(chosen_estimator);
            },
            options.m_pad_estimator
        );

        pad_result.merge(std::move(voxels));

    }

    return pad_result;
}

auto compute_scene_bounds(const std::vector<lvox::Scan>& scans) -> lvox::Bounds
{
    Bounds total_bounds;

    for (const auto& scan : scans)
        total_bounds.grow(scan.m_bounds);

    return total_bounds;
}

} // namespace lvox::algorithms
