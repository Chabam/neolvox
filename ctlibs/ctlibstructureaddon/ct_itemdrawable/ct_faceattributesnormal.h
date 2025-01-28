#ifndef CT_FACEATTRIBUTESNORMAL_H
#define CT_FACEATTRIBUTESNORMAL_H

#include "ct_itemdrawable/abstract/ct_abstractfaceattributes.h"
#include "ct_attributes/ct_attributesnormal.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_FaceAttributesNormal : public CT_AttributesNormal<CT_AbstractFaceAttributes>
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_FaceAttributesNormal, CT_AbstractFaceAttributes, Face normal attributes)
    using SuperClass = CT_AttributesNormal<CT_AbstractFaceAttributes>;

public:
    CT_FaceAttributesNormal() = default;

    template<class ManagerT>
    CT_FaceAttributesNormal(CT_FCIR fcir,
                            ManagerT& manager) :
        SuperClass(fcir,
                   manager)
    {
        static_assert(SFINAE_And_<IsAFaceCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, CT_Normal>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to face");
    }

    CT_FaceAttributesNormal(const CT_FaceAttributesNormal& other) = default;

    size_t numberOfSetValues() const override { return CT_AttributesNormal::numberOfSetValues(); }

    bool hasBeenSet(const size_t& globalIndex) const override { return CT_AttributesNormal::hasBeenSet(globalIndex); }

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return normalsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_FaceAttributesNormal)


private:
    CT_DEFAULT_IA_BEGIN(CT_FaceAttributesNormal)
    CT_DEFAULT_IA_V2(CT_FaceAttributesNormal, CT_AbstractCategory::staticInitDataSize(), &CT_FaceAttributesNormal::numberOfSetValues, QObject::tr("Taille"))
    CT_DEFAULT_IA_END(CT_FaceAttributesNormal)
};

#endif // CT_FACEATTRIBUTESNORMAL_H
