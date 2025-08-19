#include <execution>
#include <future>
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

namespace lvox::algorithms
{

PadComputeData::PadComputeData(double length)
    : m_hits{0}
    , m_counts{1}
    , m_lengths{length}
{}


PadComputeData::PadComputeData(LengthWithVariance length)
    : m_hits{0}
    , m_counts{1}
    , m_lengths{length}
{}

auto PadComputeData::operator+=(const PadComputeData& rhs) -> PadComputeData&
{
    m_counts += rhs.m_counts;

    std::visit(
        [&rhs](auto& length) mutable -> void {
            using T = std::decay_t<decltype(length)>;
            if constexpr (std::is_same<T, double>::value)
            {
                length += std::get<double>(rhs.m_lengths);
            }
            else
            {
                auto [new_length, new_variance]  = std::get<LengthWithVariance>(rhs.m_lengths);
                auto& [cur_length, cur_variance] = length;
                cur_length += new_length;
                cur_variance += new_variance;
            }
        },
        m_lengths
    );

    return *this;
}

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
    {
        for (auto&& val : res.get())
        {
            auto [ref, inserted] = out.try_insert(val);
            if (!inserted)
                ref += val.second;
        }
    }
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
) -> VoxelsPadInfo
{
    auto grid_traversal = std::invoke([&grid]() {
        if constexpr (pe::is_uplbl<PadEstimator>::value)
            return GridTraversalExactDistance{grid};
        else
            return GridTraversalVoxelRounding{grid};
    });

    const auto ray_trace = [&](auto&& points) -> VoxelsPadInfo {
        VoxelsPadInfo visited_voxels{grid};
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

                PadComputeData hit_voxel;
                hit_voxel.m_hits = 1;

                auto [ref, inserted] = visited_voxels.try_insert(idx, hit_voxel);
                if (!inserted)
                    ref.m_hits += 1;
            }

            if constexpr (limit_ray_length)
            {
                max_distance = beam_to_point.norm();
            }

            const auto traced_voxels =
                grid_traversal(Beam{scan_origin, beam_to_point}, max_distance);

            for (auto&& voxel : traced_voxels)
            {
                PadComputeData visited_voxel{voxel.m_distance_in_voxel};
                auto [ref, inserted] = visited_voxels.try_insert(voxel.m_index, visited_voxel);

                if (!inserted)
                    ref += visited_voxel;
            }
        }

        logger.debug("thread finished");
        return visited_voxels;
    };

    VoxelsPadInfo counts_and_lengths{grid};
    parallel_compute_then_merge(*scan.m_points, options.m_job_limit, ray_trace, counts_and_lengths);
    return counts_and_lengths;
}

auto compute_rays_count_and_length(
    const Grid& grid, const Scan& scan, const ComputeOptions& options
) -> VoxelsPadInfo
{
    constexpr bool limit_ray_length = true;
    constexpr bool compute_hits = true;
    Logger         logger{"Compute Rays count and length"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VoxelsPadInfo {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_theoriticals(const Grid& grid, const Scan& scan, const ComputeOptions& options)
    -> VoxelsPadInfo
{
    constexpr bool limit_ray_length = false;
    constexpr bool compute_hits = false;
    Logger         logger{"Compute theoriticals"};
    return std::visit(
        [&](auto&& chosen_estimator) -> VoxelsPadInfo {
            using T = std::decay_t<decltype(chosen_estimator)>;
            return compute_rays_count_and_length_impl<limit_ray_length, compute_hits, T>(
                grid, scan, options, logger
            );
        },
        options.m_pad_estimator
    );
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Grid grid{compute_scene_bounds(scans), options.m_voxel_size};

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    PadResult pad_result{grid};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {
        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(grid, scan, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());
        VoxelsPadInfo voxels = compute_rays_count_and_length(grid, scan, options);

        // TODO: Make the treshold configurable
        auto voxels_with_data =
            voxels | std::views::filter([](const VoxelsPadInfo::value_pair& pair) {
                return pair.second.m_counts >= 5;
            }) | std::ranges::to<std::vector>();

        const auto compute_pad_for_cell = [&options, scan_count = scans.size()](
            const VoxelsPadInfo::value_pair& pair
        ) -> PadResult::value_pair {
            const auto& [idx, data] = pair;
            const auto pad_estimation = [&data](auto&& chosen_estimator) -> double {
                using T = std::decay_t<decltype(chosen_estimator)>;
                if constexpr (pe::is_bl<T>::value)
                    return pe::beer_lambert(data);
                else if constexpr (pe::is_cf<T>::value)
                    return pe::contact_frequency(data);
                else if constexpr (pe::is_uplbl<T>::value)
                    return pe::unequal_path_length_beer_lambert(data);
                else
                    static_assert(false, "PAD estimation unimplemented");
            };

            // Weighted sum of the PAD to avoid doing the average afterwards
            return {idx, std::visit(pad_estimation, options.m_pad_estimator) / scan_count};
        };


        std::vector<PadResult::value_pair> computed_pad_values{voxels_with_data.size()};

        std::transform(
            std::execution::par_unseq,
            voxels_with_data.begin(),
            voxels_with_data.end(),
            computed_pad_values.begin(),
            compute_pad_for_cell
        );

        for (auto&& [idx, pad_value] : computed_pad_values)
        {
            auto [ref, inserted] = pad_result.try_insert(idx, pad_value);

            if (!inserted)
                ref += pad_value;
        }
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
