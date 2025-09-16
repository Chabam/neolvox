#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <lvox/types.hpp>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

class Scan;
class Beam;
class VoxelsMetrics;

namespace algorithms
{

struct ComputeOptions
{
    double                       m_voxel_size;
    unsigned int                 m_job_limit;
    pad_estimators::PADEstimator m_pad_estimator;
    bool                         m_compute_theoriticals;
    bool                         m_use_dense_grid;
};

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
auto compute_pad(const std::vector<Scan>& scans, const ComputeOptions& options) -> Grid;

} // namespace algorithms

} // namespace lvox
#endif // LVOX_ALGORITHMS_HPP
