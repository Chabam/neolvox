#ifndef CT_ABSTRACTEDGEATTRIBUTESSCALAR_H
#define CT_ABSTRACTEDGEATTRIBUTESSCALAR_H

#include "ct_itemdrawable/abstract/ct_abstractedgeattributes.h"
#include "ct_attributes/abstract/ct_abstractattributesscalar.h"

/**
 * @brief Represents a cloud of attributes of scalar type (int, float, double, etc...) for edges.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractEdgeAttributesScalar : public CT_AbstractEdgeAttributes, public CT_AbstractAttributesScalar
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractEdgeAttributesScalar, CT_AbstractEdgeAttributes, Edge attributes)

public:
    CT_AbstractEdgeAttributesScalar() = default;

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
     *          - Shared pointer to edge cloud index registered
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractEdgeAttributesScalar(const CT_AbstractEdgeAttributesScalar& other) = default;
};

#endif // CT_ABSTRACTEDGEATTRIBUTESSCALAR_H
