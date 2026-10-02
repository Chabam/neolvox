#ifndef LVOX_EXPLORE_GRID_HPP
#define LVOX_EXPLORE_GRID_HPP

#include <future>

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
    bool               use_classifications,
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
    double unit_attenuation_coeff = 0.0;

    if (options.m_smallest_element_area != 0)
    {
        const double voxel_size = std::visit(
            [](auto&& grid) {
                return grid.bounded_grid().cell_size();
            },
            grid
        );
        unit_attenuation_coeff =
            options.m_smallest_element_area / (voxel_size * voxel_size * voxel_size);
    }

    const auto ray_trace = [&](const PointRange& points) -> void {
        const bool compute_occlusion = options.m_compute_occlusion;
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.gps_time();
            const Vector pt{timed_point.x(), timed_point.y(), timed_point.z()};
            using ComputeBeamOrigin = ScanT::ComputeBeamOrigin;
            const Vector scan_origin =
                std::visit(ComputeBeamOrigin{gps_time}, scan.m_scanner_origin);

            bool compute_hit = true;

            if constexpr (use_classifications)
            {
                compute_hit = !timed_point.is_hit() || !timed_point.is_in_bounding_box();
            }

            // When computing the hit, we know that the point is in
            // the bounding box. Given that knowledge, we can take a
            // shortcut and start computing from the point to the
            // scanner. That way, the first voxel that intersects with
            // the ray is the one containing the hit.
            const Vector beam_dir = std::invoke([&scan_origin, &pt, compute_hit]() {
                if (compute_hit)
                    return Vector{scan_origin - pt};
                else
                    return Vector{pt - scan_origin};
            });

            const Vector point_origin = std::invoke([&scan_origin, &pt, compute_hit]() {
                if (compute_hit)
                    return pt;
                else
                    return scan_origin;
            });

            bool is_hit_computed = false;

            grid_traversal(
                Beam{point_origin, beam_dir},
                [&grid, unit_attenuation_coeff, compute_hit, &is_hit_computed, compute_occlusion](
                    const VoxelHitInfo& hit
                ) {
                    if constexpr (pe::estimator_uses_effective_lengths<PadEstimator>::value)
                    {
                        std::visit(
                            [&hit, unit_attenuation_coeff, compute_hit, is_hit_computed](
                                auto& grid
                            ) {
                                double effective_length = hit.m_distance_in_voxel;
                                if (unit_attenuation_coeff != 0.0)
                                    effective_length =
                                        -(std::log(
                                              1. - unit_attenuation_coeff * hit.m_distance_in_voxel
                                          ) /
                                          unit_attenuation_coeff);

                                if constexpr (pe::is_uplbl<PadEstimator>::value)
                                    grid.add_length_count_and_variance(
                                        hit.m_index,
                                        effective_length,
                                        compute_hit && !is_hit_computed
                                    );
                                else
                                    grid.add_length_and_count(
                                        hit.m_index,
                                        effective_length,
                                        compute_hit && !is_hit_computed
                                    );
                            },
                            grid
                        );
                    }
                    else
                    {
                        std::visit(
                            [&hit, is_hit_computed](auto& grid) {
                                grid.add_length_and_count(
                                    hit.m_index, hit.m_distance_in_voxel, !is_hit_computed
                                );
                            },
                            grid
                        );
                    }

                    if (compute_hit)
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
                beam_dir.norm()
            );

            if (compute_occlusion)
            {
                const Vector beam_to_point{pt - scan_origin};
                grid_traversal(
                    Beam{pt, beam_to_point},
                    [&grid](const VoxelHitInfo& hit) {
                        std::visit(
                            [&hit](auto&& grid) {
                                grid.add_potential_length_and_count(
                                    hit.m_index, hit.m_distance_in_voxel
                                );
                            },
                            grid
                        );
                    },
                    std::numeric_limits<double>::infinity()
                );
            }

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
    using ScanT = Scan<PointT, PointCloudT>;
    Logger logger{"Explore grid", options.m_log_stream};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            if (options.m_use_classification)
            {
                constexpr bool use_classification = true;
                explore_grid_impl<ScanT, PointT, PointCloudT, use_classification, T>(
                    grid, scan, options, logger
                );
            }
            else
            {
                constexpr bool use_classification = false;
                explore_grid_impl<ScanT, PointT, PointCloudT, use_classification, T>(
                    grid, scan, options, logger
                );
            }
        },
        options.m_pad_estimator
    );
}

} // namespace algorithms

} // namespace lvox

#endif // LVOX_EXPLORE_GRID_HPP
