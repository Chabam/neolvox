#include <thread>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/lvox.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox
{

auto compute_hits(
    const std::shared_ptr<lvox::Scan>& scan, const Bounds bounds, const LvoxOptions& options
) -> lvox::ThreadSafeDenseGridU32i
{
    using dim = pdal::Dimension::Id;
    Logger logger{"Hits"};

    ThreadSafeDenseGridU32i hits{bounds, options.voxel_size};

    const auto   core_count      = std::thread::hardware_concurrency();
    const auto&  points          = scan->get_points();
    const Index point_count     = points->size();
    const Index points_per_core = std::ceil(static_cast<float>(point_count) / core_count);

    logger.debug(
        R"(
Point count      {}
Point count     {}
Point per core  {})",
        core_count,
        point_count,
        points_per_core
    );

    // IMPORTANT: must be scoped in order for jthreads to be automatically join
    {
        std::vector<std::jthread> threads;
        auto                      start_it           = points->begin();
        const auto                find_point_in_grid = [&logger](
                                            ThreadSafeDenseGridU32i& grid,
                                            pdal::PointViewIter      start_it,
                                            Index                   points_to_compute
                                        ) {
            for (const auto& point : std::ranges::subrange(start_it, start_it + points_to_compute))
            {
                const auto [x, y, z] = grid.index_of_point({
                    point.getFieldAs<double>(dim::X),
                    point.getFieldAs<double>(dim::Y),
                    point.getFieldAs<double>(dim::Z),
                });
                grid.at(x, y, z) += 1;
            }
            logger.debug("Hits thread finished");
        };

        for (Index i = 0; i < core_count; ++i)
        {
            const Index points_to_compute = std::min(
                points_per_core, static_cast<Index>(std::distance(start_it, points->end()))
            );
            threads.emplace_back(find_point_in_grid, std::ref(hits), start_it, points_to_compute);
            start_it += points_to_compute;
        }
    }

    return hits;
}

auto compute_before(
    const std::shared_ptr<lvox::Scan>& scan, const Bounds bounds, const LvoxOptions& options
) -> lvox::ThreadSafeDenseGridU32i
{
    using before_result = ThreadSafeDenseGridU32i;
    using dim           = pdal::Dimension::Id;

    ThreadSafeDenseGridU32i before{bounds, options.voxel_size};

    Logger logger{"Before"};

    // TODO: use GPS time
    Point scan_origin = scan->get_scan_position({});

    const auto   core_count      = std::thread::hardware_concurrency();
    const auto&  points          = scan->get_points();
    const Index point_count     = points->size();
    const Index points_per_core = std::ceil(static_cast<float>(point_count) / core_count);

    logger.debug(
        R"(
Point count      {}
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
                                            ThreadSafeDenseGridU32i& grid,
                                            pdal::PointViewIter      start_it,
                                            const Point&             scan_origin,
                                            Index                   points_to_compute
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
                    grid,
                    Beam{scan_origin, beam_to_point},
                    [&grid](const Index3D& idxs, double t) mutable -> void {
                        const auto [x, y, z] = idxs;
                        grid.at(x, y, z) += 1;
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
                std::ref(before),
                start_it,
                std::ref(scan_origin),
                points_to_compute
            );
            start_it += points_to_compute;
        }
    }

    return before;
}

auto compute_theoriticals(
    const std::shared_ptr<lvox::Scan>& scan, const Bounds bounds, const LvoxOptions& options
) -> lvox::ThreadSafeDenseGridU32i
{
    using dim = pdal::Dimension::Id;

    using dim = pdal::Dimension::Id;
    Logger logger{"Compute theoriticals"};

    ThreadSafeDenseGridU32i theoriticals{bounds, options.voxel_size};

    const auto   core_count     = std::thread::hardware_concurrency();
    const auto&  beams          = scan->get_beams();
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
                                                   ThreadSafeDenseGridU32i&          grid,
                                                   std::vector<Beam>::const_iterator start_it,
                                                   Index beams_to_compute
                                               ) -> void {
            for (const auto& beam : std::ranges::subrange(start_it, start_it + beams_to_compute))
            {
                Grid::traversal(
                    grid,
                    beam,
                    [&grid](const Index3D& idxs, double t) mutable -> void {
                        const auto [x, y, z] = idxs;
                        grid.at(x, y, z) += 1;
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
                compute_rays_from_scanner, std::ref(theoriticals), start_it, beams_to_compute
            );

            start_it += beams_to_compute;
        }
    }

    return theoriticals;
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
        const auto&             scan = scans[i];
        ThreadSafeDenseGridU32i theoriticals;

        if (options.simulated_scanner)
        {
            logger.info("Compute theoriticals {}/{}", i + 1, scans.size());
            theoriticals = compute_theoriticals(scan, total_bounds, options);
        }
        else
        {
            theoriticals = ThreadSafeDenseGridU32i{total_bounds, options.voxel_size};
        }

        logger.info("Compute hits {}/{}", i + 1, scans.size());
        ThreadSafeDenseGridU32i hits{compute_hits(scan, total_bounds, options)};

        logger.info("Compute before {}/{}", i + 1, scans.size());
        ThreadSafeDenseGridU32i before{compute_before(scan, total_bounds, options)};

        for (Index i = 0; i < theoriticals.cell_count(); ++i)
        {
            pda_result.at(i) += pad_compute_method(theoriticals.at(i), hits.at(i), before.at(i));
        }
    }

    return pda_result;
}

} // namespace lvox
