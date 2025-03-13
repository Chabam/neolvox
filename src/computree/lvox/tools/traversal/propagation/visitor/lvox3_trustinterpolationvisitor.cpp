#include "lvox3_trustinterpolationvisitor.h"
#include <cmath>
#include <cassert>

LVOX3_TrustInterpolationVisitor::LVOX3_TrustInterpolationVisitor(const lvox::Grid3Df* inDensityGrid,
                                                                 lvox::Grid3Df* const outDensityGrid,
                                                                 const lvox::Grid3Di* inBeforeGrid,
                                                                 const lvox::Grid3Di* inTheoreticalsGrid,
                                                                 qint32 effectiveRayThreshold,
                                                                 qint32 endRayThreshold) :
    m_inGrid(inDensityGrid), m_outGrid(outDensityGrid), m_beforeGrid(inBeforeGrid),
    m_theoriticalsGrid(inTheoreticalsGrid), m_effectiveRayThreshold(effectiveRayThreshold),
    m_endRayThreshold(endRayThreshold),
    m_denominator(0), m_numerator(0)
{
    if(m_effectiveRayThreshold > m_endRayThreshold)
        qSwap(m_effectiveRayThreshold, m_endRayThreshold);

    m_diffEffectiveRay = m_endRayThreshold - m_effectiveRayThreshold;

    // FIXME: what is the proper way to validate inputs?
    // I would do a hard assert here supposing the inputs are validated from the UI.
    Q_ASSERT(m_diffEffectiveRay != 0);
}

void LVOX3_TrustInterpolationVisitor::start(const LVOX3_PropagationVisitorContext &context)
{
    Q_UNUSED(context)

    m_numerator = 0;
    m_denominator = 0;
}

/**
 * Static function for testing
 */
double LVOX3_TrustInterpolationVisitor::getTrustFactor(const qint32& Nt, const qint32& Nb,
                                                       const qint32& effectiveRayThreshold, const qint32 &endRayThreshold,
                                                       const qint32& diffEffectiveRay)
{
    double trustFactor = 0;
    const qint32 Nt_minus_Nb = Nt - Nb;

    if(Nt_minus_Nb > effectiveRayThreshold) {
        if(Nt_minus_Nb < endRayThreshold) {
            double ratio = ((double)(Nt_minus_Nb - effectiveRayThreshold)) / ((double)diffEffectiveRay);
            trustFactor = 0.5 * (std::sin(ratio * M_PI - M_PI_2) + 1);
        } else {
            trustFactor = 1;
        }
    }

    return trustFactor;
}

void LVOX3_TrustInterpolationVisitor::visit(const LVOX3_PropagationVisitorContext &context)
{
    if(context.m_distance > 0) {
        const lvox::Grid3DfType density = m_inGrid->valueAtIndex(context.m_cellIndex);

        if(density > 0) {
            const qint32 Nt = m_theoriticalsGrid->valueAtIndex(context.m_cellIndex);
            const qint32 Nb = m_beforeGrid->valueAtIndex(context.m_cellIndex);

            const double trustFactor = getTrustFactor(Nt, Nb, m_effectiveRayThreshold, m_endRayThreshold, m_diffEffectiveRay);

            m_numerator += ((double)density) * trustFactor;
            m_denominator += trustFactor;
        }
    }
}

void LVOX3_TrustInterpolationVisitor::finish(const LVOX3_PropagationVisitorContext &context)
{
    if(m_denominator != 0) {
        m_outGrid->setValueAtIndex(context.m_cellIndex,  m_numerator/m_denominator);
    }
}
