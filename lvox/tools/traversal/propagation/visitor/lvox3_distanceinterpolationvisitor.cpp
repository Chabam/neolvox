#include "lvox3_distanceinterpolationvisitor.h"

LVOX3_DistanceInterpolationVisitor::LVOX3_DistanceInterpolationVisitor(
        const lvox::Grid3Df* const inDensityGrid, lvox::Grid3Df* const outDensityGrid,
        const int power, const float densityThreshold) :
        m_inGrid(inDensityGrid), m_outGrid(outDensityGrid), m_power(power),
        m_densityThreshold(densityThreshold), m_numerator(0), m_denominator(0)
{
}

void LVOX3_DistanceInterpolationVisitor::start(const LVOX3_PropagationVisitorContext &context)
{
    Q_UNUSED(context)

    m_numerator = 0;
    m_denominator = 0;
}

void LVOX3_DistanceInterpolationVisitor::visit(const LVOX3_PropagationVisitorContext &context)
{
    if(context.m_distance > 0) {
        const lvox::Grid3DfType density = m_inGrid->valueAtIndex(context.m_cellIndex);

        /*
         * We compare density with >= here to include zero cell
         * values if the threshold is exactly zero.
         */
        if(density >= m_densityThreshold) {
            const double denom = std::pow(context.m_distance, m_power);
            m_numerator += ((double)density)/denom;
            m_denominator += 1.0/denom;
        }
    }
}

void LVOX3_DistanceInterpolationVisitor::finish(const LVOX3_PropagationVisitorContext &context)
{
    if(m_denominator != 0) {
        double result = m_numerator/m_denominator;
        m_outGrid->setValueAtIndex(context.m_cellIndex, result);
    }
}
