#include <atomic>
#include <numbers>
#include <ranges>
#include <thread>

#include <pdal/Dimension.hpp>
#include <pdal/PointView.hpp>

#include <lvox/algorithms/algorithms.hpp>
#include <lvox/algorithms/compute_pad_visitor.hpp>
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

struct ComputeDistance
{
    ComputeDistance(const VoxelHitInfo& voxel_hit_info, std::uint32_t current_count)
        : m_voxel_hit_info{voxel_hit_info}
        , m_current_count{current_count}
    {
    }

    auto operator()(LengthGrid& lengths) const -> void
    {
        const auto [x, y, z] = m_voxel_hit_info.m_index;
        lengths.at(x, y, z).fetch_add(
            m_voxel_hit_info.m_distance_in_voxel, std::memory_order_relaxed
        );
    }

    auto operator()(EffectiveLengthsWithVariance& lengths_and_variance) -> void
    {
        const auto [x, y, z]         = m_voxel_hit_info.m_index;
        const double length_in_voxel = m_voxel_hit_info.m_distance_in_voxel;
        auto&        length_sum_ref  = lengths_and_variance.m_effective_lengths.at(x, y, z);
        auto&        variance_ref = lengths_and_variance.m_effective_lengths_variance.at(x, y, z);

        // TODO: make this configurable maybe?
        // So far it's based on Computree's NeedleFromDimension
        constexpr double elem_length   = 0.06;
        constexpr double elem_diameter = 0.02;
        constexpr double attenuation_coeff =
            (2. * std::numbers::pi * elem_length * elem_diameter) / 4.;

        const double attenuated_length =
            -(std::log(1. - attenuation_coeff * length_in_voxel) / attenuation_coeff);

        length_sum_ref += attenuated_length;

        // Best effort for calculating the variance,
        // might be off because this line definitely
        // represents multiple instructions.
        variance_ref +=
            std::pow(attenuated_length - (length_sum_ref / m_current_count), 2) / m_current_count;
    }
    const VoxelHitInfo& m_voxel_hit_info;
    std::uint32_t       m_current_count;
};

auto compute_rays_count_and_length(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    Logger logger{"Compute ray counts and length"};

    const auto ray_trace = [&logger, &scan](ComputeData& data, auto&& points) -> void {
        auto grid_traveral = GridTraversalVoxelRounding{data.m_counts};
        for (const auto& timed_point : points)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            const Vector beam_to_point{pt - scan_origin};

            const auto [x, y, z] = data.m_hits.index_of_point(pt);
            data.m_hits.at(x, y, z).fetch_add(1, std::memory_order_relaxed);

            grid_traveral(
                Beam{scan_origin, beam_to_point},
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                    const auto [x, y, z] = voxel_hit_info.m_index;
                    std::visit(
                        ComputeDistance{
                            voxel_hit_info,
                            data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed)
                        },
                        data.m_lengths
                    );
                },
                beam_to_point.norm()
            );
        }

        logger.debug("thread finished");
    };
    compute_in_parallel(*scan.m_points, data, ray_trace, options.m_job_limit);
}

auto compute_theoriticals(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void
{
    Logger     logger{"Compute theoriticals"};
    const auto compute_rays_from_scanner = [&logger, &scan](ComputeData& data, auto&& blanks) -> void {
        auto grid_traveral = GridTraversalVoxelRounding{data.m_counts};
        for (const auto& timed_point : blanks)
        {
            const double gps_time = timed_point.m_gps_time;
            const Point& pt       = timed_point.m_point;
            const Point  scan_origin =
                std::visit(Scan::ComputeBeamOrigin{timed_point.m_gps_time}, scan.m_scanner_origin);

            const Vector beam_to_point{pt - scan_origin};

            grid_traveral(
                Beam{scan_origin, beam_to_point},
                [&data](const VoxelHitInfo& voxel_hit_info) mutable -> void {
                const auto [x, y, z] = voxel_hit_info.m_index;
                std::visit(
                    ComputeDistance{
                        voxel_hit_info,
                        data.m_counts.at(x, y, z).fetch_add(1, std::memory_order_relaxed)
                    },
                    data.m_lengths
                );
            });
        }

        logger.debug("thread finished");
    };

    compute_in_parallel(**scan.m_blank_shots, data, compute_rays_from_scanner, options.m_job_limit);
}

struct CreateLengthGrids
{
    CreateLengthGrids(const Bounds& bounds, double voxel_size)
        : m_bounds{bounds}
        , m_voxel_size{voxel_size}
    {
    }

    auto operator()(pe::BeerLambert) const -> DistanceGrids
    {
        return LengthGrid{m_bounds, m_voxel_size};
    }

    auto operator()(pe::ContactFrequency) const -> DistanceGrids
    {
        return LengthGrid{m_bounds, m_voxel_size};
    }

    auto operator()(pe::UnequalPathLengthBeerLambert) const -> DistanceGrids
    {
        return EffectiveLengthsWithVariance{
            LengthGrid{m_bounds, m_voxel_size}, LengthGrid{m_bounds, m_voxel_size}
        };
    }

    const Bounds& m_bounds;
    double        m_voxel_size;
};

auto compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options) -> PadResult
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const Bounds total_bounds = compute_scene_bounds(scans);

    PadResult pad_result{total_bounds, options.m_voxel_size};

    for (const auto [scan_num, scan] : std::views::enumerate(scans))
    {
        ComputeData data{
            .m_counts  = CountGrid{total_bounds, options.m_voxel_size},
            .m_hits    = CountGrid{total_bounds, options.m_voxel_size},
            .m_lengths = std::visit(
                CreateLengthGrids{total_bounds, options.m_voxel_size},
                options.m_pad_estimator
            )
        };

        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num + 1, scans.size());
            compute_theoriticals(scan, data, options);
        }

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
                // Weighted sum of the PAD to avoid doing the average afterwards
                pad_result.at(idx).fetch_add(
                    std::visit(ComputePADVisitor{data, idx}, options.m_pad_estimator) /
                        static_cast<double>(scan_count),
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
