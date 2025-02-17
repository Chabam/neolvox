#include "distanceinterpolationvisitor.h"

DistanceInterpolationVisitor::DistanceInterpolationVisitor(const Grid3Df& inDensityGrid, Grid3Df& outDensityGrid,
                                                           const int power, const float densityThreshold)
    : m_inGrid(inDensityGrid), m_outGrid(outDensityGrid), m_power(power), m_densityThreshold(densityThreshold),
      m_numerator(0), m_denominator(0)
{
}

void DistanceInterpolationVisitor::start(const PropagationVisitorContext& context)
{

    m_numerator = 0;
    m_denominator = 0;
}

void DistanceInterpolationVisitor::visit(const PropagationVisitorContext& context)
{
    if (context.m_distance > 0)
    {
        const float density = m_inGrid.getValueAtIndex(context.m_cellIndex);

        /*
         * We compare density with >= here to include zero cell
         * values if the threshold is exactly zero.
         */
        if (density >= m_densityThreshold)
        {
            const double denom = std::pow(context.m_distance, m_power);
            m_numerator += ((double)density) / denom;
            m_denominator += 1.0 / denom;
        }
    }
}

void DistanceInterpolationVisitor::finish(const PropagationVisitorContext& context)
{
    if (m_denominator != 0)
    {
        double result = m_numerator / m_denominator;
        m_outGrid.setValueAtIndex(context.m_cellIndex, result);
    }
}
