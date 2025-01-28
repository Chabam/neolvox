#ifndef LVOX_COMPUTETHEORITICALSTHREAD_H
#define LVOX_COMPUTETHEORITICALSTHREAD_H

#include <QThread>
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_tools/ct_monitoredqthread.h"

/*!
*  \brief Computes the "theoritical" grid of a point cloud and the distances associated (delta theoritical)
*
*  Each voxel of the resulting "hit" grid stores the number of theoritical rays from a given scanner intersecting this voxel
*  Each voxel of the resulting "deltaTheoritical" grid stores the average time spent by the rays in this voxel
*
*  \param _scanner : scanner that launches ray through the grid
*  \param _outputTheoriticalGrid : output theoritical grid
*  \param _outputDeltaTheoriticalGrid : output deltaTheoritical grid
*
*/
class LVOX_ComputeTheoriticalsThread : public CT_MonitoredQThread
{
    Q_OBJECT
public:
    LVOX_ComputeTheoriticalsThread(const CT_Scanner *scanner,
                                   CT_Grid3D<int> *outputTheoriticalGrid,
                                   CT_Grid3D<float> *outputDeltaTheoriticalGrid,
                                   bool computeDistance);

    void run();

private:
    const CT_Scanner*   _scanner;
    CT_Grid3D<int>*     _outputTheoriticalGrid;
    CT_Grid3D<float>*  _outputDeltaTheoriticalGrid;
    bool                _computeDistance;
};

#endif // LVOX_COMPUTETHEORITICALSTHREAD_H
