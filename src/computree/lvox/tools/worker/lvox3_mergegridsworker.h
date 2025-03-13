#ifndef LVOX3_MERGEGRIDSWORKER_H
#define LVOX3_MERGEGRIDSWORKER_H

#include "tools/worker/lvox3_worker.h"
#include "tools/lvox3_mergegrids.h"

class LVOX3_MergeGridsWorker : public LVOX3_Worker
{
public:
    LVOX3_MergeGridsWorker(LVOXGridSet *merged,
                           QVector<LVOXGridSet*> *gs,
                           VoxelReducer *reducer);

    void doTheJob();

private:
    LVOXGridSet *m_merged;
    QVector<LVOXGridSet *> *m_gs;
    VoxelReducer *m_reducer;
};

#endif // LVOX3_MERGEGRIDSWORKER_H
