#include <atomic>
#include <sstream>
#include <thread>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/explore_grid.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/algorithms/trace_beam.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox::algorithms
{

namespace pe = lvox::algorithms::pad_estimators;

template <bool limit_ray_length, bool compute_hits, typename PadEstimator>
void explore_grid_impl(Grid& grid, const Scan& scan, const ComputeOptions& options, Logger logger)
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

    const auto          points_to_process = std::ranges::distance(*scan.m_points);
    std::atomic<size_t> processed_points  = 0;
    unsigned int        last_progress     = 0;

    struct PointRange
    {
        using const_iterator = PointCloudView::element_type::const_iterator;

        const_iterator m_start;
        const_iterator m_end;

        const_iterator begin() const { return m_start; }
        const_iterator end() const { return m_end; }
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
            processed_points.fetch_add(1, std::memory_order_relaxed);
        }
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

    unsigned int loaded_processed_points;
    do
    {
        loaded_processed_points = processed_points.load(std::memory_order_relaxed);
        const unsigned int current_progress =
            std::ceil((static_cast<float>(loaded_processed_points) / points_to_process) * 100.f);
        if (last_progress <= current_progress)
        {
            last_progress                          = current_progress;
            constexpr auto     progress_size       = 25;
            const unsigned int compressed_progress = (current_progress / 100.f) * progress_size;
            std::ostringstream oss;
            for (unsigned int i = 0; i < progress_size + 2; ++i)
            {
                oss << '\b';
            }
            oss << "[";
            for (unsigned int i = 0; i < compressed_progress; ++i)
            {
                oss << "=";
            }
            for (unsigned int i = 0; i < (progress_size - compressed_progress); ++i)
            {
                oss << " ";
            }
            oss << "]";

            std::cout << oss.str();
            std::cout.flush();
        }
    } while (last_progress < 100);
    std::cout << std::endl;
}
void explore_grid_theoriticals(Grid& grid, const Scan& scan, const ComputeOptions& options)
{
    constexpr bool limit_ray_length = false;
    constexpr bool compute_hits     = false;
    Logger         logger{"Explore grid theoriticals"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            explore_grid_impl<limit_ray_length, compute_hits, T>(grid, scan, options, logger);
        },
        options.m_pad_estimator
    );
}

void explore_grid(Grid& grid, const Scan& scan, const ComputeOptions& options)
{
    constexpr bool limit_ray_length = true;
    constexpr bool compute_hits     = true;
    Logger         logger{"Explore grid"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            explore_grid_impl<limit_ray_length, compute_hits, T>(grid, scan, options, logger);
        },
        options.m_pad_estimator
    );
}
} // namespace lvox::algorithms
