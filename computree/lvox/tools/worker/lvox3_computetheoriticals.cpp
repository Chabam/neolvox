#include "lvox3_computetheoriticals.h"

//Tools
#include "ct_itemdrawable/tools/gridtools/ct_grid3dwootraversalalgorithm.h"
#include "tools/traversal/woo/visitor/lvox3_countvisitor.h"
#include "tools/traversal/woo/visitor/lvox3_distancevisitor.h"
#include "tools/lvox3_errorcode.h"

#include <QFile>

LVOX3_ComputeTheoriticals::LVOX3_ComputeTheoriticals(const CT_ShootingPattern* pattern,
                                                     lvox::Grid3Di* theoricals,
                                                     bool computedistance) : LVOX3_Worker()
{
    m_pattern = pattern;
    m_outputTheoriticalGrid = theoricals;
    //_outputStatsTheoriticalGrid = shotStatsDistance;
    _computedistance =computedistance;
}

LVOX3_ComputeTheoriticals::~LVOX3_ComputeTheoriticals()
{
}

void LVOX3_ComputeTheoriticals::doTheJob()
{
    // Creates visitors
    QList<CT_AbstractGrid3DBeamVisitor*> list;

    LVOX3_CountVisitor<lvox::Grid3DiType> countVisitor(m_outputTheoriticalGrid);
    LVOX3_DistanceVisitor<lvox::Grid3DiType> distVisitor(m_outputTheoriticalGrid);

    if (!_computedistance) list.append(&countVisitor);
    else list.append(&distVisitor);

    // Creates traversal algorithm
    CT_Grid3DWooTraversalAlgorithm algo(m_outputTheoriticalGrid, true, list);

    const size_t nShot = m_pattern->numberOfShots();
    qDebug() << "Nb shot Theo: " << nShot;

    //setProgressRange(0, /*(_outputStatsTheoriticalGrid != NULL)*/_computedistance ? nShot+1 : nShot);
    setProgressRange(0, _computedistance ? nShot+1 : nShot);

    for(size_t i=0; (i<nShot) && !mustCancel(); ++i) {
        // NB : No change for MLS
        const CT_Shot shot = m_pattern->shotAt(i);
        // qDebug() << "(" << shot.direction().x() << " " << shot.direction().y() << " " << shot.direction().z() << ")"
        //          << " (" << shot.origin().x() << " " << shot.origin().y() << " " << shot.origin().z() << ")";

        // algo already check if the ray touch the grid or not so we don't have to do twice !
        CT_Beam beam(shot.origin(), shot.direction());
        algo.compute(beam);

        setProgress(i);
    }

    // Don't forget to calculate min and max in order to visualize it as a colored map
    m_outputTheoriticalGrid->computeMinMax();

    setProgress(nShot+1);

}
