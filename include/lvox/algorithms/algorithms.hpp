#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <algorithm>
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

struct Voxels
{
    auto insert(std::pair<Index3D, PadComputeData> value) -> void
    {
        auto lower = std::lower_bound(
            m_values.begin(), m_values.end(), value.first, [this](const auto& lhs, const auto& rhs) -> bool {
                const auto to_flat_index = [this](const Index3D& idx) {
                    const auto [x, y, z] = idx;
                    return x + y * m_grid.dim_x() + z * m_grid.dim_x() * m_grid.dim_y();
                };

                return to_flat_index(lhs.first) < to_flat_index(rhs);
            }
        );

        if (lower == m_values.end())
        {
            m_values.push_back(value);
            return;
        }

        auto& existing_voxel = lower->second;
        existing_voxel.m_count += value.second.m_count;
        existing_voxel.m_lengths += value.second.m_lengths;
    }

    std::vector<std::pair<Index3D, PadComputeData>> m_values;
    Grid m_grid;
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


using VisitedVoxels = Voxels;
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
