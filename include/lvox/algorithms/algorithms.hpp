#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>
#include <lvox/scanner/spherical_scanner.hpp>

namespace lvox
{

class Scan;
class Beam;

namespace algorithms
{

struct ComputeOptions
{
    double          voxel_size;
    unsigned int    job_limit;

    enum class PADMethod
    {
        BeerLambert,
        ContactFrequency
    } pad_computation_method;

    std::optional<SphericalScanner> theoritical_scanner;

};

using PadResult  = GridD;
using CountGrid  = GridU32;
using LengthGrid = GridD;

struct ComputeData
{
    CountGrid                m_counts;
    LengthGrid               m_lengths;
    std::optional<CountGrid> m_hits;
};

// Compute which voxels the rays have went to in a voxel grid and
// computes the length that the rays travelled inside each of them.
auto compute_rays_count_and_length(
    const Scan& scan, ComputeData& data, const ComputeOptions& options
) -> void;

// Compute the rays count and length from a virtual scanner
auto compute_theoriticals(
    const std::vector<Beam>& scan, ComputeData& data, const ComputeOptions& options
) -> void;

//  Wrapper for the whole PAD computation. Does the following:
//
// - Compute the scene bounds
// - Creates a compute data struct
// - For each scans
//   - Compute the rays length and count in each voxels (with `compute_rays_count_and_length`)
//   - Compute with the beams from a virtual scanner if requested
//   - Compute the PAD values for each voxels using the values from the computed grids
// - Averages the PAD values from every scans
auto compute_pad(
    const std::vector<Scan>& scans, const ComputeOptions& options
) -> PadResult;

auto compute_scene_bounds(const std::vector<Scan>& scans) -> lvox::Bounds;

} // namespace algorithms

} // namespace lvox
#endif // LVOX_ALGORITHMS_HPP
