#include <atomic>
#include <future>
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

// template <std::ranges::range R, typename F>
// auto compute_in_parallel(const R& objects, ComputeData& data, const F& func, unsigned int job_count)
// {
//     const size_t object_count     = std::distance(std::begin(objects), std::end(objects));
//     const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / job_count);

//     std::vector<std::jthread> threads;
//     for (auto chunk : objects | std::views::chunk(objects_per_core))
//     {
//         threads.emplace_back(func, std::ref(data), std::move(chunk));
//     }
// }

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
//     constexpr double attenuation_coeff = (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;

//     const double attenuated_length =
//         -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

//     // No variance possible if the count is not big enough
//     if (previous_count < 2)
//         return;

//     // https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
//     const double previous_mean = previous_lengths / previous_count;
//     const double delta         = length_in_voxel - previous_mean;
//     const double new_count     = previous_count + 1;
//     const double new_mean      = previous_mean + (delta / new_count);
//     const double delta_2       = length_in_voxel - new_mean;

//     // -1 on the count here because we are computing a variance sample.
//     variance_ref.fetch_add((delta * delta_2) / (new_count - 1), std::memory_order_relaxed);
// }

template <bool limit_ray_length, typename PadEstimator>
auto compute_rays_count_and_length_impl(
    const Grid& grid, const Scan& scan, const ComputeOptions& options, Logger logger
) -> VisitedVoxels
{
    auto grid_traversal = GridTraversalVoxelRounding{grid};// std::invoke([&grid]() {
    //     if constexpr (pe::is_uplbl<PadEstimator>::value)
    //         return GridTraversalExactDistance{grid};
    //     else
    //         return GridTraversalVoxelRounding{grid};
    // });

    const auto ray_trace = [&logger, &scan, &grid_traversal](auto&& points) -> VisitedVoxels {
        VisitedVoxels visited_voxels;
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

            const auto traced_voxels =
                grid_traversal(Beam{scan_origin, beam_to_point}, max_distance);

            // for (auto&& voxel : traced_voxels)
            // {
            //     auto it = visited_voxels.find(voxel.m_index);
            //     if (it != visited_voxels.end())
            //     {
            //         auto& existing_voxel = it->second;
            //         existing_voxel.m_count += 1;
            //         existing_voxel.m_lengths += voxel.m_distance_in_voxel;
            //     }
            //     else
            //     {
            //         visited_voxels[voxel.m_index] = PadComputeData{1, voxel.m_distance_in_voxel, 0};
            //     }
            // }
        }

        logger.debug("thread finished");
        return visited_voxels;
    };
    const size_t object_count     = std::distance(std::begin(*scan.m_points), std::end(*scan.m_points));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / options.m_job_limit);

    std::vector<std::future<VisitedVoxels>> all_visited_voxels;
    for (auto chunk : *scan.m_points | std::views::chunk(objects_per_core))
    {
        all_visited_voxels.emplace_back(std::async(ray_trace,  std::move(chunk)));
    }

    VisitedVoxels visited_voxels;
    for (auto&& res : std::move(all_visited_voxels))
    {
        for (auto&& [idx, voxel] : res.get())
        {
            auto it = visited_voxels.find(idx);
            if (it != visited_voxels.end())
            {
                auto& existing_voxel = it->second;
                existing_voxel.m_count += voxel.m_count;
                existing_voxel.m_lengths += voxel.m_lengths;
            }
            else
            {
                visited_voxels.emplace(idx, std::move(voxel));
            }
        }
    }

    return visited_voxels;
}

auto compute_rays_count_and_length(
    const Grid& grid, const Scan& scan, const ComputeOptions& options
) -> VisitedVoxels
{
    constexpr bool limit_ray_length = true;
    Logger         logger{"Compute Rays count and length"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VisitedVoxels {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, T>(grid, scan, options, logger);
        },
        options.m_pad_estimator
    );
}

