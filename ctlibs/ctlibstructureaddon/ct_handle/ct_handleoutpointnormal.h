#ifndef CT_HANDLEOUTPOINTNORMAL_H
#define CT_HANDLEOUTPOINTNORMAL_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_pointsattributesnormal.h"
#include "ct_attributes/managers/ct_densepointnormalmanager.h"
#include "ct_attributes/managers/ct_sparsepointnormalmanager.h"

class CT_HandleOutPointNormal : public CT_HandleOutItem<CT_PointsAttributesNormal>
{
public:
    enum EApplicableTo
    {
        ApplicableToPoint = 1
    };

    CT_HandleOutPointNormal() = default;
};

class CT_HandleOutPointNormalWithDenseManager : public CT_HandleOutPointNormal, public CT_DensePointNormalManager
{
public:
    CT_HandleOutPointNormalWithDenseManager() = default;

    CT_PointsAttributesNormal* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesNormal(pcir, *this);
    }
};

class CT_HandleOutPointNormalWithSparseManager : public CT_HandleOutPointNormal, public CT_SparsePointNormalManager
{
public:
    CT_HandleOutPointNormalWithSparseManager() = default;

    CT_PointsAttributesNormal* createAttributeInstance(CT_PCIR pcir)
    {
        return new CT_PointsAttributesNormal(pcir, *this);
    }
};

#endif // CT_HANDLEOUTPOINTNORMAL_H
