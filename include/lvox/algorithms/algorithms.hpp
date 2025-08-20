#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/types.hpp>

namespace lvox
{

class Scan;
class Beam;
class Grid;
class VoxelsMetrics;

namespace algorithms
{

struct ComputeOptions
{
    double                       m_voxel_size;
    unsigned int                 m_job_limit;
    pad_estimators::PADEstimator m_pad_estimator;
    bool                         m_compute_theoriticals;
};

// Compute which voxels the rays have went to in a voxel grid and
// computes the length that the rays travelled inside each of them.
[[nodiscard]]
auto compute_rays_count_and_length(
    const Grid& grid, const Scan& scan, const ComputeOptions& options
) -> VoxelsMetrics;

//  Wrapper for the whole PAD computation. Does the following:
//
// - Compute the scene bounds
// - Creates a compute data struct
// - For each scans
//   - Compute the rays length and count in each voxels (with `compute_rays_count_and_length`)
//   - Compute with the beams from a virtual scanner if requested
//   - Compute the PAD values for each voxels using the values from the computed grids
// - Averages the PAD values from every scans
[[nodiscard]]
auto compute_pad(const std::vector<Scan>& scans, const ComputeOptions& options) -> VoxelsMetrics;

[[nodiscard]]
auto compute_scene_bounds(const std::vector<Scan>& scans) -> lvox::Bounds;

} // namespace algorithms

} // namespace lvox
#endif // LVOX_ALGORITHMS_HPP
