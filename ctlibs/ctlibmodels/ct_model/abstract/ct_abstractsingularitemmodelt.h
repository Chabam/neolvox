#ifndef CT_ABSTRACTSINGULARITEMMODELT_H
#define CT_ABSTRACTSINGULARITEMMODELT_H

#include "ct_abstractmodel.h"

/**
 * @brief Represent a model for an singular item (IN or OUT). A singular item can only have item attributes for childrens.
 *
 *        Use the template parameters to define if you want a IN or OUT model
 */
template <class GroupModelT, class AttributModelT, class InheritedT = CT_AbstractModel>
class CT_AbstractSingularItemModelT : public InheritedT
{
public:
    CT_AbstractSingularItemModelT(const QString& displayableName);

    /**
     * @brief Copy constructor. Doesn't copy attributes and parent model !
     */
    CT_AbstractSingularItemModelT(const CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>& other) : InheritedT(other) {}
    ~CT_AbstractSingularItemModelT();

    /**
     * @brief Add a model of item attribute
     * @warning No verification is made to check if the unique index is already present or not !
     */
    void addItemAttribute(AttributModelT* attribute);

    /**
     * @brief Remove from collection and delete from memory the item attribute from this item (compare address)
     * @param attribute : item attribute to remove
     * @return Returns true if the item attribute has been found and deleted
     */
    bool removeItemAttribute(AttributModelT *attribute);

    /**
     * @brief Call it to visit all attributes of this model
     */
    bool visitAttributes(const typename InheritedT::AttributeVisitor& visitor) const override;

    /**
     * @brief Remove from the collection and delete from memory all item attributes
     */
    void clearItemAttributes();

    /*!
     * @brief Returns the parent group. nullptr if it was not set.
     */
    GroupModelT* parentGroup() const;

    /*!
     * @brief Returns the root group. nullptr if this item has no parent group.
     */
    GroupModelT* rootGroup() const;

    // INHERITED FROM CT_AbstractModel
    bool visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const override;
    bool isEmpty() const override;

protected:
    /**
     * @brief Called when a item attribute is added to the list
     */
    virtual void itemAttributeAdded(const AttributModelT *att) { Q_UNUSED(att) }

    /**
     * @brief Called when a item attribute is removed from the list
     */
    virtual void itemAttributeRemoved(const AttributModelT *att) { Q_UNUSED(att) }

private:
    /**
     * @brief The collection of attributes
     */
    QList<AttributModelT*>          m_attributes;
};

#include "ct_abstractsingularitemmodelt.hpp"

#endif // CT_ABSTRACTSINGULARITEMMODELT_H
