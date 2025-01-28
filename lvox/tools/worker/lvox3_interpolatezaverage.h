#ifndef LVOX3_INTERPOLATEZAVERAGE_H
#define LVOX3_INTERPOLATEZAVERAGE_H

#include <QObject>
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

class LVOX3_InterpolateZAverage : public LVOX3_Worker
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
     */
    LVOX3_InterpolateZAverage(const lvox::Grid3Df *originalDensityGrid,
                              lvox::Grid3Df* const outDensityGrid,
                              const lvox::Grid3Di *hitGrid,
                              const lvox::Grid3Di *theoreticalGrid,
                              const lvox::Grid3Di *beforeGrid,
                              qint32 effectiveRayThreshold,
                              qint32 numZCalculatedOn);
/*
    LVOX3_InterpolateZAverage(const lvox::Grid3Df *originalDensityGrid,
                              lvox::Grid3Df* const outDensityGrid,
                              const lvox::Grid3Di *hitGrid,
                              const lvox::Grid3Di *theoreticalGrid,
                              const lvox::Grid3Di *beforeGrid,
                              qint32 effectiveRayThreshold,
                              qint32 isRelativeThreshold,
                              qint32 numZCalculatedOn);
*/
protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    /*m_originalDensityGrid = (lvox::Grid3Df*)INdensityGrid;
    m_outDensityGrid = (lvox::Grid3Df*)OUTdensityGrid;
    m_beforeGrid = (lvox::Grid3Di*)beforeGrid;
    m_theoriticalsGrid = (lvox::Grid3Di*)theoreticalGrid;
    m_hitGrid = (lvox::Grid3Di*)hitGrid;
    m_effectiveRayThreshold = effectiveRayThreshold;*/

    lvox::Grid3Df*      m_originalDensityGrid;
    lvox::Grid3Di*      m_beforeGrid;
    lvox::Grid3Di*      m_theoriticalsGrid;
    lvox::Grid3Df*      m_outDensityGrid;
    lvox::Grid3Di*      m_hitGrid;
    qint32              m_effectiveRayThreshold;
    //qint32              m__isRelativeThreshold;
    qint32              m_numZCalculatedOn;
};

#endif // LVOX3_INTERPOLATEZAVERAGE_H
