#ifndef CT_EDGEATTRIBUTESSCALART_H
#define CT_EDGEATTRIBUTESSCALART_H

#include "ct_itemdrawable/abstract/ct_abstractedgeattributesscalar.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_attributes/ct_attributesscalart.h"

template<typename SCALAR>
class CT_EdgeAttributesScalarT : public CT_AttributesScalarT<SCALAR, CT_AbstractEdgeAttributesScalar>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_EdgeAttributesScalarT, SCALAR, CT_AbstractEdgeAttributesScalar, Edge %1 attributes)

    using SuperClass = CT_AttributesScalarT<SCALAR, CT_AbstractEdgeAttributesScalar>;

public:
    CT_EdgeAttributesScalarT() = default;

    template<class ManagerT>
    CT_EdgeAttributesScalarT(CT_ECIR ecir,
                             ManagerT& manager) :
        SuperClass(ecir,
                   manager)
    {
        static_assert (SFINAE_And_<IsAEdgeCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to edge");
    }

    template<class ManagerT>
    CT_EdgeAttributesScalarT(CT_ECIR ecir,
                             ManagerT& manager,
                             const SCALAR& min,
                             const SCALAR& max) :
        SuperClass(ecir,
                   manager,
                   min,
                   max)
    {
        static_assert (SFINAE_And_<IsAEdgeCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to edge");
    }

    CT_EdgeAttributesScalarT(const CT_EdgeAttributesScalarT<SCALAR>& other) = default;

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return SuperClass::scalarsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_EdgeAttributesScalarT<SCALAR>)
};

#endif // CT_EDGEATTRIBUTESSCALART_H
