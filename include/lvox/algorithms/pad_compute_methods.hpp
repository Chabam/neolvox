#ifndef LVOX_PAD_COMPUTE_METHODS
#define LVOX_PAD_COMPUTE_METHODS

#include <lvox/types.hpp>

namespace lvox::algorithms
{

struct ComputeData;

namespace pad_compute_methods
{

struct BeerLambert
{
    static auto operator()(const algorithms::ComputeData&, const Index& index) -> double;
};

struct ContactFrequency
{
    static auto operator()(const algorithms::ComputeData&, const Index& index) -> double;
};

struct UnequalPathLengthBeerLambert
{
    static auto operator()(const algorithms::ComputeData&, const Index& index) -> double;
};


} // namespace pad_compute_methods

} // namespace lvox::algorithms

#endif // LVOX_PAD_COMPUTE_METHODS
