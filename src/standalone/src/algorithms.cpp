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

#include "lvox/scanner/trajectory.hpp"
#include "lvox/voxel/grid.hpp"

namespace lvox::algorithms
{

auto compute_rays_count_and_length(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    using dim = pdal::Dimension::Id;

    Logger logger{"Compute ray counts and length"};

    const PointCloudView& points      = scan.m_points;
    const Index           point_count = points->size();
    const Index points_per_core = std::ceil(static_cast<float>(point_count) / options.job_limit);

    logger.debug(
        R"(
Core count      {}
Point count     {}
Point per core  {})",
        options.job_limit,
        point_count,
        points_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically joined
    {
        std::vector<std::jthread> threads;
        const auto                trace_points_from_scanner = [&logger,
                                                &scan](ComputeData& data, auto&& points) -> void {
            for (const auto& point : points)
            {
                const Point pt{
                    point.template getFieldAs<double>(dim::X),
                    point.template getFieldAs<double>(dim::Y),
                    point.template getFieldAs<double>(dim::Z),
                };

                const Point scan_origin = std::visit(
                    Scan::ComputeBeamOrigin{point.template getFieldAs<double>(dim::GpsTime)},
                    scan.m_scanner_origin
                );

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

        for (auto chunk : *points | std::views::chunk(points_per_core))
        {
            threads.emplace_back(trace_points_from_scanner, std::ref(data), std::move(chunk));
        }
    }
}

auto compute_theoriticals(
    const std::vector<Beam>& beams, ComputeData& data, const ComputeOptions& options
) -> void
{
    using dim = pdal::Dimension::Id;

    using dim = pdal::Dimension::Id;
    Logger logger{"Compute theoriticals"};

    const Index beam_count     = beams.size();
    const Index beams_per_core = std::ceil(static_cast<float>(beam_count) / options.job_limit);

    logger.debug(
        R"(
Core count      {}
Beams count     {}
Beams per core  {})",
        options.job_limit,
        beam_count,
        beams_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically joined
    {
        std::vector<std::jthread> threads;
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
        for (const auto chunk : beams | std::views::chunk(beams_per_core))
        {
            threads.emplace_back(compute_rays_from_scanner, std::ref(data), chunk);
        }
    }
}

auto compute_beer_lambert(const ComputeData& data, const Index& index) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    const CountGrid& hits_grid       = *data.m_hits;
    const auto       hits            = hits_grid.is_na(index) ? 0 : hits_grid.at(index).load();
    const auto       ray_count       = data.m_counts.at(index).load();
    const double     RDI             = hits / static_cast<double>(ray_count);
    const double     mean_ray_length = data.m_lengths.at(index).load() / ray_count;

    return -(std::log(1. - RDI) / G(mean_ray_length));
}

auto compute_bias_corrected_maximum_likelihood_estimator(
    const ComputeData& data, const Index& index
) -> double
{
    // TODO: implement
    return 0.;
}

auto compute_pad(const std::vector<lvox::Scan>& scans, const PADComputeOptions& options)
    -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    std::function<double(const ComputeData& data, const Index& index)> pad_compute_method;
    bool needs_hits_computation = false;
    switch (options.pad_computation_method)
    {
    case PADComputeOptions::PADMethod::BeerLambert:
        pad_compute_method     = compute_beer_lambert;
        needs_hits_computation = true;
        break;
    case PADComputeOptions::PADMethod::BiasCorrectedMaximumLikelihoodEstimator:
        pad_compute_method = compute_bias_corrected_maximum_likelihood_estimator;
        break;
    default:
        throw std::runtime_error{"Unsupported PAD Computation method provided"};
    }

    const Bounds total_bounds = compute_scene_bounds(scans, options);

    PadResult pad_result{total_bounds, options.voxel_size};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {

        ComputeData data{
            .m_counts  = CountGrid{total_bounds, options.voxel_size},
            .m_lengths = LengthGrid{total_bounds, options.voxel_size},
            .m_hits    = needs_hits_computation ? CountGrid{total_bounds, options.voxel_size}
                                                : std::optional<CountGrid>{},
        };

        if (options.theoritical_scanner)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(options.theoritical_scanner->get_beams(), data, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());

        // TODO: preload a spherical region around scanner to avoid contention with the grid
        compute_rays_count_and_length(scan, data, options);

        const auto index_of_cells_with_data =
            data.m_counts | std::views::enumerate | std::views::filter([](const auto& pair) {
                return std::get<1>(pair) > 0;
            }) |
            std::views::keys | std::ranges::to<std::vector<Index>>();
        const auto  cell_count     = std::ranges::distance(index_of_cells_with_data);
        const Index cells_per_core = std::ceil(cell_count / options.job_limit);

        logger.info("Computing PAD", scans.size());
        logger.debug(
            R"(
Core count      {}
Cells count     {}
Cells per core  {})",
            options.job_limit,
            cell_count,
            cells_per_core
        );

        constexpr auto is_under_threshold = [](const auto& ray_count) -> bool {
            return ray_count < 5;
        };

        std::vector<std::jthread> threads;

        const auto process_cells_with_data =
            [&logger,
             &is_under_threshold,
             &pad_compute_method,
             &pad_result,
             scan_count = scans.size()](const ComputeData& data, auto&& idx_range) -> void {
            for (const auto& idx : idx_range)
            {
                const auto& ray_count = data.m_counts.at(idx);
                if (is_under_threshold(ray_count))
                    continue;

                // Weighted sum of the PAD to avoid the mean afterwards
                pad_result.at(idx) +=
                    pad_compute_method(data, idx) / static_cast<double>(scan_count);
            }
            logger.debug("Compute PAD job finished");
        };

        for (auto&& chunk : index_of_cells_with_data | std::views::chunk(cells_per_core))
        {
            threads.emplace_back(process_cells_with_data, std::ref(data), std::move(chunk));
        }
    }

    return pad_result;
}

auto compute_scene_bounds(const std::vector<lvox::Scan>& scans, const ComputeOptions& options)
    -> lvox::Bounds
{
    Bounds total_bounds;

    for (const auto& scan : scans)
    {
        Bounds scan_bounds;
        scan.m_points->calculateBounds(scan_bounds);
        total_bounds.grow(scan_bounds);
    }

    return total_bounds;
}

} // namespace lvox::algorithms
