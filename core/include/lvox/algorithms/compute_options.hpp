#ifndef LVOX_ALGORITHMS_HPP
#define LVOX_ALGORITHMS_HPP

#include <lvox/algorithms/pad_estimators.hpp>

namespace lvox::algorithms
{

struct ComputeOptions
{
    double                       m_voxel_size;
    unsigned int                 m_job_limit;
    pad_estimators::PADEstimator m_pad_estimator;
    bool                         m_compute_theoriticals;
    bool                         m_use_sparse_grid;
    unsigned int                 m_required_hits;
    std::ostream&                m_log_stream;
};

} // namespace lvox::algorithms

#endif // LVOX_ALGORITHMS_HPP
