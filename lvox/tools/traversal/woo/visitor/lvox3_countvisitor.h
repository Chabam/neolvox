/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COUNTVISITOR_H
#define LVOX3_COUNTVISITOR_H

#include "tools/lvox3_gridtype.h"

#include "tools/3dgrid/lvox_grid3d.h"
#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"

template<typename T>
class LVOX3_CountVisitor : public CT_AbstractGrid3DBeamVisitor
{
public:
    LVOX3_CountVisitor(LVOX_Grid3D<T>* grid,
                       lvox::MutexCollection* collection = nullptr) {
        m_grid = grid;
        m_multithreadCollection = collection;
    }

    /**
     * @brief Called when a voxel must be visited
     */
    virtual void visit(const size_t &index, [[maybe_unused]] const CT_Beam *beam) {
        if(m_multithreadCollection != NULL) {
            QMutex* mutex = (*m_multithreadCollection)[index];
            mutex->lock();
            m_grid->addValueAtIndex(index, 1);
            mutex->unlock();
        } else {
            m_grid->addValueAtIndex(index, 1);
        }
    }

private:
    LVOX_Grid3D<T>*        m_grid;
    lvox::MutexCollection*  m_multithreadCollection;
};

#endif // LVOX3_COUNTVISITOR_H
