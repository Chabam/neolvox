/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_COMPUTEBEFORE_H
#define LVOX3_COMPUTEBEFORE_H

#include "lvox3_worker.h"
#include "tools/lvox3_gridtype.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

/*!
 * @brief Computes the "before" grid of a scene
 */
class LVOX3_ComputeBefore : public LVOX3_Worker
{
    Q_OBJECT

public:
    /**
     * @brief Create an object that will do the job.
     * @param pattern : shooting pattern
     * @param pointCloudIndex : index of points
     * @param before : store it the number of hits that was not stopped
     * @param shotDeltaDistance  : store it the distance between the first intersection point (IN) AND the second intersection point (OUT)
     */
    LVOX3_ComputeBefore(const CT_ShootingPattern* pattern,
                        const CT_AbstractPointCloudIndex* pointCloudIndex,
                        lvox::Grid3Di* before, bool computeDistance/*,
                        lvox::Grid3Di* shotStatsDistance = NULL*/);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    const CT_ShootingPattern*                 m_pattern;
    const CT_AbstractPointCloudIndex*   m_pointCloudIndex;
    lvox::Grid3Di*                      m_before;
    bool                                _computeDistance;
    //lvox::Grid3Di*                      _shotStatsDistance;
};

#endif // LVOX3_COMPUTEBEFORE_H
