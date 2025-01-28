#include "ct_abstractmetric_las.h"

CT_AbstractMetric_LAS::CT_AbstractMetric_LAS(QString pluginName) : CT_AbstractMetric_XYZ(pluginName)
{
    _lasAttributes = nullptr;
}

CT_AbstractMetric_LAS::CT_AbstractMetric_LAS(const CT_AbstractMetric_LAS &other) : CT_AbstractMetric_XYZ(other)
{

}

bool CT_AbstractMetric_LAS::initLasDatas(const CT_AbstractPointCloudIndex *inCloud, const CT_AreaShape2DData *plotArea, const CT_StdLASPointsAttributesContainer *lasAttributes)
{
    if((lasAttributes == nullptr) || lasAttributes->lasPointsAttributes().isEmpty() || !initDatas(inCloud, plotArea))
        return false;

    _lasAttributes = const_cast<CT_StdLASPointsAttributesContainer*>(lasAttributes);

    return true;
}

CT_StdLASPointsAttributesContainer *CT_AbstractMetric_LAS::lasAttributes() const
{
    return _lasAttributes;
}
