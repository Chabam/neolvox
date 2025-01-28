#include "lvox2_computehitsthread.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"

#include "ct_iterator/ct_pointiterator.h"

LVOX2_ComputeHitsThread::LVOX2_ComputeHitsThread(const CT_Scanner *scanner,
                                               CT_Grid3D<int> *grilleHits,
                                               CT_Grid3D<float> *grilleIn,
                                               CT_Grid3D<float> *grilleOut,
                                               const CT_Scene *scene,
                                               bool computeDistance, bool cylindricFilter,
                                                 double xMin, double xMax, double yMin, double yMax, double zMin, double zMax) : CT_MonitoredQThread()
{
    _scanner = scanner;
    _grilleHits = grilleHits;
    _grilleIn = grilleIn;
    _grilleOut = grilleOut;
    _scene = scene;
    _computeDistance = computeDistance;
    _cylindricFilter = cylindricFilter;
    _xMax =xMax;
    _xMin =xMin;
    _yMax =yMax;
    _yMin =yMin;
    _zMax =zMax;
    _zMin =zMin;

}

void LVOX2_ComputeHitsThread::run()
{
    qDebug() << "Début de LVOX2_ComputeHitsThread / ScanId=" << _scanner->getScanID();
    const CT_AbstractPointCloudIndex *pointCloudIndex = _scene->getPointCloudIndex();
    size_t n_points = pointCloudIndex->size();
    Eigen::Vector3d scanPos = _scanner->getPosition();
    double res = _grilleHits->resolution();

    size_t progressStep = n_points / 20;
    size_t i = 0;

    size_t nbOutPoints = 0;
    CT_PointIterator itP(pointCloudIndex);

    // grid param for cylindric filtering filtering
    Eigen::Vector3d min, max;
    _grilleHits->getBoundingBox(min, max);
    double xc = 0.5 * (_xMax + _xMin);
    double yc = 0.5 * (_yMax + _yMin);
    double r1 = 0.5 * (_xMax - _xMin);
    double r2 = 0.5 * (_yMax - _yMin);

    if (_cylindricFilter ) {
          qDebug() << "Filtering grid : center" << xc << yc << "radius:" << r1 << r2;
    }
    while (itP.hasNext())
    {
        ++i;
        const CT_Point &point = itP.next().currentPoint();

        size_t indice;
        if (_grilleHits->indexAtXYZ(point(0), point(1), point(2), indice))
        {
            double distsp = pow(pow(point(0) - scanPos(0),2.f)+pow(point(1) - scanPos(1),2.f)+pow(point(2) - scanPos(2),2.0),0.5);
            //if (distsp<=0.05) {
            //    qDebug() << "centerpoint" << distsp <<point(0) << point(1) << point(2)<<scanPos(0)<<scanPos(1)<<scanPos(2);
            //}
            if (point(0)>=_xMin&&point(0)<=_xMax&&point(1)>=_yMin&&point(1)<=_yMax&&point(2)>=_zMin&&point(2)<=_zMax) {
             if(!_cylindricFilter || pow((point(0) - xc)/r1, 2.0) + pow((point(1) - yc)/r2,2.0)<=1.f)
                     {
                  //qDebug() << "Filtering grid : center" << xc << yc << "radius:" << r1;
            // Hits Computing
            _grilleHits->addValueAtIndex(indice, 1);

            if (_computeDistance)
            {
                // Distances Sum Computing
                Eigen::Vector3d direction ( point(0) - scanPos(0),
                                            point(1) - scanPos(1),
                                            point(2) - scanPos(2));

                Eigen::Vector3d bottom, top, in, out;
                _grilleHits->getCellBottomLeftCornerAtXYZ(point(0), point(1), point(2), bottom);
                top(0) = bottom(0) + res;
                top(1) = bottom(1) + res;
                top(2) = bottom(2) + res;

                CT_Beam beam(NULL, NULL, scanPos, direction);

                if (beam.intersect(bottom, top, in, out))
                {
                    float distanceIn = sqrt(pow(in(0)-point(0), 2) + pow(in(1)-point(1), 2) + pow(in(2)-point(2), 2));
                    float distanceOut = sqrt(pow(out(0)-point(0), 2) + pow(out(1)-point(1), 2) + pow(out(2)-point(2), 2));

                    _grilleIn->addValueAtIndex(indice, distanceIn);
                    _grilleOut->addValueAtIndex(indice, distanceOut);
                }
            }
            }
            }
        } else
        {
            nbOutPoints++;
        }

        if (i % progressStep == 0)
        {
            _progress = 100*i/n_points;
            emit progressChanged();
        }
    }

    if (nbOutPoints > 0)
    {
        qDebug() << QString("%1 points des scènes d'entrées ne sont pas dans la grille").arg(nbOutPoints);
    }

    _grilleHits->computeMinMax(); // Calcul des limites hautes et basses des valeurs de la grille => Nécessaire à la visualisation

    if (_computeDistance)
    {
        // Convert sums into means
        size_t ncells = _grilleHits->nCells();
        for (size_t i = 0 ; i < ncells ; i++)
        {
            float value = _grilleHits->valueAtIndex(i);
            int na = _grilleHits->NA();
            if (value==0 || value==na)
            {
                _grilleIn->setValueAtIndex(i, _grilleIn->NA());
                _grilleOut->setValueAtIndex(i, _grilleOut->NA());
            } else {
                float invalue = _grilleIn->valueAtIndex(i) / value;
                float outvalue = _grilleOut->valueAtIndex(i) / value;

                _grilleIn->setValueAtIndex(i, invalue);
                _grilleOut->setValueAtIndex(i, outvalue);
            }
        }
        _grilleIn->computeMinMax();
        _grilleOut->computeMinMax();
    }

    _progress = 100;
    emit progressChanged();
    qDebug() << "Fin de LVOX2_ComputeHitsThread / ScanId=" << _scanner->getScanID();
}

