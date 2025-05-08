#ifndef LVOX_LVOX_HPP
#define LVOX_LVOX_HPP

#include <lvox/voxel/concrete_grid.hpp>

namespace lvox
{

class Scan;

struct LvoxOptions
{
    double voxel_size        = .5;
    bool   simulated_scanner = false;

    enum class PADMethod
    {
        BeerLambert,
        BiasCorrectedMaximumLikelihoodEstimator
    } pad_computation_method = PADMethod::BiasCorrectedMaximumLikelihoodEstimator;
};

using PadResult = lvox::DenseGrid<double>;

auto compute_pad(
    const std::vector<std::shared_ptr<lvox::Scan>>& scans, const LvoxOptions& options
) -> PadResult;

} // namespace lvox

#endif // LVOX_LVOX_HPP
