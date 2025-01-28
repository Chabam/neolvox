#ifndef CT_POINTSATTRIBUTESNORMAL_H
#define CT_POINTSATTRIBUTESNORMAL_H

#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"
#include "ct_attributes/ct_attributesnormal.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_PointsAttributesNormal : public CT_AttributesNormal<CT_AbstractPointsAttributes>
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_PointsAttributesNormal, CT_AbstractPointsAttributes, Normal point attributes)

    using SuperClass = CT_AttributesNormal<CT_AbstractPointsAttributes>;

public:
    CT_PointsAttributesNormal() = default;

    template<class ManagerT>
    CT_PointsAttributesNormal(CT_PCIR pcir,
                              ManagerT& manager) : SuperClass(pcir, manager)
    {
        static_assert(SFINAE_And_<IsAPointCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, CT_Normal>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to point");
    }

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Shared pointer to point cloud index registered
     *          - Normal cloud is copied (call method "copy()") if not nullptr
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_PointsAttributesNormal(const CT_PointsAttributesNormal& other) = default;

    size_t numberOfSetValues() const final { return SuperClass::numberOfSetValues(); }

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return normalsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_PointsAttributesNormal)

private:
    CT_DEFAULT_IA_BEGIN(CT_PointsAttributesNormal)
    CT_DEFAULT_IA_V2(CT_PointsAttributesNormal, CT_AbstractCategory::staticInitDataSize(), &CT_PointsAttributesNormal::numberOfSetValues, QObject::tr("Taille"))
    CT_DEFAULT_IA_END(CT_PointsAttributesNormal)
};

#endif // CT_POINTSATTRIBUTESNORMAL_H