auto compute_theoriticals(const Grid& grid, const Scan& scan, const ComputeOptions& options)
    -> VisitedVoxels
{
    constexpr bool limit_ray_length = false;
    Logger         logger{"Compute theoriticals"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VisitedVoxels {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, T>(grid, scan, options, logger);
        },
        options.m_pad_estimator
    );
}

auto compute_hits(const Grid& grid, const Scan& scan,  const ComputeOptions& options) -> Hits
{
    Logger     logger{"Compute hits"};
    const auto count_hits = [&grid, &logger, &scan](auto&& points) -> Hits {
        Hits hits;
        for (const auto& timed_point : points)
        {
            const auto idx = grid.index_of_point(timed_point.m_point);

            auto it = hits.find(idx);
            if (it != hits.end())
            {
                it->second += 1;
            }
            else
            {
                hits[idx] = 1;
            }
        }
        return hits;
    };

    const size_t object_count     = std::distance(std::begin(*scan.m_points), std::end(*scan.m_points));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / options.m_job_limit);

    std::vector<std::future<Hits>> all_hits;
    for (auto chunk : *scan.m_points | std::views::chunk(objects_per_core))
    {
        all_hits.emplace_back(std::async(count_hits,  std::move(chunk)));
    }

    Hits hits;
    for (auto&& res : std::move(all_hits))
    {
        for (auto&& [idx, hit_count] : res.get())
        {
            auto it = hits.find(idx);
            if (it != hits.end())
            {
                it->second += hit_count;
            }
            else
            {
                hits[idx] = hit_count;
            }
        }
    }

    return hits;
}

// auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
// {
//     Logger logger{"LVOX"};
//     logger.info("Scan count {}", scans.size());

//     const Grid grid{compute_scene_bounds(scans), options.m_voxel_size};

//     const bool uses_variance = std::holds_alternative<pe::UnequalPathLengthBeerLambert>(
//         options.m_pad_estimator
//     );

//     for (const auto [scan_num, scan] : std::views::enumerate(scans))
//     {

//         if (options.m_compute_theoriticals && scan.m_blank_shots)
//         {
//             logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
//             compute_theoriticals(grid, scan, options);
//         }

//         logger.info("Compute hits {}/{}", scan_num + 1, scans.size());
//         Hits hits = compute_hits(grid, scan, options);
//         logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());
//         VisitedVoxels voxels = compute_rays_count_and_length(grid, scan, options);

//         // TODO: Make the treshold configurable
//         const auto indexes_of_cells_with_data =
//             voxels | std::views::filter([](const PadComputeData& voxel) {
//                 return voxel.m_count >= 5;
//             }) |
//             std::views::keys | std::ranges::to<std::vector<Index3D>>();

//         const auto compute_pad_for_cell =
//             [&logger, &pad_result, &options, scan_count = scans.size()](
//                 const ComputeData& data, auto&& idx_range
//             ) -> void {
//                 for (const auto& idx : idx_range)
//                 {
//                     const auto pad_estimation = [&data, &idx](auto&& chosen_estimator) -> double {
//                         using T = std::decay_t<decltype(chosen_estimator)>;
//                         if constexpr (pe::is_bl<T>::value)
//                             return pe::beer_lambert(data, idx);
//                         else if constexpr (pe::is_cf<T>::value)
//                             return pe::contact_frequency(data, idx);
//                         else if constexpr (pe::is_uplbl<T>::value)
//                             return pe::unequal_path_length_beer_lambert(data, idx);
//                         else
//                             static_assert(false, "PAD estimation unimplemented");
//                     };

//                     // Weighted sum of the PAD to avoid doing the average afterwards
//                     pad_result.at(idx).fetch_add(
//                         std::visit(pad_estimation, options.m_pad_estimator) / scan_count,
//                         std::memory_order_relaxed
//                     );
//                 }
//                 logger.debug("Compute PAD job finished");
//             };

//         compute_in_parallel(
//             indexes_of_cells_with_data, data, compute_pad_for_cell, options.m_job_limit
//         );
//     }

//     return pad_result;
// }

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
