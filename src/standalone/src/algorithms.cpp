#include <atomic>
#include <ranges>
#include <stdexcept>
#include <thread>
#include <variant>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/compute_grid_counts_and_lengths.hpp>
#include <lvox/algorithms/grid_traversal.hpp>
#include <lvox/algorithms/pad_compute_methods.hpp>
#include <lvox/scanner/beam.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/grid.hpp>

#include "lvox/algorithms/compute_pad.hpp"

namespace lvox::algorithms
{

struct GetCountsGrid
{
    auto operator()(pcm::BeerLambert& bl) const -> const decltype(bl.m_counts)&
    {
        return bl.m_counts;
    }
    auto operator()(pcm::ContactFrequency& cf) const -> const decltype(cf.m_counts)&
    {
        return cf.m_counts;
    }
    auto operator()(pcm::UnequalPathLengthBeerLambert& uplbl) const -> const
        decltype(uplbl.m_counts)&
    {
        return uplbl.m_counts;
    }
};

struct GetHitsGrid
{
    auto operator()(pcm::BeerLambert& bl) const -> decltype(bl.m_hits)& { return bl.m_hits; }
    auto operator()(pcm::ContactFrequency& cf) const -> decltype(cf.m_hits)& { return cf.m_hits; }
    auto operator()(pcm::UnequalPathLengthBeerLambert& uplbl) const -> decltype(uplbl.m_hits)&
    {
        return uplbl.m_hits;
    }
};

auto create_pad_compute_method(const Bounds& bounds, ComputeOptions options)
    -> lvox::algorithms::pad_compute_methods::PadComputeMethod
{
    switch (options.pad_computation_method)
    {
    case ComputeOptions::PADMethod::BeerLambert:
        return pad_compute_methods::BeerLambert{bounds, options};
    case ComputeOptions::PADMethod::ContactFrequency:
        return pad_compute_methods::ContactFrequency{bounds, options};
    case ComputeOptions::PADMethod::UnequalPathLengthBeerLambert:
        return pad_compute_methods::UnequalPathLengthBeerLambert{bounds, options};
    default:
        throw std::runtime_error{"Unsupported PAD Computation method provided"};
    }
}

template <std::ranges::range R, typename Func>
auto compute_in_parallel(const R& objects, Func&& func, unsigned int job_count)
{
    const size_t object_count     = std::distance(std::begin(objects), std::end(objects));
    const size_t objects_per_core = std::ceil(static_cast<float>(object_count) / job_count);

    std::vector<std::jthread> threads;
    for (auto chunk : objects | std::views::chunk(objects_per_core))
    {
        threads.emplace_back(
            [&func](auto&& chunk) {
                for (const auto& object : chunk)
                {
                    func(object);
                }
            },
            std::move(chunk)
        );
    }
}

auto compute_rays_count_and_length(
    const Scan& scan, pad_compute_methods::PadComputeMethod& pcm, const ComputeOptions& options
) -> void
{
    const auto do_grid_traversal = [&scan, &pcm](const auto& timed_point) {
        const double gps_time = timed_point.m_gps_time;
        const Point& pt       = timed_point.m_point;
        const Point  scan_origin =
            std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

        const Vector beam_to_point{scan_origin - pt};

        auto& hits_grid      = std::visit(GetHitsGrid{}, pcm);
        const auto [x, y, z] = hits_grid.index_of_point(pt);
        hits_grid.at(x, y, z).fetch_add(1, std::memory_order_relaxed);

        struct TraversalVisitor
        {
            auto operator()(pad_compute_methods::BeerLambert& bl) -> void
            {
                GridTraversalVoxelRounding{bl.m_counts, m_pcm}(m_beam, m_max_distance);
            }
            auto operator()(pad_compute_methods::ContactFrequency& cf) -> void
            {
                GridTraversalVoxelRounding{cf.m_counts, m_pcm}(m_beam, m_max_distance);
            }
            auto operator()(pad_compute_methods::UnequalPathLengthBeerLambert& uplbl) -> void
            {
                GridTraversalExactDistance{uplbl.m_counts, m_pcm}(m_beam, m_max_distance);
            }

            const Beam& m_beam;
            double      m_max_distance;
            pad_compute_methods::PadComputeMethod& m_pcm;
        };

        std::visit(TraversalVisitor{Beam{pt, beam_to_point}, beam_to_point.norm(), pcm}, pcm);
    };

    compute_in_parallel(*scan.m_points, do_grid_traversal, options.job_limit);
}

// auto compute_theoriticals(
//     const std::vector<Beam>& beams, pad_compute_methods::PadComputeMethod& pcm, const
//     ComputeOptions& options
// ) -> void
// {
//     Logger logger{"Compute theoriticals"};
//     const auto compute_rays_from_scanner = [&logger, &pcm](const auto& beams) -> void {
//         auto grid_traveral = GridTraversalVoxelRounding{std::visit(GetCountsGrid{}, pcm), pcm};
//         for (const auto& beam : beams)
//         {
//             grid_traveral(beam, [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
//                 const auto [x, y, z] = voxel_hit_info.m_index;
//                 data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed);
//                 data.m_lengths.at(x, y, z).fetch_add(
//                     voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed
//                 );
//             });
//         }

//         logger.debug("thread finished");
//     };

//     compute_in_parallel(beams, compute_rays_from_scanner, options.job_limit);
// }

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Bounds total_bounds = compute_scene_bounds(scans);
    auto         pad_method   = create_pad_compute_method(total_bounds, options);

    ComputePAD compute_pad{total_bounds, options};
    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {

        // if (options.theoritical_scanner)
        // {
        //     logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
        //     compute_theoriticals(options.theoritical_scanner->get_beams(), pad_method, options);
        // }

        logger.info("Compute ray counts and length {}/{}", scan_num + 1, scans.size());

        compute_rays_count_and_length(scan, pad_method, options);

        const auto indexes_of_cells_with_data =
            std::visit(GetCountsGrid{}, pad_method) | std::views::enumerate |
            std::views::filter([](const auto& pair) {
                // TODO: Make the treshold configurable
                return std::get<1>(pair) >= 5;
            }) |
            std::views::keys | std::ranges::to<std::vector<Index>>();

        const auto do_pad_computation = [&compute_pad, &pad_method](auto&& indexes) {
            for (const auto& index : indexes)
            {
                std::visit(compute_pad, pad_method, std::variant<Index>{index});
            }
        };

        compute_in_parallel(indexes_of_cells_with_data, do_pad_computation, options.job_limit);
    }

    return compute_pad.m_res;
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
