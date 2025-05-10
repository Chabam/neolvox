#include <ranges>
#include <thread>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/lvox.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox
{

struct LvoxComputeData
{
    CountGridPtr                m_counts;
    LengthGridPtr               m_lengths;
    std::optional<CountGridPtr> m_hits;
};

auto compute_rays_count_and_length(
    const PointCloudView&  points,
    const Point&           scan_origin,
    const LvoxComputeData& data,
    const LvoxOptions&     options
) -> void
{
    using dim = pdal::Dimension::Id;

    Logger logger{"Before"};

    const Index point_count     = points->size();
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

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        const auto                compute_ray_before =
            [&logger](
                const LvoxComputeData& data, const Point& scan_origin, auto&& points
            ) -> void {
            for (const auto& point : points)
            {
                const Point pt{
                    point.template getFieldAs<double>(dim::X),
                    point.template getFieldAs<double>(dim::Y),
                    point.template getFieldAs<double>(dim::Z)
                };

                const Vector beam_to_point{scan_origin - pt};

                Grid::traversal(
                    *data.m_counts,
                    Beam{scan_origin, beam_to_point},
                    [&data,
                     add_hits = data.m_hits.has_value()](const Grid::VoxelHitInfo& voxel_hit_info
                    ) mutable -> void {
                        const auto [x, y, z] = voxel_hit_info.m_index;
                        data.m_counts->at(x, y, z) += 1;
                        data.m_lengths->at(x, y, z) += 1;
                        if (add_hits)
                            (*data.m_hits)->at(x, y, z) += 1;
                    },
                    beam_to_point.norm()
                );
            }

            logger.debug("Before thread finished");
        };

        for (auto chunk : *points | std::views::chunk(points_per_core))
        {
            threads.emplace_back(
                compute_ray_before, std::ref(data), std::ref(scan_origin), std::move(chunk)
            );
        }
    }
}

auto compute_theoriticals(
    const std::vector<Beam>& beams, const LvoxComputeData& data, const LvoxOptions& options
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

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        const auto                compute_rays_from_scanner =
            [&logger](const LvoxComputeData& data, auto&& beams) -> void {
            for (const auto& beam : beams)
            {
                Grid::traversal(
                    *data.m_counts,
                    beam,
                    [&data](const Grid::VoxelHitInfo& voxel_hit_info) mutable -> void {
                        const auto [x, y, z] = voxel_hit_info.m_index;
                        data.m_counts->at(x, y, z) += 1;
                        data.m_lengths->at(x, y, z) += voxel_hit_info.m_distance_in_voxel;
                    }
                );
            }

            logger.debug("Theoritical thread finished");
        };
        for (const auto chunk : beams | std::views::chunk(beams_per_core))
        {
            threads.emplace_back(compute_rays_from_scanner, std::ref(data), chunk);
        }
    }
}

auto compute_beer_lambert(std::uint32_t ray_count, double length) -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };

    // TODO: implement
    return 0.;
}

auto compute_bias_corrected_maximum_likelihood_estimator(std::uint32_t ray_count, double length)
    -> double
{
    // TODO: implement
    return 0.;
}

auto compute_pad(const std::vector<std::shared_ptr<lvox::Scan>>& scans, const LvoxOptions& options)
    -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    Bounds total_bounds;

    for (const auto& scan : scans)
    {
        Bounds scan_bounds;
        scan->get_points()->calculateBounds(scan_bounds);
        total_bounds.grow(scan_bounds);
    }

    std::function<double(Index, double)> pad_compute_method;
    bool                                 needs_hits_computation = false;
    switch (options.pad_computation_method)
    {
    case LvoxOptions::PADMethod::BeerLambert:
        pad_compute_method     = compute_beer_lambert;
        needs_hits_computation = true;
        break;
    case LvoxOptions::PADMethod::BiasCorrectedMaximumLikelihoodEstimator:
        pad_compute_method = compute_bias_corrected_maximum_likelihood_estimator;
        break;
    default:
        throw std::runtime_error{"Unsupported PAD Computation method provided"};
    }

    PadResult pda_result{total_bounds, options.voxel_size};
    for (Index i = 0; i < scans.size(); i++)
    {
        const auto&     scan = scans[i];
        LvoxComputeData data{
            .m_counts  = std::make_unique<CountGrid>(total_bounds, options.voxel_size),
            .m_lengths = std::make_unique<LengthGrid>(total_bounds, options.voxel_size),
            .m_hits    = needs_hits_computation
                             ? std::make_unique<CountGrid>(total_bounds, options.voxel_size)
                             : std::optional<CountGridPtr>{},
        };

        if (options.simulated_scanner)
        {
            logger.info("Compute theoriticals {}/{}", i + 1, scans.size());
            compute_theoriticals(scan->get_beams(), data, options);
        }

        logger.info("Compute before {}/{}", i + 1, scans.size());

        // TODO: preload a spherical region around scanner to avoid contention with the grid
        // TODO: use GPS time for the scan position
        compute_rays_count_and_length(
            scan->get_points(), scan->get_scan_position({}), data, options
        );

        {

            const auto  cells          = std::views::keys(*data.m_counts);
            const auto  cell_count     = std::ranges::distance(cells);
            const Index cells_per_core = std::ceil(cell_count / options.job_limit);

            logger.info("Computing PAD", scans.size());
            logger.debug(
                R"(
         Core count      {}
         Beams count     {}
         Beams per core  {})",
                options.job_limit,
                cell_count,
                cells_per_core
            );
            const auto is_under_threshold = [](const auto& ray_count) -> bool {
                return ray_count < 5;
            };
            std::vector<std::jthread> threads;

            const auto process_cells_with_data =
                [&logger, &is_under_threshold, &pad_compute_method](
                    const LvoxComputeData& data, std::ranges::range auto&& idx_range
                ) -> void {
                for (auto&& idx : idx_range)
                {
                    const auto& ray_count = data.m_counts->at(idx);
                    if (is_under_threshold(ray_count))
                        continue;

                    pad_compute_method(ray_count, data.m_lengths->at(idx));
                }
                logger.debug("Compute PAD job finished");
            };

            for (auto&& chunk : cells | std::views::chunk(cells_per_core))
            {
                threads.emplace_back(process_cells_with_data, std::ref(data), std::move(chunk));
            }
        }
    }

    return pda_result;
}

} // namespace lvox
