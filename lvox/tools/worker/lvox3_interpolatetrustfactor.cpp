#include "lvox3_interpolatetrustfactor.h"

//Tools
#include "tools/lvox3_errorcode.h"

//Traversal algorithms
#include "tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h"
#include "tools/traversal/propagation/visitor/lvox3_trustinterpolationvisitor.h"

LVOX3_InterpolateTrustFactor::LVOX3_InterpolateTrustFactor(const lvox::Grid3Df* originalDensityGrid,
                                                           const lvox::Grid3Di* beforeGrid,
                                                           const lvox::Grid3Di* theoriticalsGrid,
                                                           lvox::Grid3Df* const outDensityGrid,
                                                           double radius,
                                                           qint32 effectiveRayThreshold,
                                                           qint32 endRayThreshold)
{
    m_originalDensityGrid = (lvox::Grid3Df*)originalDensityGrid;
    m_outDensityGrid = (lvox::Grid3Df*)outDensityGrid;
    m_beforeGrid = (lvox::Grid3Di*)beforeGrid;
    m_theoriticalsGrid = (lvox::Grid3Di*)theoriticalsGrid;
    m_radius = radius;
    m_effectiveRayThreshold = effectiveRayThreshold;
    m_endRayThreshold = endRayThreshold;
}

void LVOX3_InterpolateTrustFactor::doTheJob()
{
    LVOX3_TrustInterpolationVisitor visitor(m_originalDensityGrid,
                                            m_outDensityGrid,
                                            m_beforeGrid,
                                            m_theoriticalsGrid,
                                            m_effectiveRayThreshold,
                                            m_endRayThreshold);

    QVector<LVOX3_PropagationVisitor*> l;
    l.append(&visitor);

    LVOX3_Grid3DPropagationAlgorithm algo(m_originalDensityGrid,
                                          l,
                                          m_radius);

    const size_t& nCells = m_originalDensityGrid->nCells();

    for(size_t i=0; i<nCells; ++i) {
        const lvox::Grid3DfType density = m_originalDensityGrid->valueAtIndex(i);

        if(density == lvox::Nt_Minus_Nb_Inferior_Threshold) {
            algo.startFromCell(i);
        }
    }
}
