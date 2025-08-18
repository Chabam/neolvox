#include <atomic>
#include <limits>
#include <numbers>
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
auto compute_in_parallel(const R& objects, ComputeData& data, const F& func, unsigned int job_count)
{
    const size_t object_count     = std::distance(std::begin(objects), std::end(objects));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / job_count);

    std::vector<std::jthread> threads;
    for (auto chunk : objects | std::views::chunk(objects_per_core))
    {
        threads.emplace_back(func, std::ref(data), std::move(chunk));
    }
}

auto compute_lengths_variance(
    double              length_in_voxel,
    LengthGrid::cell_t& variance_ref,
    double              previous_lengths,
    unsigned int        previous_count
) -> void
{
    // TODO: make this configurable maybe?
    // So far it's based on Computree's NeedleFromDimension
    constexpr double elem_length       = 0.06;
    constexpr double elem_diameter     = 0.02;
    constexpr double attenuation_coeff = (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;

    const double attenuated_length =
        -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

    // No variance possible if the count is not big enough
    if (previous_count < 2)
        return;

    // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
    const double previous_mean = previous_lengths / previous_count;
    const double delta         = length_in_voxel - previous_mean;
    const double new_count     = previous_count + 1;
    const double new_mean      = previous_mean + (delta / new_count);
    const double delta_2       = length_in_voxel - new_mean;

    // -1 on the count here because we are computing a variance sample.
    variance_ref.fetch_add((delta * delta_2) / (new_count - 1), std::memory_order_relaxed);
}

template <bool limit_ray_length, typename PadEstimator>
auto compute_rays_count_and_length_impl(
    const Scan& scan, ComputeData& data, const ComputeOptions& options, Logger logger
) -> void
{
    auto grid_traversal = std::invoke([&data]() {
        if constexpr (pe::is_uplbl<PadEstimator>::value)
        {
            return GridTraversalExactDistance{data.m_counts};
        }
        else
        {
            return GridTraversalVoxelRounding{data.m_counts};
        }
    });

    const auto ray_trace = [&logger, &scan, &grid_traversal](ComputeData& data, auto&& points) -> void {
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            const Vector beam_to_point{pt - scan_origin};

            double max_distance = std::numeric_limits<double>::infinity();

            if constexpr (limit_ray_length)
            {
                max_distance = beam_to_point.norm();
            }

            grid_traversal(
                Beam{scan_origin, beam_to_point},
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z]        = voxel_hit_info.m_index;
                    const auto previous_lengths = data.m_lengths.at(x, y, z).fetch_add(
                        voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed
                    );
                    const auto previous_count =
                        data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed);

                    if constexpr (pe::estimator_uses_effective_lengths<PadEstimator>::value)
                    {
                        compute_lengths_variance(
                            voxel_hit_info.m_distance_in_voxel,
                            data.m_lengths_variance->at(x, y, z),
                            previous_lengths,
                            previous_count
                        );
                    }
                },
                max_distance
            );
        }

        logger.debug("thread finished");
    };
    compute_in_parallel(*scan.m_points, data, ray_trace, options.m_job_limit);
}

auto compute_rays_count_and_length(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    constexpr bool limit_ray_length = true;
    Logger         logger{"Compute Rays count and length"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            compute_rays_count_and_length_impl<limit_ray_length, T>(scan, data, options, logger);
        },
        options.m_pad_estimator
    );
}

auto compute_theoriticals(const Scan& scan, ComputeData& data, const ComputeOptions& options)
    -> void
{
    constexpr bool limit_ray_length = false;
    Logger         logger{"Compute theoriticals"};
    std::visit(
        [&](auto&& chosen_estimator) {
            using T = std::decay_t<decltype(chosen_estimator)>;
            compute_rays_count_and_length_impl<limit_ray_length, T>(scan, data, options, logger);
        },
        options.m_pad_estimator
    );
}

auto compute_hits(const Scan& scan, ComputeData& data, const ComputeOptions& options) -> void
{
    Logger     logger{"Compute hits"};
    const auto count_hits = [&logger, &scan](ComputeData& data, auto&& points) -> void {
        auto grid_traveral = GridTraversalVoxelRounding{data.m_counts};
        for (const auto& timed_point : points)
        {
            const auto [x, y, z] = data.m_hits.index_of_point(timed_point.m_point);
            data.m_hits.at(x, y, z).fetch_add(1, std::memory_order_relaxed);
        }
    };
    compute_in_parallel(*scan.m_points, data, count_hits, options.m_job_limit);
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Bounds total_bounds = compute_scene_bounds(scans);

    PadResult pad_result{total_bounds, options.m_voxel_size};

    const bool uses_variance = std::holds_alternative<pe::UnequalPathLengthBeerLambert>(
        options.m_pad_estimator
    );

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {
        ComputeData data{
            .m_counts  = CountGrid{total_bounds, options.m_voxel_size},
            .m_hits    = CountGrid{total_bounds, options.m_voxel_size},
            .m_lengths = LengthGrid{total_bounds, options.m_voxel_size}
        };

        if (uses_variance)
        {
            data.m_lengths_variance = LengthGrid{total_bounds, options.m_voxel_size};
        }

        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(scan, data, options);
        }

        logger.info("Compute hits {}/{}", scan_num + 1, scans.size());
        compute_hits(scan, data, options);
        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());
        compute_rays_count_and_length(scan, data, options);

        // TODO: Make the treshold configurable
        const auto indexes_of_cells_with_data =
            data.m_counts | std::views::enumerate | std::views::filter([](const auto& pair) {
                return std::get<1>(pair) >= 5;
            }) |
            std::views::keys | std::ranges::to<std::vector<Index>>();

        const auto compute_pad_for_cell =
            [&logger, &pad_result, &options, scan_count = scans.size()](
                const ComputeData& data, auto&& idx_range
            ) -> void {
                for (const auto& idx : idx_range)
                {
                    const auto pad_estimation = [&data, &idx](auto&& chosen_estimator) -> double {
                        using T = std::decay_t<decltype(chosen_estimator)>;
                        if constexpr (pe::is_bl<T>::value)
                            return pe::beer_lambert(data, idx);
                        else if constexpr (pe::is_cf<T>::value)
                            return pe::contact_frequency(data, idx);
                        else if constexpr (pe::is_uplbl<T>::value)
                            return pe::unequal_path_length_beer_lambert(data, idx);
                        else
                            static_assert(false, "PAD estimation unimplemented");
                    };

                    // Weighted sum of the PAD to avoid doing the average afterwards
                    pad_result.at(idx).fetch_add(
                        std::visit(pad_estimation, options.m_pad_estimator) / scan_count,
                        std::memory_order_relaxed
                    );
                }
                logger.debug("Compute PAD job finished");
            };

        compute_in_parallel(
            indexes_of_cells_with_data, data, compute_pad_for_cell, options.m_job_limit
        );
    }

    return pad_result;
}

auto compute_scene_bounds(const std::vector<lvox::Scan>& scans) -> lvox::Bounds
{
    Bounds total_bounds;

    for (const auto& scan : scans)
    {
        total_bounds.grow(scan.m_bounds);
    }

    return total_bounds;
}

} // namespace lvox::algorithms
