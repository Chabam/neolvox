#ifndef LVOX_COMPUTE_PAD
#define LVOX_COMPUTE_PAD

#include <lvox/types.hpp>
#include <lvox/algorithms/algorithms.hpp>

namespace lvox::algorithms
{

namespace pad_compute_methods
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
}

namespace pcm = lvox::algorithms::pad_compute_methods;

struct ComputePAD
{
    ComputePAD(lvox::Bounds bounds, const ComputeOptions& options);

    auto operator()(const pcm::BeerLambert& bl, Index index) -> void;
    auto operator()(const pcm::ContactFrequency& cf, Index index) -> void;
    auto operator()(const pcm::UnequalPathLengthBeerLambert& uplbl, Index index) -> void;

    PadResult m_res;
};

}

#endif // LVOX_COMPUTE_PAD
