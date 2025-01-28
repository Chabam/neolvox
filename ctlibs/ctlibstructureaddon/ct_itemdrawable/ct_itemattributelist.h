#ifndef CT_ITEMATTRIBUTELIST_H
#define CT_ITEMATTRIBUTELIST_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

/**
 * @brief A singular item that contains item attributes
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_ItemAttributeList : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_ItemAttributeList, CT_AbstractSingularItemDrawable, Item attribute list)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_ItemAttributeList() = default;

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
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_ItemAttributeList(const CT_ItemAttributeList& other) = default;

    CT_ITEM_COPY_IMP(CT_ItemAttributeList)
};

#endif // CT_ITEMATTRIBUTELIST_H
