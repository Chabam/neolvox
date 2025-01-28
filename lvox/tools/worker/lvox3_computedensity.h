/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COMPUTEDENSITY_H
#define LVOX3_COMPUTEDENSITY_H

#include "lvox3_worker.h"
#include "tools/lvox3_gridtype.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

/*!
 * @brief Computes the "density" grid of a scene
 */
class LVOX3_ComputeDensity : public LVOX3_Worker
{
    Q_OBJECT

public:
    /**
     * @brief Create an object that will do the job.
     * @param density (OUT) : grid that will contains density information or error code
     * @param hits : grid that contains Ni information
     * @param theoritical : grid that contains Nt information
     * @param before : grid that contains Nb information
     * @param effectiveRayThreshold : minimum number of effective ray (Nt-Nb must be > to threshold) to compute the density
     */
    LVOX3_ComputeDensity(lvox::Grid3Df* density,
                         const lvox::Grid3Di* hits,
                         const lvox::Grid3Di* theoritical,
                         const lvox::Grid3Di* before,
                         qint32 effectiveRayThreshold);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    lvox::Grid3Df*      m_density;
    lvox::Grid3Di*      m_hits;
    lvox::Grid3Di*      m_theoritical;
    lvox::Grid3Di*      m_before;
    qint32              m_effectiveRayThreshold;
};

#endif // LVOX3_COMPUTEDENSITY_H
