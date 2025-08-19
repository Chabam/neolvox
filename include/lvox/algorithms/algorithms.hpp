#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <unordered_set>
#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/scanner/spherical_scanner.hpp>
#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox
{

class Scan;
class Beam;

namespace algorithms
{

struct ComputeOptions
{
    double                       m_voxel_size;
    unsigned int                 m_job_limit;
    pad_estimators::PADEstimator m_pad_estimator;
    bool                         m_compute_theoriticals;
};

struct PadComputeData
{
    unsigned int m_count;
    double       m_lengths;
    double       m_length_variance;
};

struct Index3DHash
{
    auto operator()(const Index3D& index) const -> size_t
    {
        auto [x, y, z] = index;
        const auto hash = std::hash<Index>{};

        return hash(x) + hash(y) + hash(z);
    }
};


using VisitedVoxels = std::unordered_map<Index3D, PadComputeData, Index3DHash>;
using Hits          = std::unordered_map<Index3D, unsigned int, Index3DHash>;
using PadResult     = std::unordered_map<Index3D, double, Index3DHash>;

// Compute which voxels the rays have went to in a voxel grid and
// computes the length that the rays travelled inside each of them.
[[nodiscard]]
auto compute_rays_count_and_length(
    const Grid& grid, const Scan& scan, const ComputeOptions& options
) -> VisitedVoxels;

[[nodiscard]]
auto compute_hits(const Grid& grid, const Scan& scan, const ComputeOptions& options) -> Hits;

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
auto compute_pad(const std::vector<Scan>& scans, const ComputeOptions& options) -> PadResult;

[[nodiscard]]
auto compute_scene_bounds(const std::vector<Scan>& scans) -> lvox::Bounds;

} // namespace algorithms

} // namespace lvox
#endif // LVOX_ALGORITHMS_HPP
