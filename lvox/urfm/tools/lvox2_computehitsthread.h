#ifndef LVOX2_COMPUTEHITSTHREAD_H
#define LVOX2_COMPUTEHITSTHREAD_H

#include <QThread>
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_tools/ct_monitoredqthread.h"

/*!
 * \brief Computes the "hit" grid of a point cloud
 *
 * Each voxel of the resulting grid stores the number of hits inside itself
 *
 * \param _scanner scanner that launches ray through the grid
 * \param _grilleHits Output grid for hits number
 * \param _grilleIn Output grid for in length
 * \param _grilleOut Output grid for out length
 * \param _scene point cloud to take into account while computing the grid
 * \param _intensityThresh minimum or maximum intensity for a hit to contribute to the results
 * \param _greaterThan if true, only consider intensity greater than the threshold, otherwise only consider intensity below that threshold
 */
class LVOX2_ComputeHitsThread : public CT_MonitoredQThread
{
    Q_OBJECT
public:
    LVOX2_ComputeHitsThread(const CT_Scanner *scanner,
                           CT_Grid3D<int> *grilleHits,
                           CT_Grid3D<float> *grilleIn,
                           CT_Grid3D<float> *grilleOut,
                           const CT_Scene *scene,
                           bool computeDistance,bool cylindricFilter,
                            double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);

    void run();

private:
    const CT_Scanner*   _scanner;
    CT_Grid3D<int>*     _grilleHits;
    CT_Grid3D<float>*  _grilleIn;
    CT_Grid3D<float>*  _grilleOut;
    const CT_Scene*     _scene;
    bool                _computeDistance;
     bool                _cylindricFilter;
     double _xMin;
     double _xMax;
     double _yMin;
     double _yMax;
     double _zMin;
     double _zMax;
};

#endif // LVOX2_COMPUTEHITSTHREAD_H
