/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_PROPAGATIONVISITORCONTEXT_H
#define LVOX3_PROPAGATIONVISITORCONTEXT_H

#include "Eigen/Core"

/**
 * @struct PropagationVisitorContext
 * @brief This struct holds all the elements computed in the class LVOX3_Grid3DPropagationAlgorithm
 * for each cell that must be visited during the propagation algorithm.
 */
struct PropagationVisitorContext
{
    /**
     * @brief Constructor to initialize the PropagationVisitorContext.
     * @param cellIndex The index of the current cell to visit.
     * @param col The column of the current cell.
     * @param lin The line of the current cell.
     * @param level The level (z) of the current cell.
     * @param cellCenter The center coordinates of the current cell.
     * @param distance The distance between the first cell and the current cell.
     */
    PropagationVisitorContext(const size_t& cellIndex, const size_t& col, const size_t& lin, const size_t& level,
                              const Eigen::Vector3d& cellCenter, const double& distance)
        : m_cellIndex(cellIndex), m_col(col), m_lin(lin), m_level(level), m_cellCenter(cellCenter), m_distance(distance)
    {
    }

    const size_t& m_cellIndex;      /*!< The index of the current cell to visit. */
    const size_t& m_col;            /*!< The column of the current cell. */
    const size_t& m_lin;            /*!< The line of the current cell. */
    const size_t& m_level;          /*!< The level (z) of the current cell. */
    const Eigen::Vector3d& m_cellCenter; /*!< The center coordinates of the current cell. */
    const double& m_distance;       /*!< The distance between the first cell and the current cell. */
};

#endif // LVOX3_PROPAGATIONVISITORCONTEXT_H
