#include <algorithm>
#include <execution>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/compute_scene_bounds.hpp>
#include <lvox/algorithms/explore_grid.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/voxel/coo_grid.hpp>
#include <lvox/voxel/grid.hpp>
#include <ranges>

namespace lvox::algorithms
{

template <typename PADEstimationFunc>
void estimate_pad_impl(COOGrid& grid, unsigned int required_hits, PADEstimationFunc&& func)
{
    auto filtered_grid = grid | std::views::filter([required_hits](COOGrid::VoxelData voxel_view) {
                             return *voxel_view.hit >= required_hits;
                         });
    std::for_each(std::execution::par, filtered_grid.begin(), filtered_grid.end(), func);
}

PADEstimation::PADEstimation(COOGrid& grid, unsigned int required_hits)
    : m_grid{grid}
    , m_required_hits{required_hits}
{
}

void PADEstimation::operator()(algorithms::pe::BeerLambert)
{
    estimate_pad_impl(m_grid, m_required_hits, [this](COOGrid::VoxelData voxel_view) -> void {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double       hits            = *voxel_view.hit;
        const unsigned int ray_count       = *voxel_view.count;
        const double       RDI             = hits / static_cast<double>(ray_count);
        const double       ray_length      = *voxel_view.lengths;
        const double       mean_ray_length = ray_length / ray_count;

        *voxel_view.pad = -(std::log(1. - RDI) / G(mean_ray_length));
    });
}

void PADEstimation::operator()(algorithms::pe::ContactFrequency)
{
    estimate_pad_impl(m_grid, m_required_hits, [this](COOGrid::VoxelData voxel_view) -> void {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double       hits       = *voxel_view.hit;
        const unsigned int ray_count  = *voxel_view.count;
        const double       RDI        = hits / static_cast<double>(ray_count);
        const double       ray_length = *voxel_view.lengths;

        *voxel_view.pad = RDI / G(ray_length);
    });
}

void PADEstimation::operator()(algorithms::pe::UnequalPathLengthBeerLambert)
{
    estimate_pad_impl(m_grid, m_required_hits, [this](COOGrid::VoxelData voxel_view) -> void {
        Logger     logger{"ComputePAD UPLBL"};
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double       hits               = *voxel_view.hit;
        const unsigned int ray_count          = *voxel_view.count;
        const double       RDI                = hits / static_cast<double>(ray_count);
        const double       ray_length         = *voxel_view.lengths;
        const double       variance           = *voxel_view.lengths_variance;
        const double       mean_ray_length    = ray_length / ray_count;
        const double       unequal_path_ratio = variance / mean_ray_length;

        double attenuation_coeff;

        if (RDI < 1.)
        {
            const double inv_RDI = 1. - RDI;
            attenuation_coeff =
                (1. / mean_ray_length) * (std::log(inv_RDI) - (1. / (2 * ray_count * inv_RDI)));
        }
        else // RDI == 1.
        {
            attenuation_coeff = std::log(2 * ray_count + 2) / mean_ray_length;
        }

        const double res = (1. / G(attenuation_coeff)) *
                           (1. - std::sqrt(1. - 2 * unequal_path_ratio * attenuation_coeff));

        if (std::isnan(res))
        {
            logger.debug(
                R"(
    hits                = {}
    ray_count           = {}
    RDI                 = {}
    ray_length          = {}
    mean_ray_length     = {}
    ray_length_variance = {}
    unequal_path_ratio  = {}
    attenuation_coeff   = {})",
                hits,
                ray_count,
                RDI,
                ray_length,
                mean_ray_length,
                variance,
                unequal_path_ratio,
                attenuation_coeff
            );
        }

        *voxel_view.pad = res;
    });
}

void PADEstimation::operator()(algorithms::pe::BiasCorrectedMaximumLikelyhoodEstimator)
{
    estimate_pad_impl(m_grid, m_required_hits, [this](COOGrid::VoxelData voxel_view) -> void {
        const auto G = [](double val) -> double {
            return 0.5 * val;
        };

        const double       hits        = *voxel_view.hit;
        const unsigned int ray_count   = *voxel_view.count;
        const double       hits_length = *voxel_view.hits_length;
        const double       lengths     = *voxel_view.lengths;

        double res = (1. / G(lengths)) * (hits - (hits_length / lengths));

        *voxel_view.pad = res;
    });
}

void compute_pad(COOGrid& grid, const ComputeOptions& options)
{
    std::visit(PADEstimation{grid, options.m_required_hits}, options.m_pad_estimator);
}

COOGrid compute_pad(const std::vector<lvox::Scan>& scans, const ComputeOptions& options)
{
    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    Grid grid = std::invoke([&]() -> Grid {
        if (options.m_use_sparse_grid)
            return ChunkedGrid{compute_scene_bounds(scans), options.m_voxel_size, uses_variance};
        else
            return DenseGrid{compute_scene_bounds(scans), options.m_voxel_size, uses_variance};
    });

    auto scan_num = 1;
    for (const auto& scan : scans)
    {
        if (options.m_compute_theoriticals && scan.m_blank_shots)
        {
            logger.info("Compute theoriticals {}/{}", scan_num, scans.size());
            explore_grid_theoriticals(grid, scan, options);
        }

        logger.info("Compute ray counts and length {}/{}", scan_num, scans.size());
        explore_grid(grid, scan, options);
        ++scan_num;
    }

    COOGrid result = std::visit(
        [](const auto& grid) -> COOGrid {
            return COOGrid{grid};
        },
        grid
    );

    logger.info("Estimating PAD", scan_num, scans.size());
    compute_pad(result, options);

    return result;
}
} // namespace lvox::algorithms
