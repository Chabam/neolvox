#ifndef LVOX_COMPUTEBEFOREWITHLENGTHTHRESHOLDTHREAD_H
#define LVOX_COMPUTEBEFOREWITHLENGTHTHRESHOLDTHREAD_H

#include <QThread>
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_tools/ct_monitoredqthread.h"

class LVOX_ComputeBeforeWithLengthThresholdThread : public CT_MonitoredQThread
{
    Q_OBJECT
public:
    LVOX_ComputeBeforeWithLengthThresholdThread(const CT_Scanner *scanner,
                                                CT_Grid3D<int> *outputBeforeGrid,
                                                const CT_Scene *scene,
                                                double threshold);

    void run();

private:
    const CT_Scanner*   _scanner;
    CT_Grid3D<int>*     _outputBeforeGrid;
    const CT_Scene*     _scene;
    double              _threshold;
};

#endif // LVOX_COMPUTEBEFOREWITHLENGTHTHRESHOLDTHREAD_H
