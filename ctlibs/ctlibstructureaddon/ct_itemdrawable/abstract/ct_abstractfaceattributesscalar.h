#ifndef CT_ABSTRACTFACEATTRIBUTESSCALAR_H
#define CT_ABSTRACTFACEATTRIBUTESSCALAR_H

#include "ct_itemdrawable/abstract/ct_abstractfaceattributes.h"
#include "ct_attributes/abstract/ct_abstractattributesscalar.h"

/**
 * @brief Represents a cloud of attributes of scalar type (int, float, double, etc...) for faces.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractFaceAttributesScalar : public CT_AbstractFaceAttributes, public CT_AbstractAttributesScalar
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractFaceAttributesScalar, CT_AbstractFaceAttributes, Face attributes)

public:
    CT_AbstractFaceAttributesScalar() = default;

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
     *          - Shared pointer to face cloud index registered
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractFaceAttributesScalar(const CT_AbstractFaceAttributesScalar& other) = default;
};

#endif // CT_ABSTRACTFACEATTRIBUTESSCALAR_H
