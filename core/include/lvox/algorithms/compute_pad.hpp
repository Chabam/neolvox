#ifndef LVOX_COMPUTE_PAD_HPP
#define LVOX_COMPUTE_PAD_HPP

#include <vector>

namespace lvox
{

class COOGrid;
struct Scan;

namespace algorithms
{

namespace pad_estimators
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
struct BiasCorrectedMaximumLikelyhoodEstimator;
} // namespace pad_estimators

struct ComputeOptions;

class PADEstimation
{
  public:
    PADEstimation(COOGrid& grid, unsigned int required_hits);
    void operator()(algorithms::pad_estimators::BeerLambert);
    void operator()(algorithms::pad_estimators::ContactFrequency);
    void operator()(algorithms::pad_estimators::UnequalPathLengthBeerLambert);
    void operator()(algorithms::pad_estimators::BiasCorrectedMaximumLikelyhoodEstimator);

  private:
    COOGrid& m_grid;
    unsigned int m_required_hits;
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
COOGrid compute_pad(const std::vector<Scan>& scans, const ComputeOptions& options);

void compute_pad(COOGrid& grid, const ComputeOptions& options);

} // namespace algorithms

} // namespace lvox

#endif // LVOX_COMPUTE_PAD_HPP
