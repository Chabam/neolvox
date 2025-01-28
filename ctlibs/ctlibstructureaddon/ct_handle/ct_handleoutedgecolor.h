#ifndef CT_HANDLEOUTEDGECOLOR_H
#define CT_HANDLEOUTEDGECOLOR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_edgeattributescolor.h"
#include "ct_attributes/managers/ct_denseedgecolormanager.h"
#include "ct_attributes/managers/ct_sparseedgecolormanager.h"

class CT_HandleOutEdgeColor : public CT_HandleOutItem<CT_EdgeAttributesColor>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleOutEdgeColor() = default;
};

class CT_HandleOutEdgeColorWithDenseManager : public CT_HandleOutEdgeColor, public CT_DenseEdgeColorManager
{
public:
    CT_HandleOutEdgeColorWithDenseManager() = default;

    CT_EdgeAttributesColor* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesColor(ecir, *this);
    }
};

class CT_HandleOutEdgeColorWithSparseManager : public CT_HandleOutEdgeColor, public CT_SparseEdgeColorManager
{
public:
    CT_HandleOutEdgeColorWithSparseManager() = default;

    CT_EdgeAttributesColor* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesColor(ecir, *this);
    }
};

#endif // CT_HANDLEOUTEDGECOLOR_H
