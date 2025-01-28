#ifndef CT_FACEATTRIBUTESSCALART_H
#define CT_FACEATTRIBUTESSCALART_H

#include "ct_itemdrawable/abstract/ct_abstractfaceattributesscalar.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_attributes/ct_attributesscalart.h"

template<typename SCALAR>
class CT_FaceAttributesScalarT : public CT_AttributesScalarT<SCALAR, CT_AbstractFaceAttributesScalar>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_FaceAttributesScalarT, SCALAR, CT_AbstractFaceAttributesScalar, Face %1 attributes)
    using SuperClass = CT_AttributesScalarT<SCALAR, CT_AbstractFaceAttributesScalar>;

public:
    CT_FaceAttributesScalarT();

    template<class ManagerT>
    CT_FaceAttributesScalarT(CT_FCIR fcir,
                             ManagerT& manager) :
        SuperClass(fcir,
                   manager)
    {
        static_assert(SFINAE_And_<IsAFaceCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to face");
    }

    template<class ManagerT>
    CT_FaceAttributesScalarT(CT_FCIR fcir,
                             ManagerT& manager,
                             const SCALAR& min,
                             const SCALAR& max) :
        SuperClass(fcir,
                   manager,
                   min,
                                 max)
    {
        static_assert(SFINAE_And_<IsAFaceCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to face");
    }

    CT_FaceAttributesScalarT(const CT_FaceAttributesScalarT<SCALAR>& other) = default;

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return SuperClass::scalarsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_FaceAttributesScalarT<SCALAR>)
};

#endif // CT_FACEATTRIBUTESSCALART_H
