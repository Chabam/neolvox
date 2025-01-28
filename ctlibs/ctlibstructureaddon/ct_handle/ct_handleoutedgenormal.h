#ifndef CT_HANDLEOUTEDGENORMAL_H
#define CT_HANDLEOUTEDGENORMAL_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_edgeattributesnormal.h"
#include "ct_attributes/managers/ct_denseedgenormalmanager.h"
#include "ct_attributes/managers/ct_sparseedgenormalmanager.h"

class CT_HandleOutEdgeNormal : public CT_HandleOutItem<CT_EdgeAttributesNormal>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleOutEdgeNormal() = default;
};

class CT_HandleOutEdgeNormalWithDenseManager : public CT_HandleOutEdgeNormal, public CT_DenseEdgeNormalManager
{
public:
    CT_HandleOutEdgeNormalWithDenseManager() = default;

    CT_EdgeAttributesNormal* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesNormal(ecir, *this);
    }
};

class CT_HandleOutEdgeNormalWithSparseManager : public CT_HandleOutEdgeNormal, public CT_SparseEdgeNormalManager
{
public:
    CT_HandleOutEdgeNormalWithSparseManager() = default;

    CT_EdgeAttributesNormal* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesNormal(ecir, *this);
    }
};

#endif // CT_HANDLEOUTEDGENORMAL_H
