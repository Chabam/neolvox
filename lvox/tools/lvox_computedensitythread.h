#ifndef LVOX_COMPUTEDENSITYTHREAD_H
#define LVOX_COMPUTEDENSITYTHREAD_H

#include "ct_tools/ct_monitoredqthread.h"
#include "ct_itemdrawable/ct_grid3d.h"

class LVOX_ComputeDensityThread : public CT_MonitoredQThread
{
    Q_OBJECT
public:
    LVOX_ComputeDensityThread(CT_Grid3D<float> *densityGrid,
                              CT_Grid3D<int> *hitsGrid,
                              CT_Grid3D<int> *theoriticalGrid,
                              CT_Grid3D<int> *beforeGrid,
                              int effectiveRayThresh);

    void run();

private:
    CT_Grid3D<float>*  _densityGrid;
    CT_Grid3D<int>*     _hitsGrid;
    CT_Grid3D<int>*     _theoriticalGrid;
    CT_Grid3D<int>*     _beforeGrid;
    int                 _effectiveRayThresh;
};

#endif // LVOX_COMPUTEDENSITYTHREAD_H
