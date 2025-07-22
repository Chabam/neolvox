#include <atomic>
#include <numbers>
#include <ranges>
#include <stdexcept>
#include <thread>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/grid_traversal.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>

#include "lvox/algorithms/pad_compute_methods.hpp"
#include "lvox/voxel/grid.hpp"

namespace lvox::algorithms
{

template <typename T>
struct is_bl : std::is_same<T, algorithms::pad_compute_methods::BeerLambert>
{
};

template <typename T>
struct is_cf : std::is_same<T, algorithms::pad_compute_methods::ContactFrequency>
{
};

template <typename T>
struct is_uplbl : std::is_same<T, algorithms::pad_compute_methods::UnequalPathLengthBeerLambert>
{
};

template <typename T>
struct pad_method_uses_hit_grid : std::disjunction<is_bl<T>, is_cf<T>, is_uplbl<T>>
{
};

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

template <typename PadComputeMethod>
auto compute_rays_count_and_length_impl(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    Logger logger{"Compute ray counts and length"};

    const auto trace_points_from_scanner = [&logger,
                                            &scan](ComputeData& data, auto&& points) -> void {
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            const Vector beam_to_point{scan_origin - pt};

            if constexpr (pad_method_uses_hit_grid<PadComputeMethod>::value)
            {
                const auto [x, y, z] = data.m_hits->index_of_point(pt);
                data.m_hits->at(x, y, z).fetch_add(1, std::memory_order_relaxed);
            }

            grid_traversal(
                data.m_counts,
                Beam{pt, beam_to_point},
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z] = voxel_hit_info.m_index;
                    if constexpr (is_uplbl<PadComputeMethod>::value)
                    {
                        const double length_in_voxel = voxel_hit_info.m_distance_in_voxel;
                        auto&        count_ref       = data.m_counts.at(x, y, z);
                        auto&        length_sum_ref  = data.m_lengths.at(x, y, z);
                        auto&        variance_ref    = data.m_lengths_variance->at(x, y, z);

                        // TODO: make this configurable maybe?
                        // So far it's based on Computree's NeedleFromDimension
                        constexpr double elem_length   = 0.06;
                        constexpr double elem_diameter = 0.02;
                        constexpr double attenuation_coeff =
                            (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;

                        const double attenuated_length = -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

                        count_ref += 1;
                        length_sum_ref += attenuated_length ;

                        // Best effort for calculating the variance,
                        // might be off because this line definitely
                        // represents multiple instructions.
                        variance_ref +=
                            std::pow(attenuated_length - (length_sum_ref / count_ref), 2) / count_ref;
                    }
                    else
                    {
                        data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed);
                        data.m_lengths.at(x, y, z).fetch_add(voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed);
                    }
                },
                beam_to_point.norm()
            );
        }

        logger.debug("thread finished");
    };
    compute_in_parallel(*scan.m_points, data, trace_points_from_scanner, options.job_limit);
}

auto compute_rays_count_and_length(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    switch (options.pad_computation_method)
    {
    case ComputeOptions::PADMethod::BeerLambert:
        return compute_rays_count_and_length_impl<pad_compute_methods::BeerLambert>(
            scan, data, options
        );
    case ComputeOptions::PADMethod::ContactFrequency:
        return compute_rays_count_and_length_impl<pad_compute_methods::ContactFrequency>(
            scan, data, options
        );
    case ComputeOptions::PADMethod::UnequalPathLengthBeerLambert:
        return compute_rays_count_and_length_impl<
            pad_compute_methods::UnequalPathLengthBeerLambert>(scan, data, options);
    default:
        throw std::runtime_error{"Unsupported PAD Computation method provided"};
    }
}

auto compute_theoriticals(
    const std::vector<Beam>& beams, ComputeData& data, const ComputeOptions& options
) -> void
{
    Logger     logger{"Compute theoriticals"};
    const auto compute_rays_from_scanner = [&logger](ComputeData& data, auto&& beams) -> void {
        for (const auto& beam : beams)
        {
            grid_traversal(
                data.m_counts,
                beam,
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z] = voxel_hit_info.m_index;
                    data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed);
                    data.m_lengths.at(x, y, z).fetch_add(voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed);
                }
            );
        }

        logger.debug("thread finished");
    };

    compute_in_parallel(beams, data, compute_rays_from_scanner, options.job_limit);
}

template <typename PadComputeMethod>
auto compute_pad_impl(const std::vector<lvox::Scan>& scans, const ComputeOptions& options)
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Bounds total_bounds = compute_scene_bounds(scans);

    PadResult pad_result{total_bounds, options.voxel_size};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {

        ComputeData data{
            .m_counts  = CountGrid{total_bounds, options.voxel_size},
            .m_lengths = LengthGrid{total_bounds, options.voxel_size},
            .m_hits    = std::invoke([&total_bounds, &options]() -> std::optional<CountGrid> {
                if constexpr (pad_method_uses_hit_grid<PadComputeMethod>::value)
                {
                    return {CountGrid{total_bounds, options.voxel_size}};
                }
                return {};
            }),
            .m_lengths_variance =
                std::invoke([&total_bounds, &options]() -> std::optional<LengthGrid> {
                    if constexpr (is_uplbl<PadComputeMethod>::value)
                    {
                        return {LengthGrid{total_bounds, options.voxel_size}};
                    }
                    return {};
                })
        };

        if (options.theoritical_scanner)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(options.theoritical_scanner->get_beams(), data, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());

        compute_rays_count_and_length(scan, data, options);

        // TODO: Make the treshold configurable
        const auto indexes_of_cells_with_data =
            data.m_counts | std::views::enumerate | std::views::filter([](const auto& pair) {
                return std::get<1>(pair) >= 5;
            }) |
            std::views::keys | std::ranges::to<std::vector<Index>>();

        PadComputeMethod pad_compute_method;
        const auto       compute_pad_for_cell =
            [&logger, &pad_result, &pad_compute_method, scan_count = scans.size()](
                const ComputeData& data, auto&& idx_range
            ) -> void {
            for (const auto& idx : idx_range)
            {
                // Weighted sum of the PAD to avoid doing the average afterwards
                pad_result.at(idx).fetch_add(
                    pad_compute_method(data, idx) / static_cast<double>(scan_count),
                    std::memory_order_relaxed
                );
            }
            logger.debug("Compute PAD job finished");
        };

        compute_in_parallel(
            indexes_of_cells_with_data, data, compute_pad_for_cell, options.job_limit
        );
    }

    return pad_result;
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
{
    switch (options.pad_computation_method)
    {
    case ComputeOptions::PADMethod::BeerLambert:
        return compute_pad_impl<pad_compute_methods::BeerLambert>(scans, options);
    case ComputeOptions::PADMethod::ContactFrequency:
        return compute_pad_impl<pad_compute_methods::ContactFrequency>(scans, options);
    case ComputeOptions::PADMethod::UnequalPathLengthBeerLambert:
        return compute_pad_impl<pad_compute_methods::UnequalPathLengthBeerLambert>(scans, options);
    default:
        throw std::runtime_error{"Unsupported PAD Computation method provided"};
    }
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
