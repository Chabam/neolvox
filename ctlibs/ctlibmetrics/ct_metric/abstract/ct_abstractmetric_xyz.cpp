#include "ct_abstractmetric_xyz.h"

CT_AbstractMetric_XYZ::CT_AbstractMetric_XYZ(QString pluginName) : CT_AbstractMetricGeneric(pluginName)
{
    _inCloud = nullptr;
    _plotArea = nullptr;
}

CT_AbstractMetric_XYZ::CT_AbstractMetric_XYZ(const CT_AbstractMetric_XYZ &other) : CT_AbstractMetricGeneric(other)
{
    _inCloud = other._inCloud;
    _plotArea = other._plotArea;
}

bool CT_AbstractMetric_XYZ::initDatas(const CT_AbstractPointCloudIndex* inCloud, const CT_AreaShape2DData* plotArea)
{
    _inCloud = inCloud;
    _plotArea = plotArea;

    return (_inCloud != nullptr);
}

const CT_AreaShape2DData *CT_AbstractMetric_XYZ::plotArea() const
{
    return _plotArea;
}

const CT_AbstractPointCloudIndex *CT_AbstractMetric_XYZ::pointCloud() const
{
    return _inCloud;
}
