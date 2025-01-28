#ifndef CT_ABSTRACTPOINTSATTRIBUTES_H
#define CT_ABSTRACTPOINTSATTRIBUTES_H

#include "ct_itemdrawable/abstract/ct_abstractattributes.h"

/**
 * @brief Represents a cloud of attributes for points.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractPointsAttributes : public CT_AbstractAttributes
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractPointsAttributes, CT_AbstractAttributes, Point attributes)

    using SuperClass = CT_AbstractAttributes;

public:
    CT_AbstractPointsAttributes();

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
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractPointsAttributes(const CT_AbstractPointsAttributes& other) = default;
};

#endif // CT_ABSTRACTPOINTSATTRIBUTES_H
