/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_INTERPOLATETRUSTFACTOR_H
#define LVOX3_INTERPOLATETRUSTFACTOR_H

#include "lvox3_worker.h"
#include "tools/lvox3_gridtype.h"

/**
 * @brief Interpolate the grid density by using cells values that was in a defined radius and use a trust factor
 *
 * The formula used is : Sum(IDR * TF)
 *                       --------------------
 *                       Sum(TF)
 *
 * "IDR" = value of density of the cell inspected
 * "TF" a trust factor calculated like this :
 *      if ((Nb - Nt) is <= effectiveRayThreshold)                  => TF = 0
 *      if ((Nb - Nt) > endRayThreshold)                            => TF = 1
 *      if (effectiveRayThreshold < (Nb - Nt) < endRayThreshold     => TF = sin( ((Nb - Nt) - effectiveRayThreshold) / (endRayThreshold-effectiveRayThreshold))
 */
class LVOX3_InterpolateTrustFactor : public LVOX3_Worker
{
    Q_OBJECT

public:
    /**
     * @brief Construct this worker
     * @param originalDensityGrid : the original density grid to get values of density
     * @param beforeGrid : before grid to get values
     * @param theoriticalsGrid : theoriticals grid to get values
     * @param outDensityGrid : the density grid that will be modified with new density values
     * @param radius : max radius to search cells
     * @param effectiveRayThreshold : minimum number of effective ray (Nt-Nb must be > to threshold) to have a trust factor > 0
     * @param endRayThreshold : maximum number of effective ray. If (Nt-Nb) > endRayThreshold the trust factor will be 1
     */
    LVOX3_InterpolateTrustFactor(const lvox::Grid3Df* originalDensityGrid,
                                 const lvox::Grid3Di* beforeGrid,
                                 const lvox::Grid3Di* theoriticalsGrid,
                                 lvox::Grid3Df* outDensityGrid,
                                 double radius,
                                 qint32 effectiveRayThreshold,
                                 qint32 endRayThreshold);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    lvox::Grid3Df*      m_originalDensityGrid;
    lvox::Grid3Di*      m_beforeGrid;
    lvox::Grid3Di*      m_theoriticalsGrid;
    lvox::Grid3Df*      m_outDensityGrid;
    double              m_radius;
    qint32              m_effectiveRayThreshold;
    qint32              m_endRayThreshold;
};

#endif // LVOX3_INTERPOLATETRUSTFACTOR_H
