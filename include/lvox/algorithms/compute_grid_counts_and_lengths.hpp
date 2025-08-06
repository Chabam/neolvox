#ifndef LVOX_COMPUTE_GRID_COUNTS_AND_LENGTHS
#define LVOX_COMPUTE_GRID_COUNTS_AND_LENGTHS

#include <lvox/types.hpp>
#include <lvox/scanner/scan.hpp>

namespace lvox::algorithms
{

namespace pad_compute_methods
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
} // namespace pad_compute_methods

struct VoxelHitInfo;

namespace pcm = lvox::algorithms::pad_compute_methods;

struct ComputeGridCountsAndLengths
{
    ComputeGridCountsAndLengths(const VoxelHitInfo& hit_info);

    auto operator()(pcm::BeerLambert& bl) const -> void;
    auto operator()(pcm::ContactFrequency& cf) const -> void;
    auto operator()(pcm::UnequalPathLengthBeerLambert& uplbl) const -> void;

  private:
    const VoxelHitInfo& m_hit_info;
};

}

#endif // LVOX_COMPUTE_GRID_COUNTS_AND_LENGTHS
