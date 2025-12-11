#ifndef LVOX_EXPLORE_GRID_HPP
#define LVOX_EXPLORE_GRID_HPP

#include <future>
#include <numbers>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/trace_beam.hpp>
#include <lvox/logger/progress.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

namespace algorithms
{

namespace pe = lvox::algorithms::pad_estimators;

template <
    typename ScanT,
    Point              PointT,
    PointCloud<PointT> PointCloudT,
    bool               limit_ray_length,
    bool               compute_hits,
    typename PadEstimator>
void explore_grid_impl(Grid& grid, const ScanT& scan, const ComputeOptions& options, Logger logger)
{
    auto grid_traversal = std::visit(
        [](const auto& grid) {
            if constexpr (pe::estimator_uses_effective_lengths<PadEstimator>::value)
                return TraceBeamExactDistance{grid.bounded_grid()};
            else
                return TraceBeamVoxelRounding{grid.bounded_grid()};
        },
        grid
    );

    const auto points_to_process = std::ranges::distance(scan.m_points);
    Progress   progress{static_cast<size_t>(points_to_process), options.m_log_stream};

    struct PointRange
    {
        using const_iterator = PointCloudT::const_iterator;

        const_iterator m_start;
        const_iterator m_end;

        const_iterator begin() const { return m_start; }
        const_iterator end() const { return m_end; }
    };

    const auto ray_trace = [&](const PointRange& points) -> void {
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.gps_time();
            const Vector pt{timed_point.x(), timed_point.y(), timed_point.z()};
            using ComputeBeamOrigin = ScanT::ComputeBeamOrigin;
            const Vector scan_origin =
                std::visit(ComputeBeamOrigin{gps_time}, scan.m_scanner_origin);

            // If we compute the hits, we start at the point in the
            // point cloud towards the scanner. This is done to avoid
            // recomputing the index of the point since it is provided
            // by the grid traversal.
            const Vector beam_dir = std::invoke([&scan_origin, &pt]() {
                if constexpr (compute_hits)
                    return Vector{scan_origin - pt};
                else
                    return Vector{pt - scan_origin};
            });

            // Invertly, if we do not compute the hits, we start at
            // the scanner towards the point.
            const Vector point_origin = std::invoke([&scan_origin, &pt]() {
                if constexpr (compute_hits)
                    return pt;
                else
                    return scan_origin;
            });

            double max_distance = std::numeric_limits<double>::infinity();

            if constexpr (limit_ray_length)
            {
                max_distance = beam_dir.norm();
            }

            bool is_hit_computed = false;

            grid_traversal(
                Beam{point_origin, beam_dir},
                [&grid, &is_hit_computed](const VoxelHitInfo& hit) {
                    if constexpr (pe::estimator_uses_effective_lengths<PadEstimator>::value)
                        std::visit(
                            [&hit, &is_hit_computed](auto& grid) {
                                // TODO: make this configurable maybe, and
                                // based on a specific PAD estimation method So far it's based on
                                // Computree's NeedleFromDimension
                                constexpr double elem_length   = 0.06;
                                constexpr double elem_diameter = 0.02;
                                constexpr double projected_area_of_single_element =
                                    (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;
                                const double voxel_size = grid.bounded_grid().cell_size();
                                const double attenuation_coeff =
                                    projected_area_of_single_element /
                                    (voxel_size * voxel_size * voxel_size);

                                const double attenuated_length =
                                    -(std::log(1. - attenuation_coeff * hit.m_distance_in_voxel) /
                                      attenuation_coeff);

                                if constexpr (pe::is_uplbl<PadEstimator>::value)
                                    grid.add_length_count_and_variance(
                                        hit.m_index, attenuated_length, !is_hit_computed
                                    );
                                else
                                    grid.add_length_and_count(
                                        hit.m_index, attenuated_length, !is_hit_computed
                                    );
                            },
                            grid
                        );
                    else
                        std::visit(
                            [&hit, &is_hit_computed](auto& grid) {
                                grid.add_length_and_count(
                                    hit.m_index, hit.m_distance_in_voxel, !is_hit_computed
                                );
                            },
                            grid
                        );

                    if constexpr (compute_hits)
                    {
                        if (!is_hit_computed)
                        {
                            std::visit(
                                [&hit](auto& grid) {
                                    grid.register_hit(hit.m_index);
                                },
                                grid
                            );

                            is_hit_computed = true;
                        }
                    }
                },
                max_distance
            );
            progress.increase_progression_by(1);
        }
    };

    const size_t                   point_count      = scan.m_points.size();
    const size_t                   points_per_tasks = point_count / options.m_job_limit;
    std::vector<std::future<void>> threads;
    auto                           start_it       = scan.m_points.begin();
    auto                           current_thread = 0;
    for (auto i = 0; i < point_count; i += points_per_tasks, ++current_thread)
    {
        auto next_end = start_it + std::min(points_per_tasks, point_count - i);
        threads.push_back(
            std::async(std::launch::async, ray_trace, PointRange{start_it, next_end})
        );
        start_it = next_end;
    }

    while (!progress.completed())
    {
        if (std::all_of(threads.begin(), threads.end(), [](const std::future<void>& thread) {
                return thread.wait_for(std::chrono::seconds{0}) == std::future_status::ready;
            }))
            progress.stop();

        progress.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

template <Point PointT, PointCloud<PointT> PointCloudT>
void explore_grid(Grid& grid, const Scan<PointT, PointCloudT>& scan, const ComputeOptions& options)
{
    using ScanT                     = Scan<PointT, PointCloudT>;
    constexpr bool limit_ray_length = true;
    constexpr bool compute_hits     = true;
    Logger         logger{"Explore grid", options.m_log_stream};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            explore_grid_impl<ScanT, PointT, PointCloudT, limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

template <Point PointT, PointCloud<PointT> PointCloudT>
void explore_grid_theoriticals(
    Grid& grid, const Scan<PointT, PointCloudT>& scan, const ComputeOptions& options
)
{
    using ScanT                     = Scan<PointT, PointCloudT>;
    constexpr bool limit_ray_length = false;
    constexpr bool compute_hits     = false;
    Logger         logger{"Explore grid theoriticals", options.m_log_stream};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            explore_grid_impl<ScanT, PointT, PointCloudT, limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

} // namespace algorithms

} // namespace lvox

#endif // LVOX_EXPLORE_GRID_HPP
