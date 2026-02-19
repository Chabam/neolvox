#ifndef LVOX_COMPUTE_PAD_HPP
#define LVOX_COMPUTE_PAD_HPP

#include <vector>

#include <lvox/algorithms/compute_options.hpp>
#include <lvox/algorithms/compute_scene_bounds.hpp>
#include <lvox/algorithms/explore_grid.hpp>
#include <lvox/logger/logger.hpp>
#include <lvox/scanner/scan.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/coo_grid.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

namespace algorithms
{

class PADEstimation
{
  public:
    PADEstimation(COOGrid& grid, unsigned int required_hits);
    void operator()(algorithms::pad_estimators::BeerLambert);
    void operator()(algorithms::pad_estimators::ContactFrequency);
    void operator()(algorithms::pad_estimators::UnequalPathLengthBeerLambert);
    void operator()(algorithms::pad_estimators::BiasCorrectedMaximumLikelyhoodEstimator);

  private:
    COOGrid&     m_grid;
    unsigned int m_required_counts;
};

void compute_pad(COOGrid& grid, const ComputeOptions& options);

//  Wrapper for the whole PAD computation. Does the following:
//
// - Compute the scene bounds
// - Creates a compute data struct
// - For each scans
//   - Compute the rays length and count in each voxels (with `compute_rays_count_and_length`)
//   - Compute with the beams from a virtual scanner if requested
//   - Compute the PAD values for each voxels using the values from the computed grids
// - Averages the PAD values from every scans
template <Point PointT, PointCloud<PointT> PointCloudT>
[[nodiscard]]
COOGrid compute_pad(
    const std::vector<Scan<PointT, PointCloudT>>& scans, const ComputeOptions& options
)
{
    namespace pe = lvox::algorithms::pad_estimators;

    Logger logger{"LVOX"};
    logger.info("Scan count {}", scans.size());

    const bool uses_variance =
        std::holds_alternative<pe::UnequalPathLengthBeerLambert>(options.m_pad_estimator);

    Grid grid = std::invoke([&]() -> Grid {
        Bounds<double> bounds;

        if (options.m_bounds)
            bounds = *options.m_bounds;
        else
            bounds = compute_scene_bounds(scans);

        if (options.m_use_sparse_grid)
            return ChunkedGrid{bounds, options.m_voxel_size, uses_variance};
        else
            return DenseGrid{bounds, options.m_voxel_size, uses_variance};
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

} // namespace algorithms

} // namespace lvox

#endif // LVOX_COMPUTE_PAD_HPP
