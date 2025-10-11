/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_PROPAGATIONVISITORCONTEXT_H
#define LVOX3_PROPAGATIONVISITORCONTEXT_H

#include "Eigen/Core"

/**
 * @brief This is all elements computed in class LVOX3_Grid3DPropagationAlgorithm for
 *        each cell that must be visited
 */
struct LVOX3_PropagationVisitorContext {
    LVOX3_PropagationVisitorContext(const size_t& cellIndex,
                                    const size_t& col,
                                    const size_t& lin,
                                    const size_t& level,
                                    const Eigen::Vector3d& cellCenter,
                                    const double& distance) : m_cellIndex(cellIndex),
                                                                         m_col(col),
                                                                         m_lin(lin),
                                                                         m_level(level),
                                                                         m_cellCenter(cellCenter),
                                                                         m_distance(distance)
    {

    }

    const size_t& m_cellIndex;
    const size_t& m_col;
    const size_t& m_lin;
    const size_t& m_level;
    const Eigen::Vector3d& m_cellCenter;
    const double& m_distance;
};

#endif // LVOX3_PROPAGATIONVISITORCONTEXT_H
