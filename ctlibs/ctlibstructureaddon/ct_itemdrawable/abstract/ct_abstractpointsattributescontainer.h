#ifndef CT_ABSTRACTPOINTSATTRIBUTESCONTAINER_H
#define CT_ABSTRACTPOINTSATTRIBUTESCONTAINER_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractPointsAttributesContainer : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractPointsAttributesContainer, CT_AbstractSingularItemDrawable, Point attributes list)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_AbstractPointsAttributesContainer();
    CT_AbstractPointsAttributesContainer(const CT_AbstractPointsAttributesContainer& other) = default;

    /**
     * @brief Returns the CT_AbstractPointAttributesScalar at key defined in parameter
     */
    virtual CT_AbstractPointsAttributes* pointsAttributesAt(const int &key) const;

    virtual void clearPointsAttributesCollection();

private:
    QMultiHash<int, CT_AbstractPointsAttributes*>   m_att;

protected:
    /**
     * @brief Insert a CT_AbstractPointsAttributes at key in the map
     */
    void internalInsertAttributes(const int &key, const CT_AbstractPointsAttributes *att);

    /**
     * @brief Returns the map
     */
    const QMultiHash<int, CT_AbstractPointsAttributes*>& internalAttributes() const;
};

#endif // CT_ABSTRACTPOINTSATTRIBUTESCONTAINER_H
