/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COMPUTEHITS_H
#define LVOX3_COMPUTEHITS_H

#include "lvox3_worker.h"
#include "tools/lvox3_gridtype.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

/*!
 * @brief Computes the "hit" grid of a scene
 */
class LVOX3_ComputeHits : public LVOX3_Worker
{
    Q_OBJECT

public:
    /**
     * @brief Create an object that will do the job. Each voxel of the resulting grid stores the number of hits inside itself.
     * @param pattern : shooting pattern
     * @param pointCloudIndex : index of points
     * @param hitsGrid : store it the number of hits
     * @param shotInDistance  : store it the distance between the first intersection point of the shot and the voxel AND the hitted point
     * @param shotOutDistance  : store it the distance between the second intersection point of the shot and the voxel AND the hitted point
     */
    LVOX3_ComputeHits(const CT_ShootingPattern* pattern,
                      const CT_AbstractPointCloudIndex* pointCloudIndex,
                      lvox::Grid3Di* hits, bool computedistance);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    const CT_ShootingPattern*           m_pattern;
    const CT_AbstractPointCloudIndex*   m_pointCloudIndex;
    lvox::Grid3Di*                  m_hits;
    bool _computedistance;
    //lvox::Grid3Di*                   m_shotInDistance;
    /*lvox::Grid3Df*                   m_shotOutDistance;*/
};

#endif // LVOX3_COMPUTEHITS_H
