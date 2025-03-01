/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_DISTANCEVISITOR_H
#define LVOX3_DISTANCEVISITOR_H

#include "lvox3_grid3dvoxelwoovisitor.h"
#include "tools/lvox3_rayboxintersectionmath.h"
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_errorcode.h"

#include "tools/3dgrid/lvox3_grid3d.h"

template<typename T>
class LVOX3_DistanceVisitor : public LVOX3_Grid3DVoxelWooVisitor
{
public:
    LVOX3_DistanceVisitor(const LVOX3_Grid3D<T>* grid,
                          const lvox::MutexCollection* collection = NULL) {
        m_grid = (LVOX3_Grid3D<T>*)grid;
        m_multithreadCollection = (lvox::MutexCollection*)collection;
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
    void visit(const LVOX3_Grid3DVoxelWooVisitorContext& context) {        
        Eigen::Vector3d bot, top, nearInter, farInter;
        m_gridTools->computeCellBottomLeftTopRightCornerAtColLinLevel(context.colLinLevel.x(),
                                                                      context.colLinLevel.y(),
                                                                      context.colLinLevel.z(),
                                                                      bot,
                                                                      top);

        if (LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(bot, top, context.rayOrigin, context.rayDirection, nearInter, farInter))
        {
            //qDebug()<<context.rayOrigin(0) << context.rayOrigin(1)<< context.rayOrigin(2) << context.rayDirection(0)<< context.rayDirection(1)<< context.rayDirection(2);

            const double distance = (nearInter - farInter).norm();
            //qDebug()<<"XXXXXXXXX:"<<nearInter(0) << nearInter(1)<< nearInter(2) << farInter(0)<< farInter(1)<< farInter(2) << distance;
            if(m_multithreadCollection != NULL) {
                QMutex* mutex = (*m_multithreadCollection)[context.currentVoxelIndex];
                mutex->lock();                
                m_grid->doSommation(context.currentVoxelIndex,distance,distance);
                m_grid->addValueAtIndex(context.currentVoxelIndex, 1);

                mutex->unlock();
            } else {

                m_grid->doSommation(context.currentVoxelIndex,distance,distance);
                m_grid->addValueAtIndex(context.currentVoxelIndex, 1);

            }
        }
    }

private:
    LVOX3_Grid3D<T>*           m_grid;
    LVOX3_GridTools*        m_gridTools;
    lvox::MutexCollection*  m_multithreadCollection;
};

#endif // LVOX3_DISTANCEVISITOR_H
