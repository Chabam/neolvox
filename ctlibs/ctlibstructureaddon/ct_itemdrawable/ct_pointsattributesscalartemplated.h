#ifndef CT_POINTSATTRIBUTESSCALARTEMPLATED_H
#define CT_POINTSATTRIBUTESSCALARTEMPLATED_H

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_attributes/ct_attributesscalart.h"

/**
 *  Points attributes of scalar type (int, float, double, etc...)
 */
template<typename SCALAR>
class CT_PointsAttributesScalarTemplated : public CT_AttributesScalarT<SCALAR, CT_AbstractPointAttributesScalar>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_PointsAttributesScalarTemplated, SCALAR, CT_AbstractPointAttributesScalar, Point %1 attributes)

    using SuperClass = CT_AttributesScalarT<SCALAR, CT_AbstractPointAttributesScalar>;

public:
    CT_PointsAttributesScalarTemplated() = default;

    /**
     * @brief Create a collection of SCALAR with min and max values processed in constructor.
     */
    template<class ManagerT>
    CT_PointsAttributesScalarTemplated(CT_PCIR pcir,
                                       ManagerT& manager) : SuperClass(pcir,
                                                                       manager)
    {
        static_assert(SFINAE_And_<IsAPointCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to point");
    }

    /**
     * @brief Create a collection of SCALAR with min and max value set.
     */
    template<class ManagerT>
    CT_PointsAttributesScalarTemplated(CT_PCIR pcir,
                                       ManagerT& manager,
                                       const SCALAR& min,
                                       const SCALAR& max) : SuperClass(pcir,
                                                                       manager,
                                                                       min,
                                                                       max)
    {
        static_assert(SFINAE_And_<IsAPointCloudManager(ManagerT), IsABaseOfCT_AbstractXAttributeManager<ManagerT, SCALAR>>::value, "The manager you attempt to set in constructor is not a base of CT_AbstractXAttributeManager or is not applicable to point");
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
     *          - Collection is copied if not nullptr
     *          - Min and max value are copied
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_PointsAttributesScalarTemplated(const CT_PointsAttributesScalarTemplated<SCALAR>& other) = default;

    bool copyAttributesOfSForD(CT_CIR source, CT_CIR destination) final { return SuperClass::scalarsManager()->copyAndModifyAttributesOfSForD(source, destination); }

    CT_ITEM_COPY_IMP(CT_PointsAttributesScalarTemplated<SCALAR>)
};

#include "ct_itemdrawable/ct_pointsattributesscalartemplated.hpp"

#endif // CT_POINTSATTRIBUTESSCALARTEMPLATED_H
