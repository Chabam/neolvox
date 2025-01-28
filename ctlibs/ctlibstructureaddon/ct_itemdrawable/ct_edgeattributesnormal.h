#ifndef CT_EDGEATTRIBUTESNORMAL_H
#define CT_EDGEATTRIBUTESNORMAL_H

#include "ct_itemdrawable/abstract/ct_abstractedgeattributes.h"
#include "ct_attributes/ct_attributesnormal.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_EdgeAttributesNormal : public CT_AttributesNormal<CT_AbstractEdgeAttributes>
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_EdgeAttributesNormal, CT_AbstractEdgeAttributes, Edge normal attributes)

    using SuperClass = CT_AttributesNormal<CT_AbstractEdgeAttributes>;

public:
    CT_EdgeAttributesNormal() = default;


    template<class ManagerT>
    CT_EdgeAttributesNormal(CT_ECIR ecir,
                            ManagerT& manager) :
        SuperClass(ecir,
                   manager)
    {
        static_assert (SFINAE_And_<IsAEdgeCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, CT_Normal>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to edge");
    }

    CT_EdgeAttributesNormal(const CT_EdgeAttributesNormal& other) = default;

    size_t numberOfSetValues() const final { return SuperClass::numberOfSetValues(); }

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return normalsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_EdgeAttributesNormal)

private:
    CT_DEFAULT_IA_BEGIN(CT_EdgeAttributesNormal)
    CT_DEFAULT_IA_V2(CT_EdgeAttributesNormal, CT_AbstractCategory::staticInitDataSize(), &CT_EdgeAttributesNormal::numberOfSetValues, QObject::tr("Taille"))
    CT_DEFAULT_IA_END(CT_EdgeAttributesNormal)
};

#endif // CT_EDGEATTRIBUTESNORMAL_H
