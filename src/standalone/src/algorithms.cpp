#include <iterator>
#include <limits>
#include <memory>
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
#include <lvox/voxel/coo_grid.hpp>

namespace lvox::algorithms
{

template <bool limit_ray_length, bool compute_hits, typename PadEstimator>
auto compute_rays_count_and_length_impl(
    Grid& grid, const Scan& scan, const ComputeOptions& options, Logger logger
) -> void
{
    auto grid_traversal = std::visit(
        [](const auto& grid) {
            if constexpr (pe::is_uplbl<PadEstimator>::value)
                return GridTraversalExactDistance{grid.bounded_grid()};
            else
                return GridTraversalVoxelRounding{grid.bounded_grid()};
        },
        grid
    );

    const auto points_to_process = std::ranges::distance(*scan.m_points);

    struct PointRange
    {
        using const_iterator = PointCloudView::element_type::const_iterator;

        const_iterator m_start;
        const_iterator m_end;

        auto begin() const -> const_iterator { return m_start; }
        auto end() const -> const_iterator { return m_end; }
    };

    const auto ray_trace = [&](const PointRange& points) -> void {
        for (const auto& timed_point : points)
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

                                grid.add_length_count_and_variance(
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
        }

        logger.debug("thread finished");
    };

    const size_t point_count = scan.m_points->size();
    const size_t points_per_tasks =
        std::ceil(static_cast<float>(point_count) / options.m_job_limit);
    std::vector<std::jthread> threads;
    auto                      start_it = scan.m_points->begin();
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

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> COOGrid
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    Grid grid = std::invoke([&]() -> Grid {
        if (options.m_use_sparse_grid)
            return ChunkedGrid{compute_scene_bounds(scans), options.m_voxel_size, uses_variance};
        else
            return DenseGrid{compute_scene_bounds(scans), options.m_voxel_size, uses_variance};
    });

    auto scan_num = 1;
    std::unique_ptr<COOGrid> result;
    for (const auto& scan : scans)
    {
        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num, scans.size());
            compute_theoriticals(grid, scan, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num, scans.size());
        compute_rays_count_and_length(grid, scan, options);

        result = std::visit([](const auto& grid) -> std::unique_ptr<COOGrid>{
            return std::make_unique<COOGrid>(grid);
        }, grid);

        logger.info("Estimating PAD {}/{}", scan_num, scans.size());
        std::visit(
            [&result](auto&& chosen_estimator) -> void {
                result->compute_pad(chosen_estimator);
            },
            options.m_pad_estimator
        );
        ++scan_num;
    }

    return *result;
}

} // namespace lvox::algorithms
