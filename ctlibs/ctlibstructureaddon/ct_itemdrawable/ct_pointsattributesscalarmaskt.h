#ifndef CT_POINTSATTRIBUTESSCALARMASKT_H
#define CT_POINTSATTRIBUTESSCALARMASKT_H

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_attributes/abstract/ct_abstractattributesscalar.h"
#include "ct_attributes/managers/abstract/ct_abstractxattributemanager.h"
#include "ct_attributes/ct_attributesscalart.h"

/**
 *  This class is like a CT_PointsAttributesScalarTemplated but the difference is that the cloud of information
 *  can mask the information of the element in cloud to get the real information.
 *
 *  @example You must create a structure with the 'MASK' typedef :
 *
 *           struct myStruct { // information of a point
 *              typedef quint8 MASK; // typedef of the mask type // OBLIGATORY in your struct and named 'MASK'
 *
 *              quint8 entire;   // first element is contained in 3 bits and second element in 5 bits // OBLIGATORY in your struct and named 'entire'
 *           };
 *
 *           // This manager use the structure that contains all informations
 *           CT_DensePointScalarManager<myStruct> manager;
 *
 *           // Set 6 for the first element and 4 for the second element
 *           myStruct tmp;
 *           tmp.entire = 196; (equal 6 for the first and 4 for the second if use the mask)
 *
 *           auto setter = manager.createAttributesSetter(pcir);
 *           setter.setValueWithGlobalIndex(globalIndex, tmp);
 *
 *           // Create two CT_PointsAttributesScalarMaskT
 *           // The first must contains a pointer to the vector and delete it when it will be destroyed from memory
 *           // The second contains also a pointer to the vector but doesn't delete it when it will be destroyed from memory
 *           CT_PointsAttributesScalarMaskT<myStruct> firstElement(224, 5, pcir, manager);    // the mask is '0b11100000' in binary, and shift right is 5
 *           CT_PointsAttributesScalarMaskT<myStruct> secondElement(31, 0, pcir, manager);     // the mask is '0b00011111' in binary, and shift right is 0
 *
 *           const double first = firstElement.scalarAsDoubleAt(globalIndex);       // first == 6
 *           const double second = secondElement.scalarAsDoubleAt(globalIndex);     // second == 4
 */
template<typename StructType>
class CT_PointsAttributesScalarMaskT : public CT_AttributesScalarT<typename StructType::MASK, CT_AbstractPointAttributesScalar, StructType>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_PointsAttributesScalarMaskT, StructType, CT_AbstractPointAttributesScalar, Point %1 attributes)

    using SuperClass = CT_AttributesScalarT<typename StructType::MASK, CT_AbstractPointAttributesScalar, StructType>;

public:
    typedef typename StructType::MASK StructMASK;

    /**
     * @brief The visitor receive the global index of the attribute and the attribute. The visitor
     *        must returns true if the visit must continue or false to abort it.
     */
    using SVisitor = std::function<bool (const size_t& /*globalIndex*/, StructMASK /*value*/)>;

    CT_PointsAttributesScalarMaskT();

    template<class ManagerT>
    CT_PointsAttributesScalarMaskT(StructMASK mask,
                                   quint16 shiftRight,
                                   CT_PCIR pcir,
                                   ManagerT& manager) :
        SuperClass(pcir,
                   manager),
        m_mask(mask),
        m_shiftRight(shiftRight)
    {
        static_assert(SFINAE_And_<IsAPointCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, StructType>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to point");
    }

    template<class ManagerT>
    CT_PointsAttributesScalarMaskT(StructMASK mask,
                                   quint16 shiftRight,
                                   const StructMASK& min,
                                   const StructMASK& max,
                                   CT_PCIR pcir,
                                   ManagerT& manager) :
        SuperClass(pcir,
                   manager,
                   min,
                   max),
        m_mask(mask),
        m_shiftRight(shiftRight)
    {
        static_assert(SFINAE_And_<IsAPointCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, StructType>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to point");
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
     *          - Pointer of the collection
     *          - Mask
     *          - Shift Right
     *          - Min and max value
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     *          - Auto delete collection is set to false
     */
    CT_PointsAttributesScalarMaskT(const CT_PointsAttributesScalarMaskT<StructType>& other);

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return SuperClass::scalarsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_PointsAttributesScalarMaskT<StructType>)

    quint64 mask() const final;

private:
    StructMASK                                          m_mask;
    quint16                                             m_shiftRight;

protected:
    StructMASK convertScalarOfManagerToScalar(const StructType& value) const final;
};

#include "ct_itemdrawable/ct_pointsattributesscalarmaskt.hpp"

#endif // CT_POINTSATTRIBUTESSCALARMASKT_H
