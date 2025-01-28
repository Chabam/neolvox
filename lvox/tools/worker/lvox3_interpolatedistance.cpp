#include "lvox3_interpolatedistance.h"

//Tools
#include "ct_global/ct_context.h"
#include "loginterface.h"
#include "tools/lvox3_errorcode.h"

//Traversal algorithms
#include "tools/traversal/propagation/lvox3_grid3dpropagationalgorithm.h"
#include "tools/traversal/propagation/visitor/lvox3_distanceinterpolationvisitor.h"

LVOX3_InterpolateDistance::LVOX3_InterpolateDistance(
        const lvox::Grid3Df* originalDensityGrid,
        lvox::Grid3Df* const outDensityGrid, double radius, int power, float densityThreshold) :
        m_originalDensityGrid(originalDensityGrid), m_outDensityGrid(outDensityGrid),
        m_radius(radius), m_power(power), m_densityThreshold(densityThreshold) {
}

void LVOX3_InterpolateDistance::doTheJob()
{
    LVOX3_DistanceInterpolationVisitor visitor(m_originalDensityGrid,
                                               m_outDensityGrid,
                                               m_power, m_densityThreshold);

    QVector<LVOX3_PropagationVisitor*> l;
    l.append(&visitor);

    LVOX3_Grid3DPropagationAlgorithm algo(m_originalDensityGrid,
                                          l,
                                          m_radius);

    const size_t& nCells = m_originalDensityGrid->nCells();

    PS_LOG->addInfoMessage(LogInterface::step, tr("Cell number : %1")
                                                   .arg((QString::number(nCells))));

    for(size_t i=0; i<nCells; ++i) {
        const lvox::Grid3DfType density = m_originalDensityGrid->valueAtIndex(i);

        if(i%100000 == 0)
        {
        PS_LOG->addInfoMessage(LogInterface::step, tr("i : %1")
                                                       .arg((QString::number(i))));
        }
        //Goes through the grid and interpolates what is considered no data (-1,-2), the rest is an error code and brought back to zero
        if(lvox::NoDataCode::isNoData(density)) {
            algo.startFromCell(i);
        }
    }
}
