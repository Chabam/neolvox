#include "trustinterpolationvisitor.h"
#include <cassert>
#include <cmath>

TrustInterpolationVisitor::TrustInterpolationVisitor(const Grid3Df& inDensityGrid, Grid3Df& outDensityGrid,
                                                     const Grid3Di& inBeforeGrid, const Grid3Di& inTheoreticalsGrid,
                                                     int32_t effectiveRayThreshold, int32_t endRayThreshold)
    : m_inGrid(inDensityGrid), m_outGrid(outDensityGrid), m_beforeGrid(inBeforeGrid),
      m_theoriticalsGrid(inTheoreticalsGrid), m_effectiveRayThreshold(effectiveRayThreshold),
      m_endRayThreshold(endRayThreshold), m_denominator(0), m_numerator(0)
{
    if (m_effectiveRayThreshold > m_endRayThreshold)
        std::swap(m_effectiveRayThreshold, m_endRayThreshold);

    // TODO : throw exception if not different than zero
    m_diffEffectiveRay = m_endRayThreshold - m_effectiveRayThreshold;
}

void TrustInterpolationVisitor::start(const PropagationVisitorContext& context)
{
    (void)context;

    m_numerator = 0;
    m_denominator = 0;
}

/**
 * Static function for testing
 */
double TrustInterpolationVisitor::getTrustFactor(const int32_t& Nt, const int32_t& Nb,
                                                 const int32_t& effectiveRayThreshold, const int32_t& endRayThreshold,
                                                 const int32_t& diffEffectiveRay)
{
    double trustFactor = 0;
    const int32_t Nt_minus_Nb = Nt - Nb;

    if (Nt_minus_Nb > effectiveRayThreshold)
    {
        if (Nt_minus_Nb < endRayThreshold)
        {
            double ratio = ((double)(Nt_minus_Nb - effectiveRayThreshold)) / ((double)diffEffectiveRay);
            trustFactor = 0.5 * (std::sin(ratio * M_PI - M_PI_2) + 1);
        }
        else
        {
            trustFactor = 1;
        }
    }

    return trustFactor;
}

void TrustInterpolationVisitor::visit(const PropagationVisitorContext& context)
{
    if (context.m_distance > 0)
    {
        const float density = m_inGrid.getValueAtIndex(context.m_cellIndex);

        if (density > 0)
        {
            const int32_t Nt = m_theoriticalsGrid.getValueAtIndex(context.m_cellIndex);
            const int32_t Nb = m_beforeGrid.getValueAtIndex(context.m_cellIndex);

            const double trustFactor =
                getTrustFactor(Nt, Nb, m_effectiveRayThreshold, m_endRayThreshold, m_diffEffectiveRay);

            m_numerator += ((double)density) * trustFactor;
            m_denominator += trustFactor;
        }
    }
}

void TrustInterpolationVisitor::finish(const PropagationVisitorContext& context)
{
    if (m_denominator != 0)
    {
        m_outGrid.setValueAtIndex(context.m_cellIndex, m_numerator / m_denominator);
    }
}
