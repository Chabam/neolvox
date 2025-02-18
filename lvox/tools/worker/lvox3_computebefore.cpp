#include <QDebug>
#include "lvox3_computebefore.h"

//Tools/Traversal algorithms
#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"
#include "tools/traversal/woo/lvox3_traversaldefs.h"
#include "tools/traversal/woo/visitor/lvox3_countvisitor.h"
#include "tools/traversal/woo/visitor/lvox3_distancevisitor.h"

#include "ct_iterator/ct_pointiterator.h"

LVOX3_ComputeBefore::LVOX3_ComputeBefore(CT_ShootingPattern* pattern,
                                         const CT_AbstractPointCloudIndex* pointCloudIndex,
                                         lvox::Grid3Di* before,
                                         bool computeDistance)
{
    m_pattern = pattern;
    m_pointCloudIndex = pointCloudIndex;
    m_before = before;
    _computeDistance = computeDistance;
    //_shotStatsDistance = shotStatsDistance;
}

void LVOX3_ComputeBefore::doTheJob()
{
    int i = 0;
    size_t n_points = m_pointCloudIndex->size();

    // Creates visitors
    QList<CT_AbstractGrid3DBeamVisitor*> list;

    LVOX3_CountVisitor<lvox::Grid3DiType> countVisitor(m_before);
    LVOX3_DistanceVisitor<lvox::Grid3DiType> distVisitor(m_before);



    if (_computeDistance)
        list.append(&distVisitor);
    else list.append(&countVisitor);

    // Creates traversal algorithm
    LVOX3_Grid3DWooTraversalAlgorithm algo(m_before, false, list);

    setProgressRange(0, /*(_shotStatsDistance != NULL)*/_computeDistance ? n_points+1 : n_points);

    CT_PointIterator itP(m_pointCloudIndex);

    while (itP.hasNext()
           && !mustCancel())
    {
        const CT_Point &point = itP.next().currentPoint();
        // Eigen::Vector3d shotOrigin2 = m_pattern->shotForPoint(point).origin();

        // Change needed for MLS scans : gps_time attribut has the same indexation as point cloud
        Eigen::Vector3d shotOrigin = m_pattern->shotAt(i).origin();

        //Eigen::Vector3d direction = m_pattern->getShotForPoint(point).getDirection();
        // algo already check if the beam touch the grid or not so we don't have to do twice !
        //algo.compute(point, point - shotOrigin);
        CT_Beam beam(point, (point - shotOrigin).normalized());
        algo.compute(beam);

        ++i;
        setProgress(i);
    }

    // Don't forget to calculate min and max in order to visualize it as a colored map
    m_before->computeMinMax();
    /*if(_shotStatsDistance != NULL)*/
    if (_computeDistance && !mustCancel())
    {

        // To get the mean distance we have to divide in each voxel the sum of distances by the number of hits
        /*for (int i = 0 ; i < m_before->nCells() && !mustCancel() ; i++ )
        {
            const float nHits = m_before->valueAtIndex(i);

            if (nHits <= 0)
                m_before->setValueAtIndex(i, nHits);  // TODO : check if must set an error code here
            else
                m_before->setValueAtIndex(i, m_shotDeltaDistance->valueAtIndex(i)/nHits);
        }

        m_before->computeMinMax();*/

        setProgress(n_points+1);
    }
}
