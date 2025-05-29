#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <thread>

#include <lvox/types.hpp>
#include <lvox/voxel/concrete_grid.hpp>

namespace lvox
{

class Scan;

namespace algorithms
{

struct ComputeOptions
{
    double voxel_size        = .5;
    bool   simulated_scanner = false;
    int    job_limit         = std::thread::hardware_concurrency();

};

struct PADComputeOptions final : public ComputeOptions
{
    enum class PADMethod
    {
        BeerLambert,
        BiasCorrectedMaximumLikelihoodEstimator
    } pad_computation_method = PADMethod::BeerLambert;
};

using PadResult  = lvox::SparseGrid<std::atomic<double>>;
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
    const PointCloudView& points,
    const Point&          scan_origin,
    ComputeData&          data,
    const ComputeOptions& options
) -> void;

// Compute the rays count and length from a virtual scanner
auto compute_theoriticals(
    const std::vector<Beam>& beams, ComputeData& data, const ComputeOptions& options
) -> void;

//  Calls `compute_rays_count_and_length` plus the chosen PAD compute algorithm
auto compute_pad(
    const std::vector<std::shared_ptr<lvox::Scan>>& scans, const PADComputeOptions& options
) -> PadResult;

} // namespace algorithms

} // namespace lvox
#endif // LVOX_ALGORITHMS_HPP
