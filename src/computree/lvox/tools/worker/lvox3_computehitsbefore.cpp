#include <QDebug>
#include "lvox3_computehitsbefore.h"

#include "tools/traversal/woo/lvox3_grid3dwootraversalalgorithm.h"
#include "tools/traversal/woo/visitor/lvox3_countvisitor.h"
#include "tools/traversal/woo/visitor/lvox3_distancevisitor.h"
#include "tools/lvox3_errorcode.h"
#include "tools/lvox3_gridtools.h"

#include "ct_itemdrawable/tools/gridtools/ct_grid3dwootraversalalgorithm.h"
#include "ct_iterator/ct_pointiterator.h"

LVOX3_ComputeHitsBefore::LVOX3_ComputeHitsBefore(CT_ShootingPattern* pattern,
                                         const CT_AbstractPointCloudIndex* pointCloudIndex,
                                         lvox::Grid3Di* before,
                                         lvox::Grid3Df* shotDeltaDistance,
                                         lvox::Grid3Di* hits,
                                         lvox::Grid3Df* shotInDistance,
                                         lvox::Grid3Df* shotOutDistance)
{
    m_pattern = pattern;
    m_pointCloudIndex = pointCloudIndex;
    m_before = before;
    m_shotDeltaDistance = shotDeltaDistance;
    m_hits = hits;
    m_shotInDistance = shotInDistance;
    m_shotOutDistance = shotOutDistance;
}

void LVOX3_ComputeHitsBefore::doTheJob()
{
    size_t i = 0, indice, pointCol, pointLin, pointLevel;
    Eigen::Vector3d bottom, top, in, out;
    size_t n_points = m_pointCloudIndex->size();

    // Creates visitors
    QVector<LVOX3_Grid3DVoxelWooVisitor*> list;

    LVOX3_CountVisitor<lvox::Grid3DiType> countVisitor(m_before);
    LVOX3_DistanceVisitor<lvox::Grid3DfType> distVisitor(m_shotDeltaDistance);

    list.append(&countVisitor);

    if (m_shotDeltaDistance != NULL)
        list.append(&distVisitor);

    // Creates traversal algorithm
    LVOX3_Grid3DWooTraversalAlgorithm<lvox::Grid3DiType> algo(m_before, false, list);

    setProgressRange(0, (m_shotDeltaDistance != NULL) ? n_points+1 : n_points);

    CT_PointIterator itP(m_pointCloudIndex);
    LVOX3_GridTools gridTool(m_hits);

    while (itP.hasNext()
           && !mustCancel())
    {
        ++i;
        const CT_Point &point = itP.next().currentPoint();
        Eigen::Vector3d shotOrigin = m_pattern->getShotForPoint(point).getOrigin();

        //Before Grid
        algo.compute(point, point - shotOrigin);

        //Hit Grid Computing
        if ((point.x()>m_hits->minX())&(point.x()<m_hits->maxX())&(point.y()>m_hits->minY())&(point.y()<m_hits->maxY())&(point.z()>m_hits->minZ())&(point.z()<m_hits->maxZ())) {

            gridTool.computeGridIndexForPoint(point, pointCol, pointLin, pointLevel, indice);

            if(!lvox::FilterCode::isFiltered(m_hits->valueAtIndex(indice))) {
                m_hits->addValueAtIndex(indice, 1);

                if (m_shotDeltaDistance != NULL)
                {
                    gridTool.computeCellBottomLeftTopRightCornerAtColLinLevel(pointCol, pointLin, pointLevel, bottom, top);
                    CT_Shot shot = m_pattern->getShotAt(indice);
                    Eigen::Vector3d shotOrig = shot.getOrigin();
                    if (LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(bottom, top, shotOrig, point - shotOrig, in, out))
                    {
                        if(m_shotInDistance != NULL)
                            m_shotInDistance->addValueAtIndex(indice, (in-point).norm());

                        if(m_shotOutDistance != NULL)
                            m_shotOutDistance->addValueAtIndex(indice, (out-point).norm());
                    }
                }
            }
        }
        setProgress(i);
    }
    //Don't forget to calculate min and max in order to visualize it as a colored map
    m_before->computeMinMax(); //Calculates the grid limits BEFOREGRID
    m_hits->computeMinMax(); // Calcul des limites hautes et basses des valeurs de la grille => Nécessaire à la visualisation HITS

    if (m_shotDeltaDistance != NULL
            && !mustCancel())
    {
        // Convert sums into means
        size_t ncells = m_hits->nCells();

        for (size_t i = 0 ; (i < ncells) && !mustCancel(); i++)
        {
            float nHits = m_hits->valueAtIndex(i);

            if (nHits <= 0)
            {
                m_shotDeltaDistance->setValueAtIndex(i, nHits);  //Before Grid distance
                if(m_shotInDistance != NULL)
                    m_shotInDistance->setValueAtIndex(i, nHits); //Hit Grid Distance

                if(m_shotOutDistance != NULL)
                    m_shotOutDistance->setValueAtIndex(i, nHits); //Hit Grid Distance
            } else {
                m_shotDeltaDistance->setValueAtIndex(i, /*m_shotDeltaDistance->valueAtIndex(i)/*/nHits); //Before Grid distance
                if(m_shotInDistance != NULL)
                    m_shotInDistance->setValueAtIndex(i, m_shotInDistance->valueAtIndex(i) / nHits); //Hit Grid Distance

                if(m_shotOutDistance != NULL)
                    m_shotOutDistance->setValueAtIndex(i, m_shotOutDistance->valueAtIndex(i) / nHits); //Hit Grid Distance
            }
        }
        //Before distance computeMinMax
        m_shotDeltaDistance->computeMinMax();
        //Hits distance computeMinMax
        if(m_shotInDistance != NULL)
            m_shotInDistance->computeMinMax();

        if(m_shotOutDistance != NULL)
            m_shotOutDistance->computeMinMax();

        setProgress(n_points+1);
    }
}
