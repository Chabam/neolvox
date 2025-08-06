#ifndef LVOX_PAD_COMPUTE_METHODS
#define LVOX_PAD_COMPUTE_METHODS

#include <lvox/types.hpp>
#include <lvox/voxel/grid.hpp>

namespace lvox::algorithms
{

struct ComputeOptions;

namespace pad_compute_methods
{

struct BeerLambert
{
    BeerLambert(const Bounds& bounds, const ComputeOptions& options);

    GridU32 m_hits;
    GridU32 m_counts;
    GridD m_lengths;
};

struct ContactFrequency
{
    ContactFrequency(const Bounds& bounds, const ComputeOptions& options);

    GridU32 m_hits;
    GridU32 m_counts;
    GridD m_lengths;
};

struct UnequalPathLengthBeerLambert
{
    UnequalPathLengthBeerLambert(const Bounds& bounds, const ComputeOptions& options);

    GridU32 m_hits;
    GridU32 m_counts;
    GridD m_effective_lengths;
    GridD m_effective_lengths_variance;
};

using PadComputeMethod = std::variant<BeerLambert, ContactFrequency, UnequalPathLengthBeerLambert>;

} // namespace pad_compute_methods

} // namespace lvox::algorithms

#endif // LVOX_PAD_COMPUTE_METHODS
