#include <iterator>
#include <limits>
#include <ranges>
#include <thread>
#include <variant>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/grid_traversal.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox::algorithms
{

template <std::ranges::range R, typename F>
auto compute_in_parallel(const R& objects, unsigned int job_count, const F& func)
{
    const size_t object_count     = std::distance(std::begin(objects), std::end(objects));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / job_count);

    std::vector<std::jthread> threads;
    for (auto chunk : objects | std::views::chunk(objects_per_core))
    {
        threads.emplace_back(func, std::move(chunk));
    }
}

template <bool limit_ray_length, bool compute_hits, typename PadEstimator>
auto compute_rays_count_and_length_impl(
    Grid& grid, const Scan& scan, const ComputeOptions& options, Logger logger
) -> void
{
    auto grid_traversal = std::invoke([&grid]() {
        if constexpr (pe::is_uplbl<PadEstimator>::value)
            return GridTraversalExactDistance{grid};
        else
            return GridTraversalVoxelRounding{grid};
    });

    const auto points_to_process = std::ranges::distance(*scan.m_points);


    struct PointRange
    {
        using const_iterator = PointCloudView::element_type::const_iterator;

        const_iterator m_start;
        const_iterator m_end;

        auto begin() const -> const_iterator { return m_start; }
        auto end() const -> const_iterator { return m_end; }
    };

    const auto ray_trace = [&](const PointRange& task) -> void {
        for (const auto& timed_point : task)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            // If we compute the hits, we start at the point in the
            // point cloud towards the scanner. This is done to avoid
            // recomputing the index of the point since it is provided
            // by the grid traversal.
            const Vector beam_dir = std::invoke([&scan_origin, &pt]() -> Vector {
                if constexpr (compute_hits)
                    return Vector{scan_origin - pt};
                else
                    return Vector{pt - scan_origin};
            });

            // Invertly, if we do not compute the hits, we start at
            // the scanner towards the point.
            const Point point_origin = std::invoke([&scan_origin, &pt]() -> Vector {
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
                    if constexpr (pe::is_uplbl<PadEstimator>::value)
                        grid.add_length_count_and_variance(hit.m_index, hit.m_distance_in_voxel);
                    else
                        grid.add_length_and_count(hit.m_index, hit.m_distance_in_voxel);

                    if constexpr (compute_hits)
                    {
                        if (!is_hit_computed)
                        {
                            grid.register_hit(hit.m_index);
                            is_hit_computed = true;
                        }
                    }
                },
                max_distance
            );
        }

        logger.debug("thread finished");
    };

    const size_t point_count     = scan.m_points->size();
    const size_t points_per_tasks = std::ceil(static_cast<float>(point_count) / options.m_job_limit);
    std::vector<std::jthread> threads;
    auto start_it = scan.m_points->begin();
    for (auto i = 0; i < point_count; i += (points_per_tasks + 1))
    {
        auto next_end = start_it + std::min(points_per_tasks, point_count - i);
        threads.emplace_back(ray_trace, PointRange{start_it, next_end});

        start_it = next_end + 1;
    }
}

auto compute_rays_count_and_length(Grid& grid, const Scan& scan, const ComputeOptions& options)
    -> void
{
    constexpr bool limit_ray_length = true;
    constexpr bool compute_hits     = true;
    Logger         logger{"Compute Rays count and length"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_theoriticals(Grid& grid, const Scan& scan, const ComputeOptions& options) -> void
{
    constexpr bool limit_ray_length = false;
    constexpr bool compute_hits     = false;
    Logger         logger{"Compute theoriticals"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_scene_bounds(const std::vector<lvox::Scan>& scans) -> lvox::Bounds
{
    Bounds total_bounds;

    for (const auto& scan : scans)
        total_bounds.grow(scan.m_bounds);

    return total_bounds;
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> Grid
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    Grid grid{compute_scene_bounds(scans), options.m_voxel_size, uses_variance};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {
        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(grid, scan, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());
        compute_rays_count_and_length(grid, scan, options);

        logger.info("Estimating PAD {}/{}", scan_num + 1, scans.size());
        std::visit(
            [&grid](auto&& chosen_estimator) -> void {
                grid.compute_pad(chosen_estimator);
            },
            options.m_pad_estimator
        );
    }

    return grid;
}

} // namespace lvox::algorithms
