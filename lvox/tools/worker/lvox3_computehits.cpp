#include <QDebug>
#include "lvox3_computehits.h"

//Tools
#include "ct_global/ct_context.h"
#include "loginterface.h"
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_rayboxintersectionmath.h"
#include "tools/lvox3_errorcode.h"

//Drawables
#include "ct_itemdrawable/ct_beam.h"
#include "ct_iterator/ct_pointiterator.h"


LVOX3_ComputeHits::LVOX3_ComputeHits(CT_ShootingPattern* pattern,
                                     const CT_AbstractPointCloudIndex* pointCloudIndex,
                                     lvox::Grid3Di* hits,
                                     bool computedistance) : LVOX3_Worker()
                                     /*lvox::Grid3Di *shotInDistance,
                                     lvox::Grid3Df *shotOutDistance) : LVOX3_Worker()*/
{
    m_pattern = pattern;
    m_pointCloudIndex = pointCloudIndex;
    m_hits = hits;
    _computedistance = computedistance;
    //m_shotInDistance = shotInDistance;
    /*m_shotOutDistance = shotOutDistance;*/
}

void LVOX3_ComputeHits::doTheJob()
{
    qDebug() << "Start Hits ";
    size_t i = 0, indice, pointCol, pointLin, pointLevel;
    Eigen::Vector3d bottom, top, in, out;

    const size_t n_points = m_pointCloudIndex->size();

    bool computeDistance = _computedistance;/*(m_shotInDistance != NULL) || (m_shotOutDistance != NULL);*/

    setProgressRange(0, computeDistance ? n_points+1 : n_points);

    LVOX3_GridTools gridTool(m_hits);

    CT_PointIterator itP(m_pointCloudIndex);

    // PS_LOG->addMessage(LogInterface::info, LogInterface::step,
    //                    QObject::tr("PointCloudSize = %1, PointCloudPatternSize : %2")
    //                        .arg(QString::number(m_pointCloudIndex->size()))
    //                        .arg(QString::number(m_pattern->numberOfShots())));

    while (itP.hasNext()
           && !mustCancel())
    {
        ++i;
        const CT_Point &point = itP.next().currentPoint();

        // Eigen::Vector3d point_orig = m_pattern->shotAt(i-1).origin();
        // Eigen::Vector3d point_dir = m_pattern->shotAt(i-1).direction();

        // const Eigen::Vector3d &point2 = point_orig + point_dir;

        // if(i%50000 == 0)
        // {
        // PS_LOG->addMessage(LogInterface::info, LogInterface::step,
        //                    QObject::tr("i : %7 \n x1 : %1, y1 : %2, z1 : %3 / x2 : %4, y2 : %5, z2 : %6")
        //                         .arg(QString::number(point.x()))
        //                         .arg(QString::number(point.y()))
        //                         .arg(QString::number(point.z()))
        //                         .arg(QString::number(point2.x()))
        //                         .arg(QString::number(point2.y()))
        //                         .arg(QString::number(point2.z()))
        //                         .arg(QString::number(i-1)));
        // }




        // we now that the grid is perfectly bounding the scene so we can use this tools that don't do
        // many check to reduce the compute time !
        // FP 11/2017 NOT TRUE: we should check if the point is in the grid first thanks to the following test        
        if ( point.x() > m_hits->minX() && point.x() < m_hits->maxX() &&
             point.y() > m_hits->minY() && point.y() < m_hits->maxY() &&
             point.z() > m_hits->minZ() && point.z() < m_hits->maxZ())
        {

            gridTool.computeGridIndexForPoint(point, pointCol, pointLin, pointLevel, indice);

            if(!lvox::FilterCode::isFiltered(m_hits->valueAtIndex(indice))) {

                if (computeDistance)
                {
                    gridTool.computeCellBottomLeftTopRightCornerAtColLinLevel(pointCol, pointLin, pointLevel, bottom, top);
                    //qDebug() << "ligne 1 : " << indice;
                    CT_Shot shot = m_pattern->shotForPoint(point);

                    // Change needed for MLS scans : gps_time attribut has the same indexation as point cloud
                    // CT_Shot shot = m_pattern->shotAt(i-1);

                    //CT_Shot shot = m_pattern->getShotAt(indice); // Memory problems seem to apear from this line
                    //qDebug() << "ligne 2 B";
                    Eigen::Vector3d shotOrig = shot.origin();
                    Eigen::Vector3d direction = shot.direction();
                    if (LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(bottom, top, shotOrig, direction, in, out))
                    {                      
                        m_hits->doSommation(indice,(in-point).norm() + (out-point).norm(),(in-point).norm());
                        m_hits->addValueAtIndex(indice, 1);
                    }
                }
                else
                {
                    m_hits->addValueAtIndex(indice, 1);
                }
            }
        }
        setProgress(i);
    }

    m_hits->computeMinMax(); // Calcul des limites hautes et basses des valeurs de la grille => Nécessaire à la visualisation

    if (computeDistance && !mustCancel())
    {
        //Code Précédant pour le calcul des moyennes
        // Convert sums into means
        /*size_t ncells = m_hits->nCells();

        for (size_t i = 0 ; (i < ncells) && !mustCancel(); i++)
        {
            float nHits = m_hits->valueAtIndex(i);

            if (nHits <= 0)
            {
                if(m_shotInDistance != NULL)
                {
                    m_shotInDistance->setValueAtIndex(i, nHits);
                }

                if(m_shotOutDistance != NULL)
                {
                    m_shotOutDistance->setValueAtIndex(i, nHits);
                }
            } else {
                if(m_shotInDistance != NULL)
                {
                    m_shotInDistance->setValueAtIndex(i, nHits);


                }
                if(m_shotOutDistance != NULL)
                {
                    m_shotOutDistance->setValueAtIndex(i,  nHits);
                }
            }
        }

        if(m_shotInDistance != NULL)
            m_shotInDistance->computeMinMax();

        if(m_shotOutDistance != NULL)
            m_shotOutDistance->computeMinMax();
*/
        setProgress(n_points+1);
    }
}
