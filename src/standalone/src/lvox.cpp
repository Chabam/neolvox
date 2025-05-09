#include <thread>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/lvox.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox
{

auto compute_hits(const PointCloudView& points, LVoxGridPtr& grid, const LvoxOptions& options)
    -> void
{
    using dim = pdal::Dimension::Id;
    Logger logger{"Hits"};

    const auto  core_count      = std::thread::hardware_concurrency();
    const Index point_count     = points->size();
    const Index points_per_core = std::ceil(static_cast<float>(point_count) / core_count);

    logger.debug(
        R"(
Core count      {}
Point count     {}
Point per core  {})",
        core_count,
        point_count,
        points_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        auto                      start_it = points->begin();
        const auto                find_point_in_grid =
            [&logger](
                const LVoxGridPtr& grid, pdal::PointViewIter start_it, Index points_to_compute
            ) -> void {
            for (const auto& point : std::ranges::subrange(start_it, start_it + points_to_compute))
            {
                const auto [x, y, z] = grid->index_of_point({
                    point.getFieldAs<double>(dim::X),
                    point.getFieldAs<double>(dim::Y),
                    point.getFieldAs<double>(dim::Z),
                });
                grid->at(x, y, z) += 1;
            }
            logger.debug("Hits thread finished");
        };

        for (Index i = 0; i < core_count; ++i)
        {
            const Index points_to_compute = std::min(
                points_per_core, static_cast<Index>(std::distance(start_it, points->end()))
            );
            threads.emplace_back(find_point_in_grid, std::ref(grid), start_it, points_to_compute);
            start_it += points_to_compute;
        }
    }
}

auto compute_before(
    const PointCloudView& points,
    const Point&          scan_origin,
    const LVoxGridPtr&    grid,
    const LvoxOptions&    options
) -> void
{
    using dim = pdal::Dimension::Id;

    Logger logger{"Before"};

    const auto  core_count      = std::thread::hardware_concurrency();
    const Index point_count     = points->size();
    const Index points_per_core = std::ceil(static_cast<float>(point_count) / core_count);

    logger.debug(
        R"(
Core count      {}
Point count     {}
Point per core  {})",
        core_count,
        point_count,
        points_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        const auto                compute_ray_before = [&logger](
                                            const LVoxGridPtr&  grid,
                                            pdal::PointViewIter start_it,
                                            const Point&        scan_origin,
                                            Index               points_to_compute
                                        ) -> void {
            for (const auto& point : std::ranges::subrange(start_it, start_it + points_to_compute))
            {
                const Point pt{
                    point.getFieldAs<double>(dim::X),
                    point.getFieldAs<double>(dim::Y),
                    point.getFieldAs<double>(dim::Z)
                };

                const Vector beam_to_point{scan_origin - pt};

                Grid::traversal(
                    *grid,
                    Beam{scan_origin, beam_to_point},
                    [&grid](const Index3D& idxs, double t) mutable -> void {
                        const auto [x, y, z] = idxs;
                        grid->at(x, y, z) += 1;
                    },
                    beam_to_point.norm()
                );
            }

            logger.debug("Before thread finished");
        };

        auto start_it = points->begin();
        for (Index i = 0; i < core_count; ++i)
        {
            const Index points_to_compute = std::min(
                points_per_core, static_cast<Index>(std::distance(start_it, points->end()))
            );

            threads.emplace_back(
                compute_ray_before,
                std::ref(grid),
                start_it,
                std::ref(scan_origin),
                points_to_compute
            );
            start_it += points_to_compute;
        }
    }
}

auto compute_theoriticals(
    const std::vector<Beam>& beams, const LVoxGridPtr& grid, const LvoxOptions& options
) -> void
{
    using dim = pdal::Dimension::Id;

    using dim = pdal::Dimension::Id;
    Logger logger{"Compute theoriticals"};

    const auto  core_count     = std::thread::hardware_concurrency();
    const Index beam_count     = beams.size();
    const Index beams_per_core = std::ceil(static_cast<float>(beam_count) / core_count);

    logger.debug(
        R"(
Core count      {}
Beams count     {}
Beams per core  {})",
        core_count,
        beam_count,
        beams_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        const auto                compute_rays_from_scanner = [&logger](
                                                   const LVoxGridPtr&                grid,
                                                   std::vector<Beam>::const_iterator start_it,
                                                   Index beams_to_compute
                                               ) -> void {
            for (const auto& beam : std::ranges::subrange(start_it, start_it + beams_to_compute))
            {
                Grid::traversal(
                    *grid,
                    beam,
                    [&grid](const Index3D& idxs, double t) mutable -> void {
                        const auto [x, y, z] = idxs;
                        grid->at(x, y, z) += 1;
                    }
                );
            }

            logger.debug("Theoritical thread finished");
        };
        auto start_it = beams.begin();
        for (Index i = 0; i < core_count; ++i)
        {
            const Index beams_to_compute =
                std::min(beams_per_core, static_cast<Index>(std::distance(start_it, beams.end())));
            threads.emplace_back(
                compute_rays_from_scanner, std::ref(grid), start_it, beams_to_compute
            );

            start_it += beams_to_compute;
        }
    }
}

auto compute_beer_lambert(std::uint32_t theoriticals, std::uint32_t hits, std::uint32_t before)
    -> double
{
    const auto G = [](double val) -> double {
        return 0.5 * val;
    };
    const auto N   = theoriticals + before;
    const auto N_i = hits;
    const auto RDI = N_i / N;

    // TODO: implement
    return 0.;
}

auto compute_bias_corrected_maximum_likelihood_estimator(
    std::uint32_t theoriticals, std::uint32_t hits, std::uint32_t before
) -> double
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

    std::function<double(double, double, double)> pad_compute_method;
    switch (options.pad_computation_method)
    {
    case LvoxOptions::PADMethod::BeerLambert:
        pad_compute_method = compute_beer_lambert;
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
        const auto& scan         = scans[i];
        auto        theoriticals = std::make_unique<LVoxGrid>(total_bounds, options.voxel_size);
        auto        before       = std::make_unique<LVoxGrid>(total_bounds, options.voxel_size);
        auto        hits         = std::make_unique<LVoxGrid>(total_bounds, options.voxel_size);

        if (options.simulated_scanner)
        {
            logger.info("Compute theoriticals {}/{}", i + 1, scans.size());
            compute_theoriticals(scan->get_beams(), theoriticals, options);
        }

        logger.info("Compute hits {}/{}", i + 1, scans.size());
        compute_hits(scan->get_points(), hits, options);

        logger.info("Compute before {}/{}", i + 1, scans.size());

        // TODO: use GPS time
        compute_before(scan->get_points(), scan->get_scan_position({}), before, options);

        logger.info("Computing PAD", scans.size());
        // for (const auto& [idx, value] : before)
        // {
        //     pad_compute_method(theoriticals.at(idx), hits.at(idx), value);
        // }
    }

    return pda_result;
}

} // namespace lvox
