#ifndef LVOX2_COMPUTEACTUALBEAMTHREAD_H
#define LVOX2_COMPUTEACTUALBEAMTHREAD_H

#include <QThread>
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_tools/ct_monitoredqthread.h"

class LVOX2_ComputeActualBeamThread : public CT_MonitoredQThread
{
    Q_OBJECT
public:
    LVOX2_ComputeActualBeamThread(const CT_Scanner *scanner,
                             CT_Grid3D<int> *outputActualBeamGrid,
                             CT_Grid3D<float> *outputDeltaActualBeamGrid,
                             const CT_Scene *scene,
                             bool computeDistance);

    void run();

private:
    const CT_Scanner*   _scanner;
    CT_Grid3D<int>*     _outputActualBeamGrid;
    CT_Grid3D<float>*  _outputDeltaActualBeamGrid;
    const CT_Scene*     _scene;
    bool                _computeDistance;
};

#endif // LVOX2_COMPUTEACTUALBEAMTHREAD_H
