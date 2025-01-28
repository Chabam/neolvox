#ifndef CT_ABSTRACTMETRIC_RASTER_H
#define CT_ABSTRACTMETRIC_RASTER_H

#include "ctlibmetrics/ctlibmetrics_global.h"
#include "ctlibmetrics/ct_metric/abstract/ct_abstractmetricgeneric.h"

#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"
#include "ct_shape2ddata/ct_areashape2ddata.h"

class CTLIBMETRICS_EXPORT CT_AbstractMetric_Raster : public CT_AbstractMetricGeneric
{
    Q_OBJECT

public:
    CT_AbstractMetric_Raster(QString pluginName);
    CT_AbstractMetric_Raster(const CT_AbstractMetric_Raster &other);

    bool initDatas(const CT_AbstractImage2D* inRaster, const CT_AreaShape2DData *plotArea);

protected:
    const CT_AbstractImage2D*          _inRaster;
    const CT_AreaShape2DData*         _plotArea;
};

#endif // CT_ABSTRACTMETRIC_RASTER_H
