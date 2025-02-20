/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GRIDTOOLS_H
#define LVOX3_GRIDTOOLS_H

#include "Eigen/Core"
#include "3dgrid/lvox_grid3d.h"

class LVOX3_GridTools {
public:
    LVOX3_GridTools(const CT_AbstractGrid3D* grid) {
        grid->getMinCoordinates(m_gridBBOXMin);
        m_gridDimX = grid->xdim();
        m_gridDimXMultDimY = m_gridDimX * grid->ydim();
        // TODO: cleanup (FC)
        m_gridResolutionX = grid->resolution();
        m_gridResolutionY = grid->resolution();
        m_gridResolutionZ = grid->resolution();
        m_gridResolutionXDiv2 = m_gridResolutionX/2.0;;
        m_gridResolutionYDiv2 = m_gridResolutionY/2.0;;
        m_gridResolutionZDiv2 = m_gridResolutionZ/2.0;;
    }

    inline void computeGridIndexForPoint(const Eigen::Vector3d& point,
                                         size_t& indice) {
        const size_t col = computeColLinLevel(m_gridBBOXMin.x(), m_gridResolutionX, point.x());
        const size_t lin = computeColLinLevel(m_gridBBOXMin.y(), m_gridResolutionY, point.y());
        const size_t level = computeColLinLevel(m_gridBBOXMin.z(), m_gridResolutionZ, point.z());

        indice = level*m_gridDimXMultDimY + lin*m_gridDimX + col;
    }

    inline void computeGridIndexForPoint(const Eigen::Vector3d& point,
                                         size_t& col,
                                         size_t& lin,
                                         size_t& level,
                                         size_t& indice) {
        col = computeColLinLevel(m_gridBBOXMin.x(), m_gridResolutionX, point.x());
        lin = computeColLinLevel(m_gridBBOXMin.y(), m_gridResolutionY, point.y());
        level = computeColLinLevel(m_gridBBOXMin.z(), m_gridResolutionZ, point.z());

        indice = (level * m_gridDimXMultDimY + lin*m_gridDimX + col);
    }

    inline void computeGridIndexForColLinLevel(const size_t& col,
                                               const size_t& lin,
                                               const size_t& level,
                                               size_t& indice) {
        indice = level*m_gridDimXMultDimY + lin*m_gridDimX + col;
    }

    inline void computeCellBottomLeftCornerAtPoint(const Eigen::Vector3d& point,
                                                   Eigen::Vector3d& bottomLeftCorner) {

        computeCellBottomLeftCornerAtColLinLevel(computeColLinLevel(m_gridBBOXMin.x(), m_gridResolutionX, point.x()),
                                                 computeColLinLevel(m_gridBBOXMin.y(), m_gridResolutionY, point.y()),
                                                 computeColLinLevel(m_gridBBOXMin.z(), m_gridResolutionZ, point.z()),
                                                 bottomLeftCorner);
    }

    inline void computeCellBottomLeftCornerAtColLinLevel(const size_t& col,
                                                         const size_t& lin,
                                                         const size_t& level,
                                                         Eigen::Vector3d& bottomLeftCorner) {
        bottomLeftCorner.x() = m_gridBBOXMin.x() + col * m_gridResolutionX;
        bottomLeftCorner.y() = m_gridBBOXMin.y() + lin * m_gridResolutionY;
        bottomLeftCorner.z() = m_gridBBOXMin.z() + level * m_gridResolutionZ;
    }

    inline void computeCellBottomLeftTopRightCornerAtColLinLevel(const size_t& col,
                                                                 const size_t& lin,
                                                                 const size_t& level,
                                                                 Eigen::Vector3d& bottomLeftCorner,
                                                                 Eigen::Vector3d& topRightCorner) {

        computeCellBottomLeftCornerAtColLinLevel(col, lin, level, bottomLeftCorner);
        topRightCorner.x() = bottomLeftCorner.x() + m_gridResolutionX;
        topRightCorner.y() = bottomLeftCorner.y() + m_gridResolutionY;
        topRightCorner.z() = bottomLeftCorner.z() + m_gridResolutionZ;
    }

    inline void computeCellTopMiddleCoordsAtIndex(const size_t& index,
                                                  Eigen::Vector3d& coords) {
        size_t col, lin, level;

        computeColLinLevelForIndex(index, col, lin, level);
        computeCellBottomLeftCornerAtColLinLevel(col, lin, level, coords);

        coords.x() += m_gridResolutionXDiv2;
        coords.y() += m_gridResolutionYDiv2;
        coords.z() += m_gridResolutionZ;
    }

    inline void computeCellCenterCoordsAtColLinLevel(const size_t& col,
                                                     const size_t& lin,
                                                     const size_t& level,
                                                     Eigen::Vector3d& coords) {
        computeCellBottomLeftCornerAtColLinLevel(col, lin, level, coords);

        coords.x() += m_gridResolutionXDiv2;
        coords.x() += m_gridResolutionYDiv2;
        coords.x() += m_gridResolutionZDiv2;
    }

    inline void computeColLinLevelForIndex(const size_t& index,
                                             size_t& col,
                                             size_t& lin,
                                             size_t& level) {
        lin = 0;
        level = 0;

        if(index >= m_gridDimXMultDimY)
            level = index/m_gridDimXMultDimY;

        if(index >= m_gridDimX)
            lin = (index - (level*m_gridDimXMultDimY))/m_gridDimX;

        col = index - (level*m_gridDimXMultDimY) - (lin*m_gridDimX);
    }

    static inline size_t computeColLinLevel(const double& min,
                                            const double& gridResolution,
                                            const double& val) {

        return (size_t) std::floor((val - min) / gridResolution);
    }

private:
    Eigen::Vector3d m_gridBBOXMin;
    double          m_gridDimX;
    double          m_gridDimXMultDimY;
    double          m_gridResolutionX;
    double          m_gridResolutionY;
    double          m_gridResolutionZ;
    double          m_gridResolutionXDiv2;
    double          m_gridResolutionYDiv2;
    double          m_gridResolutionZDiv2;
};

#endif // LVOX3_GRIDTOOLS_H
