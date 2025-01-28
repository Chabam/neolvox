#include "lvox_computebeforewithlengththresholdthread.h"
#include "ct_itemdrawable/ct_beam.h"
#include "ct_itemdrawable/tools/gridtools/ct_grid3dwootraversalalgorithm.h"
#include "tools/lvox_distancevisitor.h"
#include "tools/lvox_countwithlengththresholdvisitor.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "ct_iterator/ct_pointiterator.h"

LVOX_ComputeBeforeWithLengthThresholdThread::LVOX_ComputeBeforeWithLengthThresholdThread(const CT_Scanner *scanner,
                                                                                         CT_Grid3D<int> *outputBeforeGrid,
                                                                                         const CT_Scene *scene,
                                                                                         double threshold) : CT_MonitoredQThread()
{
    _scanner = scanner;
    _outputBeforeGrid = outputBeforeGrid;
    _scene = scene;
    _threshold = threshold;
}

void LVOX_ComputeBeforeWithLengthThresholdThread::run()
{
    qDebug() << "Début de LVOX_ComputeBeforeWithLengthThresholdThread / ScanId=" << _scanner->getScanID();
    const CT_AbstractPointCloudIndex *pointCloudIndex = _scene->getPointCloudIndex();
    size_t n_points = pointCloudIndex->size();

    Eigen::Vector3d bot, top;
    _outputBeforeGrid->getMinCoordinates(bot);
    _outputBeforeGrid->getMaxCoordinates(top);

    // Creates visitors
    QList<CT_AbstractGrid3DBeamVisitor*> list;

    LVOX_CountWithLengthThresholdVisitor countVisitor(_outputBeforeGrid, _threshold);
    list.append(&countVisitor);

    // Creates traversal algorithm
    CT_Grid3DWooTraversalAlgorithm algo(_outputBeforeGrid, false, list);

    CT_Beam beam(NULL, NULL);

    size_t progressStep = n_points / 20;
    size_t i = 0;

    CT_PointIterator itP(pointCloudIndex);
    while (itP.hasNext())
    {
        ++i;
        const CT_Point &point = itP.next().currentPoint();

        // Get the next ray
        beam.setOrigin(point);
        beam.setDirection(point - _scanner->getPosition());

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
    _outputBeforeGrid->computeMinMax();

    _progress = 100;
    emit progressChanged();
    qDebug() << "Fin de LVOX_ComputeBeforeWithLengthThresholdThread / ScanId=" << _scanner->getScanID();
}
