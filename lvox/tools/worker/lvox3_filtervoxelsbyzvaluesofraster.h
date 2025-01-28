/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_FILTERVOXELSBELOWZVALUES_H
#define LVOX3_FILTERVOXELSBELOWZVALUES_H

#include "lvox3_worker.h"
#include "tools/3dgrid/abstract/lvox3_abstractgrid3d.h"
#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"

/**
 * @brief Set a value at all cells that was above or below a zValue from a raster at the same coordinate (x, y)
 */
class LVOX3_FilterVoxelsByZValuesOfRaster : public LVOX3_Worker
{
    Q_OBJECT

public:
    enum FilterType {
        Below,
        Above
    };

    LVOX3_FilterVoxelsByZValuesOfRaster(const QList<LVOX3_AbstractGrid3D*>& grids,
                                        const CT_AbstractImage2D* zValues,
                                        FilterType filter,
                                        double replacementValue);

protected:
    /**
     * @brief Do the job
     */
    void doTheJob();

private:
    QVector<LVOX3_AbstractGrid3D*>     m_grids;
    CT_AbstractImage2D*             m_zValues;
    FilterType                      m_filter;
    double                          m_replacementValue;
};

#endif // LVOX3_FILTERVOXELSBELOWZVALUES_H
