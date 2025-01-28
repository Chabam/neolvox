#ifndef CT_ABSTRACTFACEATTRIBUTES_H
#define CT_ABSTRACTFACEATTRIBUTES_H

#include "ct_defines.h"
#include "ct_itemdrawable/abstract/ct_abstractattributes.h"

/**
 * @brief Represents a cloud of attributes for faces.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractFaceAttributes : public CT_AbstractAttributes
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractFaceAttributes, CT_AbstractAttributes, Face attributes)

    using SuperClass = CT_AbstractAttributes;

public:
    CT_AbstractFaceAttributes();

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
    CT_AbstractFaceAttributes(const CT_AbstractFaceAttributes& other) = default;
};

#endif // CT_ABSTRACTFACEATTRIBUTES_H
