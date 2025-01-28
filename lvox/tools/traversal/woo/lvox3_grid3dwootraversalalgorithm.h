/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GRID3DWOOTRAVERSALALGORITHM_H
#define LVOX3_GRID3DWOOTRAVERSALALGORITHM_H

#include <QElapsedTimer>

#include "tools/3dgrid/lvox3_grid3d.h"
#include "ct_itemdrawable/tools/scanner/ct_shot.h"
#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"

#include "tools/lvox3_errorcode.h"
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_rayboxintersectionmath.h"
#include "tools/traversal/woo/visitor/lvox3_grid3dvoxelwoovisitor.h"

/**
 * @brief Use this class to propagate a shot in cells of 3D grid and do
 *        anything for each cell touch by the shot
 */
template<typename T>
class LVOX3_Grid3DWooTraversalAlgorithm
{
public:
    LVOX3_Grid3DWooTraversalAlgorithm(const LVOX3_Grid3D<T>* grid,
                                      bool visitFirstVoxelTouched,
                                      QVector<LVOX3_Grid3DVoxelWooVisitor*>& list)
    {
        m_visitorList = list;
        m_numberOfVisitors = list.size();

        m_grid = grid;
        m_grid->boundingBox(m_gridBottom, m_gridTop);

        m_gridResolution(0) = grid->xresolution();
        m_gridResolution(1) = grid->yresolution();
        m_gridResolution(2) = grid->zresolution();
        m_visitFirstVoxelTouched = visitFirstVoxelTouched;

        m_chooseAxis[0] = 2;
        m_chooseAxis[1] = 1;
        m_chooseAxis[2] = 2;
        m_chooseAxis[3] = 1;
        m_chooseAxis[4] = 2;
        m_chooseAxis[5] = 2;
        m_chooseAxis[6] = 0;
        m_chooseAxis[7] = 0;

        m_gridTools = new LVOX3_GridTools(grid);
        timeInit = 0;
        timeTraverse = 0;
        m_timer.start();
    }

    ~LVOX3_Grid3DWooTraversalAlgorithm()
    {
        delete m_gridTools;
    }

    bool isRayIntersecting(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction) {
        Eigen::Vector3d start, end;
        return LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(m_gridBottom, m_gridTop, origin, direction, start, end);
    }

    void compute(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction)
    {
        qint64 t0, t1, t2;
        Eigen::Vector3d start, end;

        if (!LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(
                    m_gridBottom, m_gridTop, origin, direction, start, end))
            return;
        m_timer.restart();
        t0 = m_timer.nsecsElapsed();
        LVOX3_Grid3DVoxelWooVisitorContext context(origin, direction);
        context.nearImpactPointWithGrid = start;
        context.farImpactPointWithGrid = end;

        Eigen::Vector3d stepAxis, boundary, tMax, tDel;
        m_gridTools->computeGridIndexForPoint(context.nearImpactPointWithGrid, context.colLinLevel.x(), context.colLinLevel.y(), context.colLinLevel.z(), context.currentVoxelIndex);

        for (int i = 0 ; i < 3 ; ++i) {
            if(context.farImpactPointWithGrid(i) > context.nearImpactPointWithGrid(i)) {
                stepAxis(i) = 1;
                boundary(i) = ((context.colLinLevel(i)+1)*m_gridResolution(i)) + m_gridBottom(i);
            } else {
                stepAxis(i) = -1;
                boundary(i) = (context.colLinLevel(i)*m_gridResolution(i)) + m_gridBottom(i);
            }

            if(direction(i) != 0) {
                tMax(i) = fabs((boundary(i) - context.nearImpactPointWithGrid(i)) / direction(i));
                tDel(i) = fabs(m_gridResolution(i) / direction(i));
            } else {
                tMax(i) = MAX_DOUBLE_VALUE;
                tDel(i) = MAX_DOUBLE_VALUE;
            }
        }
        t1 = m_timer.nsecsElapsed();
        if (m_visitFirstVoxelTouched)
        {
            if(!lvox::FilterCode::isFiltered(m_grid->valueAtIndex(context.currentVoxelIndex))) {
                for (int i = 0 ; i < m_numberOfVisitors ; ++i)
                    m_visitorList.at(i)->visit(context);
            } else {
                return;
            }
        }
        while ( 1 )
        {            
            // Finds along which axis to do the next step
            const quint8 bits =	(( tMax(0) < tMax(1) ) << 2) +
                                (( tMax(0) < tMax(2) ) << 1) +
                                (( tMax(1) < tMax(2) ));
            const quint8 nextStepAxis = m_chooseAxis[bits];
            context.colLinLevel(nextStepAxis) += stepAxis(nextStepAxis);

            if (context.colLinLevel.x() >= m_grid->xdim()) { return; }
            if (context.colLinLevel.y() >= m_grid->ydim()) { return; }
            if (context.colLinLevel.z() >= m_grid->zdim()) { return; }

            // Add the index of the voxel to the list
            m_gridTools->computeGridIndexForColLinLevel(context.colLinLevel.x(), context.colLinLevel.y(), context.colLinLevel.z(), context.currentVoxelIndex);
            if(!lvox::FilterCode::isFiltered(m_grid->valueAtIndex(context.currentVoxelIndex))) {
                for (int i = 0 ; i < m_numberOfVisitors ; ++i)
                    m_visitorList.at(i)->visit(context);
            } else {
                return;
            }

            // Updating tmax of this axis (increasing by deltaT)
            tMax(nextStepAxis) = tMax(nextStepAxis) + tDel(nextStepAxis);
        }
        t2 = m_timer.nsecsElapsed();

        timeInit += t1 - t0;
        timeTraverse += t2 - t1;
    }

    qint64 timeInit;
    qint64 timeTraverse;

private:
    LVOX3_GridTools*                            m_gridTools;
    const LVOX3_Grid3D<T>*                      m_grid;
    Eigen::Vector3d                             m_gridBottom;
    Eigen::Vector3d                             m_gridTop;
    Eigen::AlignedBox3d                         m_box;
    Eigen::Vector3d                             m_gridResolution;
    bool                                        m_visitFirstVoxelTouched;
    QVector<LVOX3_Grid3DVoxelWooVisitor* >      m_visitorList;
    int                                         m_numberOfVisitors;
    quint8                                      m_chooseAxis[8];
    QElapsedTimer                               m_timer;

    static const double MAX_DOUBLE_VALUE;
};

template<typename T>
const double LVOX3_Grid3DWooTraversalAlgorithm<T>::MAX_DOUBLE_VALUE = std::numeric_limits<double>::max();

#endif // LVOX3_GRID3DWOOTRAVERSALALGORITHM_H
