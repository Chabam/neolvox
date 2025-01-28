#ifndef CT_HANDLEOUTEDGESCALAR_H
#define CT_HANDLEOUTEDGESCALAR_H

#include "ct_model/outModel/handle/ct_handleoutitem.h"
#include "ct_itemdrawable/ct_edgeattributesscalart.h"
#include "ct_attributes/managers/ct_denseedgescalarmanager.h"
#include "ct_attributes/managers/ct_sparseedgescalarmanager.h"

template<typename SCALAR>
class CT_HandleOutEdgeScalar : public CT_HandleOutItem<CT_EdgeAttributesScalarT<SCALAR>>
{
public:
    enum EApplicableTo
    {
        ApplicableToEdge = 1
    };

    CT_HandleOutEdgeScalar() = default;
};

template<typename SCALAR>
class CT_HandleOutEdgeScalarWithDenseManager : public CT_HandleOutEdgeScalar<SCALAR>, public CT_DenseEdgeScalarManager<SCALAR>
{
public:
    CT_HandleOutEdgeScalarWithDenseManager() = default;

    CT_EdgeAttributesScalarT<SCALAR>* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesScalarT<SCALAR>(ecir, *this);
    }

    CT_EdgeAttributesScalarT<SCALAR>* createAttributeInstance(CT_ECIR ecir,
                                                              const SCALAR& min,
                                                              const SCALAR& max)
    {
        return new CT_EdgeAttributesScalarT<SCALAR>(ecir, *this, min, max);
    }
};

template<typename SCALAR>
class CT_HandleOutEdgeScalarWithSparseManager : public CT_HandleOutEdgeScalar<SCALAR>, public CT_SparseEdgeScalarManager<SCALAR>
{
public:
    CT_HandleOutEdgeScalarWithSparseManager() = default;

    CT_EdgeAttributesScalarT<SCALAR>* createAttributeInstance(CT_ECIR ecir)
    {
        return new CT_EdgeAttributesScalarT<SCALAR>(ecir, *this);
    }

    CT_EdgeAttributesScalarT<SCALAR>* createAttributeInstance(CT_ECIR ecir,
                                                              const SCALAR& min,
                                                              const SCALAR& max)
    {
        return new CT_EdgeAttributesScalarT<SCALAR>(ecir, *this, min, max);
    }
};

#endif // CT_HANDLEOUTEDGESCALAR_H
