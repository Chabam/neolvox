#include <algorithm>
#include <execution>
#include <format>

#include <lvox/algorithms/compute_pad.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/voxel/coo_grid.hpp>

namespace lvox::algorithms
{

ComputePAD::ComputePAD(COOGrid& grid)
    : m_grid{grid}
{
}

void ComputePAD::operator()(algorithms::pe::BeerLambert)
{
    std::for_each(
        std::execution::par, m_grid.begin(), m_grid.end(), [this](COOGrid::VoxelData voxel_view) -> void {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double       hits            = *voxel_view.hit;
            const unsigned int ray_count       = *voxel_view.count;
            const double       RDI             = hits / static_cast<double>(ray_count);
            const double       ray_length      = *voxel_view.lengths;
            const double       mean_ray_length = ray_length / ray_count;

            *voxel_view.pad = -(std::log(1. - RDI) / G(mean_ray_length));
        }
    );
}

void ComputePAD::operator()(algorithms::pe::ContactFrequency)
{
    std::for_each(
        std::execution::par, m_grid.begin(), m_grid.end(), [this](COOGrid::VoxelData voxel_view) -> void {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double       hits       = *voxel_view.hit;
            const unsigned int ray_count  = *voxel_view.count;
            const double       RDI        = hits / static_cast<double>(ray_count);
            const double       ray_length = *voxel_view.lengths;

            *voxel_view.pad = RDI / G(ray_length);
        }
    );
}

void ComputePAD::operator()(algorithms::pe::UnequalPathLengthBeerLambert)
{
    std::for_each(
        std::execution::par,
        m_grid.begin(),
        m_grid.end(),
        [this](COOGrid::VoxelData voxel_view) -> void {
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
        }
    );
}

void ComputePAD::operator()(algorithms::pe::BiasCorrectedMaximumLikelyhoodEstimator)
{
    std::for_each(
        std::execution::par,
        m_grid.begin(),
        m_grid.end(),
        [this](COOGrid::VoxelData voxel_view) -> void {
            const auto G = [](double val) -> double {
                return 0.5 * val;
            };

            const double       hits        = *voxel_view.hit;
            const unsigned int ray_count   = *voxel_view.count;
            const double       hits_length = *voxel_view.hits_length;
            const double       lengths     = *voxel_view.lengths;

            double res = (1. / G(lengths)) * (hits - (hits_length / lengths));

            *voxel_view.pad = res;
        }
    );
}
} // namespace lvox::algorithms
