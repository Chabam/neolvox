#include "lvox3_mergegridsworker.h"

#include <QDebug>

LVOX3_MergeGridsWorker::LVOX3_MergeGridsWorker(LVOXGridSet *merged,
                    QVector<LVOXGridSet *> *gs, VoxelReducer *reducer) :
    m_merged(merged), m_gs(gs), m_reducer(reducer)
{
}

void LVOX3_MergeGridsWorker::doTheJob()
{
    qDebug() << "LVOX3_MergeGridWorker::doTheJob()";

    size_t work = m_merged->rd->nCells();
    setProgressRange(0, work);
    //Merges the grids together(m_merged is the first grid set, m_gs is the vector of grid sets and the reducer is the type of merging)
    LVOX3_MergeGrids::apply(m_merged, m_gs, m_reducer,
        [&](const size_t &i) {
            this->setProgress(i);
            return this->mustCancel();
        }
    );
    setProgress(work);
}
