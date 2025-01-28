#ifndef CT_ABSTRACTMETRIC_XYZ_H
#define CT_ABSTRACTMETRIC_XYZ_H

#include "ctlibmetrics/ctlibmetrics_global.h"
#include "ctlibmetrics/ct_metric/abstract/ct_abstractmetricgeneric.h"

#include "ct_pointcloudindex/ct_pointcloudindexvector.h"
#include "ct_shape2ddata/ct_areashape2ddata.h"

class CTLIBMETRICS_EXPORT CT_AbstractMetric_XYZ : public CT_AbstractMetricGeneric
{
    Q_OBJECT

public:
    CT_AbstractMetric_XYZ(QString pluginName);
    CT_AbstractMetric_XYZ(const CT_AbstractMetric_XYZ &other);

    bool initDatas(const CT_AbstractPointCloudIndex* inCloud, const CT_AreaShape2DData *plotArea);

    const CT_AbstractPointCloudIndex* pointCloud() const;
    const CT_AreaShape2DData* plotArea() const;

protected:
    const CT_AbstractPointCloudIndex* _inCloud;
    const CT_AreaShape2DData*         _plotArea;
};

#endif // CT_ABSTRACTMETRIC_XYZ_H
