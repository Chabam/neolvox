#ifndef CT_HANDLEOUTPOINTCOLOR_H
#define CT_HANDLEOUTPOINTCOLOR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"
#include "ct_attributes/managers/ct_densepointcolormanager.h"
#include "ct_attributes/managers/ct_sparsepointcolormanager.h"

class CT_HandleOutPointColor : public CT_HandleOutItem<CT_PointsAttributesColor>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleOutPointColor() = default;
};

class CT_HandleOutPointColorWithDenseManager : public CT_HandleOutPointColor, public CT_DensePointColorManager
{
public:
    CT_HandleOutPointColorWithDenseManager() = default;

    CT_PointsAttributesColor* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesColor(pcir, *this);
    }
};

class CT_HandleOutPointColorWithSparseManager : public CT_HandleOutPointColor, public CT_SparsePointColorManager
{
public:
    CT_HandleOutPointColorWithSparseManager() = default;

    CT_PointsAttributesColor* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesColor(pcir, *this);
    }
};

#endif // CT_HANDLEOUTPOINTCOLOR_H
