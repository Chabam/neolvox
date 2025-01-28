#ifndef CT_STDPOINTSATTRIBUTESCONTAINER_H
#define CT_STDPOINTSATTRIBUTESCONTAINER_H

#include "ct_itemdrawable/abstract/ct_abstractpointsattributescontainer.h"

/**
 *  This class is a standard container for points attributes
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_StdPointsAttributesContainer : public CT_AbstractPointsAttributesContainer
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_StdPointsAttributesContainer, CT_AbstractPointsAttributesContainer, Point attributes list)

    using SuperClass = CT_AbstractPointsAttributesContainer;

public:
    CT_StdPointsAttributesContainer();
    CT_StdPointsAttributesContainer(const CT_StdPointsAttributesContainer& other) = default;

    /**
     * @brief Insert the CT_AbstractPointsAttributes at key 'key'. If an attribute already exist it will be replaced.
     *        The attribute is not deleted from memory, you are the owner.
     */
    void insertPointsAttributesAt(const int &key, const CT_AbstractPointsAttributes *att);

    CT_ITEM_COPY_IMP(CT_StdPointsAttributesContainer)
};

#endif // CT_STDPOINTSATTRIBUTESCONTAINER_H
