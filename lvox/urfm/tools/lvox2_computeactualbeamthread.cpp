#include "lvox2_computeactualbeamthread.h"
#include "ct_itemdrawable/ct_beam.h"
#include "ct_itemdrawable/tools/gridtools/ct_grid3dwootraversalalgorithm.h"
#include "tools/lvox_distancevisitor.h"
#include "tools/lvox_countvisitor.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "ct_iterator/ct_pointiterator.h"

LVOX2_ComputeActualBeamThread::LVOX2_ComputeActualBeamThread(const CT_Scanner *scanner,
                                                   CT_Grid3D<int> *outputActualBeamGrid,
                                                   CT_Grid3D<float> *outputDeltaActualBeamGrid,
                                                   const CT_Scene *scene,
                                                   bool computeDistance) : CT_MonitoredQThread()

{
    _scanner = scanner;
    _outputActualBeamGrid = outputActualBeamGrid;
    _outputDeltaActualBeamGrid = outputDeltaActualBeamGrid;
    _scene = scene;
    _computeDistance = computeDistance;
}

void LVOX2_ComputeActualBeamThread::run()
{
    qDebug() << "Début de LVOX2_ComputeActualBeamThread / ScanId=" << _scanner->getScanID();
    const CT_AbstractPointCloudIndex *pointCloudIndex = _scene->getPointCloudIndex();
    size_t n_points = pointCloudIndex->size();

    Eigen::Vector3d bot, top;
    _outputActualBeamGrid->getMinCoordinates(bot);
    _outputActualBeamGrid->getMaxCoordinates(top);

    // Creates visitors
    QList<CT_AbstractGrid3DBeamVisitor*> list;

    LVOX_CountVisitor countVisitor(_outputActualBeamGrid);
    list.append(&countVisitor);

    if (_computeDistance)
    {
        LVOX_DistanceVisitor distVisitor(_outputDeltaActualBeamGrid);
        list.append(&distVisitor);
    }

    // Creates traversal algorithm
    CT_Grid3DWooTraversalAlgorithm algo(_outputActualBeamGrid, false, list);

    CT_Beam beam(NULL, NULL);

    size_t progressStep = n_points / 20;
    size_t i = 0;

    CT_PointIterator itP(pointCloudIndex);
    while (itP.hasNext())
    {
        ++i;
        const CT_Point &point = itP.next().currentPoint();

        // Get the next ray
        beam.setOrigin(_scanner->getPosition());
        beam.setDirection(point-_scanner->getPosition());

        if (beam.intersect(bot, top))
        {
            algo.compute(beam);
        }

        if (i % progressStep == 0)
        {
            _progress = 100*i/n_points;
            emit progressChanged();
        }
    }

    // Don't forget to calculate min and max in order to visualize it as a colored map
    _outputActualBeamGrid->computeMinMax();

    if (_computeDistance)
    {
        // To get the mean distance we have to divide in each voxel the sum of distances by the number of hits
        for (int i = 0 ; i < _outputActualBeamGrid->nCells() ; i++ )
        {
            if ( _outputActualBeamGrid->valueAtIndex(i) == 0 )
            {
                _outputDeltaActualBeamGrid->setValueAtIndex(i,-1);
            } else
            {
                _outputDeltaActualBeamGrid->setValueAtIndex(i, _outputDeltaActualBeamGrid->valueAtIndex(i)/(float)_outputDeltaActualBeamGrid->valueAtIndex(i));
            }
        }
        _outputDeltaActualBeamGrid->computeMinMax();
    }

    _progress = 100;
    emit progressChanged();
    qDebug() << "Fin de LVOX2_ComputeActualBeamThread / ScanId=" << _scanner->getScanID();
}
