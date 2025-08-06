#include <lvox/algorithms/pad_compute_methods.hpp>
#include <lvox/algorithms/algorithms.hpp>

namespace lvox::algorithms::pad_compute_methods
{

BeerLambert::BeerLambert(const Bounds& bounds, const ComputeOptions& options)
    : m_hits{bounds, options.voxel_size},
      m_counts{bounds, options.voxel_size},
      m_lengths{bounds, options.voxel_size}
{
}

ContactFrequency::ContactFrequency(const Bounds& bounds, const ComputeOptions& options)
    : m_hits{bounds, options.voxel_size},
      m_counts{bounds, options.voxel_size},
      m_lengths{bounds, options.voxel_size}
{
}

UnequalPathLengthBeerLambert::UnequalPathLengthBeerLambert(const Bounds& bounds, const ComputeOptions& options)
    : m_hits{bounds, options.voxel_size},
      m_counts{bounds, options.voxel_size},
      m_effective_lengths{bounds, options.voxel_size},
      m_effective_lengths_variance{bounds, options.voxel_size}
{
}


}
