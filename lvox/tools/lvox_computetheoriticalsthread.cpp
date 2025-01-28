#include "lvox_computetheoriticalsthread.h"
#include "qdebug.h"
#include "ct_itemdrawable/ct_beam.h"
#include "ct_itemdrawable/tools/gridtools/ct_grid3dwootraversalalgorithm.h"
#include "tools/lvox_distancevisitor.h"
#include "tools/lvox_countvisitor.h"

LVOX_ComputeTheoriticalsThread::LVOX_ComputeTheoriticalsThread(const CT_Scanner *scanner,
                                                               CT_Grid3D<int> *outputTheoriticalGrid,
                                                               CT_Grid3D<float> *outputDeltaTheoriticalGrid,
                                                               bool computeDistance) : CT_MonitoredQThread()
{
    _scanner = scanner;
    _outputTheoriticalGrid = outputTheoriticalGrid;
    _outputDeltaTheoriticalGrid = outputDeltaTheoriticalGrid;
    _computeDistance = computeDistance;
}

void LVOX_ComputeTheoriticalsThread::run()
{
    qDebug() << "Début de LVOX_ComputeTheoriticalsThread / ScanId=" << _scanner->getScanID();
    Eigen::Vector3d bot, top;
    _outputTheoriticalGrid->getMinCoordinates(bot);
    _outputTheoriticalGrid->getMaxCoordinates(top);

    // Creates the ray traversal algorithm

    CT_ShootingPattern *pattern = _scanner->getShootingPattern();
    size_t n = pattern->getNumberOfShots();

    // Creates visitors
    QList<CT_AbstractGrid3DBeamVisitor*> list;

    LVOX_CountVisitor countVisitor(_outputTheoriticalGrid);
    list.append(&countVisitor);

    if (_computeDistance)
    {
        LVOX_DistanceVisitor distVisitor(_outputDeltaTheoriticalGrid);
        list.append(&distVisitor);
    }

    // Creates traversal algorithm
    CT_Grid3DWooTraversalAlgorithm algo(_outputTheoriticalGrid, true, list);

    CT_Beam beam(NULL, NULL);

    int progressStep = n / 1000;

    qDebug() << "number of shots=" << n;
    // For all theoritical rays of the scanner
    for (size_t i = 0; i < n; i++) {
        // Get the next ray
        _scanner->beam(i, beam);

        if (beam.intersect(bot, top)) {
            algo.compute(beam);
        }

        if (i % progressStep == 0) {
            _progress = 100*i/n;
            emit progressChanged();
        }
    }

    // Don't forget to calculate min and max in order to visualize it as a colored map
    _outputTheoriticalGrid->computeMinMax();

    if (_computeDistance)
    {
        // To get the mean distance we have to divide in each voxel the sum of distances by the number of hits
        for (size_t i = 0 ; i < _outputTheoriticalGrid->nCells() ; i++ )
        {
            if ( _outputTheoriticalGrid->valueAtIndex(i) == 0 )
            {
                _outputDeltaTheoriticalGrid->setValueAtIndex(i,-1);
            } else
            {
                _outputDeltaTheoriticalGrid->setValueAtIndex(i, _outputDeltaTheoriticalGrid->valueAtIndex(i)/(float)_outputTheoriticalGrid->valueAtIndex(i));
            }
        }
        _outputDeltaTheoriticalGrid->computeMinMax();
    }

    _progress = 100;
    emit progressChanged();
    qDebug() << "Fin de LVOX_ComputeTheoriticalsThread / ScanId=" << _scanner->getScanID();
}
