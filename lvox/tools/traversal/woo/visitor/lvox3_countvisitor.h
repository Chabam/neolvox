/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COUNTVISITOR_H
#define LVOX3_COUNTVISITOR_H

#include "lvox3_grid3dvoxelwoovisitor.h"
#include "tools/lvox3_gridtype.h"

#include "tools/3dgrid/lvox_3dgriddefs.h"

template<typename T>
class LVOX3_CountVisitor : public LVOX3_Grid3DVoxelWooVisitor
{
public:
    LVOX3_CountVisitor(const LVOX3_Grid3D<T>* grid,
                       const lvox::MutexCollection* collection = NULL) {
        m_grid = (LVOX3_Grid3D<T>*)grid;

        m_multithreadCollection = (lvox::MutexCollection*)collection;
    }

    /**
     * @brief Called when a voxel must be visited
     */
    void visit(const LVOX3_Grid3DVoxelWooVisitorContext& context) {
        if(m_multithreadCollection != NULL) {
            QMutex* mutex = (*m_multithreadCollection)[context.currentVoxelIndex];
            mutex->lock();
            m_grid->addValueAtIndex(context.currentVoxelIndex, 1);
            mutex->unlock();
        } else {
            m_grid->addValueAtIndex(context.currentVoxelIndex, 1);
        }
    }


private:

    LVOX3_Grid3D<T>*        m_grid;
    lvox::MutexCollection*  m_multithreadCollection;
};

#endif // LVOX3_COUNTVISITOR_H
