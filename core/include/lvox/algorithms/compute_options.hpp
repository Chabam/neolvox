#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <optional>

#include <lvox/algorithms/pad_estimators.hpp>
#include <lvox/voxel/bounds.hpp>

namespace lvox::algorithms
{

struct ComputeOptions
{
    double                              m_voxel_size;
    unsigned int                        m_job_limit;
    pad_estimators::PADEstimator        m_pad_estimator;
    bool                                m_compute_theoriticals;
    bool                                m_use_sparse_grid;
    unsigned int                        m_required_counts;
    double                              m_smallest_element_area;
    std::optional<lvox::Bounds<double>> m_bounds;
    std::ostream&                       m_log_stream;
};

} // namespace lvox::algorithms

#endif // LVOX_ALGORITHMS_HPP
