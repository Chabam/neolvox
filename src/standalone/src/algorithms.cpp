#include <cstddef>
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

auto compute_rays_count_and_length(
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

            if (data.m_hits)
            {
                const auto [x, y, z] = data.m_hits->index_of_point(pt);
                data.m_hits->at(x, y, z) += 1;
            }

            grid_traversal(
                data.m_counts,
                Beam{pt, beam_to_point},
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z] = voxel_hit_info.m_index;
                    data.m_counts.at(x, y, z) += 1;
                    data.m_lengths.at(x, y, z) += voxel_hit_info.m_distance_in_voxel;
                },
                beam_to_point.norm()
            );
        }

        logger.debug("thread finished");
    };
    compute_in_parallel(*scan.m_points, data, trace_points_from_scanner, options.job_limit);
}

auto compute_theoriticals(
    const std::vector<Beam>& beams, ComputeData& data, const ComputeOptions& options
) -> void
{
    Logger logger{"Compute theoriticals"};
    const auto compute_rays_from_scanner = [&logger](ComputeData& data, auto&& beams) -> void {
        for (const auto& beam : beams)
        {
            grid_traversal(
                data.m_counts,
                beam,
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z] = voxel_hit_info.m_index;
                    data.m_counts.at(x, y, z) += 1;
                    data.m_lengths.at(x, y, z) += voxel_hit_info.m_distance_in_voxel;
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
            .m_hits    = std::invoke([&]() -> std::optional<CountGrid> {
                if constexpr (std::is_same_v<PadComputeMethod, pad_compute_methods::BeerLambert>)
                {
                    return {CountGrid{total_bounds, options.voxel_size}};
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
        const auto compute_pad_for_cell =
            [&logger, &pad_result, &pad_compute_method, scan_count = scans.size()](
                const ComputeData& data, auto&& idx_range
            ) -> void {
            for (const auto& idx : idx_range)
            {
                // Weighted sum of the PAD to avoid doing the average afterwards
                pad_result.at(idx) +=
                    pad_compute_method(data, idx) / static_cast<double>(scan_count);
            }
            logger.debug("Compute PAD job finished");
        };

        compute_in_parallel(indexes_of_cells_with_data, data, compute_pad_for_cell, options.job_limit);
    }

    return pad_result;
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options)
    -> PadResult
{
    switch (options.pad_computation_method)
    {
    case ComputeOptions::PADMethod::BeerLambert:
        return compute_pad_impl<pad_compute_methods::BeerLambert>(scans, options);
    case ComputeOptions::PADMethod::ContactFrequency:
        return compute_pad_impl<pad_compute_methods::ContactFrequency>(scans, options);
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
