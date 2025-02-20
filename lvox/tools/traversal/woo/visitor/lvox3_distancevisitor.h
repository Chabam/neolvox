/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_DISTANCEVISITOR_H
#define LVOX3_DISTANCEVISITOR_H

#include "tools/lvox3_rayboxintersectionmath.h"
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_errorcode.h"

#include "tools/3dgrid/lvox_3dgriddefs.h"
#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"

template<typename T>
class LVOX3_DistanceVisitor : public CT_AbstractGrid3DBeamVisitor
{
public:
    LVOX3_DistanceVisitor(LVOX3_Grid3D<T>* grid,
                          lvox::MutexCollection* collection = nullptr) {
        m_grid = grid;
        m_multithreadCollection = collection;
        m_gridTools = NULL;

        if(grid != NULL)
            m_gridTools = new LVOX3_GridTools(grid);
    }

    ~LVOX3_DistanceVisitor() {
        delete m_gridTools;
    }

    /**
     * @brief Called when a voxel must be visited
     */
    void visit(const size_t &index, [[maybe_unused]] const CT_Beam *beam) {
        Eigen::Vector3d bot, top, nearInter, farInter;
        size_t col;
        size_t lin;
        size_t level;
        m_gridTools->computeColLinLevelForIndex(index, col, lin, level);
        m_gridTools->computeCellBottomLeftTopRightCornerAtColLinLevel(col,
                                                                      lin,
                                                                      level,
                                                                      bot,
                                                                      top);

        if (LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(bot, top, beam->origin(), beam->direction(), nearInter, farInter))
        {
            //qDebug()<<context.rayOrigin(0) << context.rayOrigin(1)<< context.rayOrigin(2) << context.rayDirection(0)<< context.rayDirection(1)<< context.rayDirection(2);

            const double distance = (nearInter - farInter).norm();
            //qDebug()<<"XXXXXXXXX:"<<nearInter(0) << nearInter(1)<< nearInter(2) << farInter(0)<< farInter(1)<< farInter(2) << distance;
            if(m_multithreadCollection != NULL) {
                QMutex* mutex = (*m_multithreadCollection)[index];
                mutex->lock();
                m_grid->doSommation(index,distance,distance);
                m_grid->addValueAtIndex(index, 1);

                mutex->unlock();
            } else {

                m_grid->doSommation(index,distance,distance);
                m_grid->addValueAtIndex(index, 1);

            }
        }
    }

private:
    LVOX3_Grid3D<T>*        m_grid;
    LVOX3_GridTools*        m_gridTools;
    lvox::MutexCollection*  m_multithreadCollection;
};

#endif // LVOX3_DISTANCEVISITOR_H
