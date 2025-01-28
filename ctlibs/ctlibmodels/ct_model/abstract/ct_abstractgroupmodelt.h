#ifndef CT_ABSTRACTGROUPMODELT_H
#define CT_ABSTRACTGROUPMODELT_H

#include "ct_abstractmodel.h"

/**
 * @brief Represent a model for a group (IN or OUT). A group can have for childrens other groups and singular items.
 *
 *        Use the template parameters to define if you want an IN or an OUT model
 */
template< class ItemT, class InheritedT = CT_AbstractModel>
class CT_AbstractGroupModelT : public InheritedT
{
public:
    typedef CT_AbstractGroupModelT<ItemT,InheritedT>    GroupType;
    typedef QList<GroupType*>                           GroupCollection;
    typedef ItemT                                       ItemType;
    typedef QList<ItemType*>                            ItemCollection;

    typedef std::function<bool(const GroupType*)>       GroupsVisitor;
    typedef std::function<bool(const ItemType*)>        ItemsVisitor;

    CT_AbstractGroupModelT(const QString& displayableName);
    CT_AbstractGroupModelT(const CT_AbstractGroupModelT<ItemT, InheritedT >& other) : InheritedT(other) {}
    ~CT_AbstractGroupModelT();

    /**
     * @brief Returns the root group
     */
    GroupType* rootGroup() const;

    /**
     * @brief Returns the parent group
     */
    GroupType* parentGroup() const;

    /**
     * @brief Add a new group to this model
     * @param group : the group to add
     */
    void addGroup(GroupType* group);

    /**
     * @brief Remove a group from this model and from memory
     * @param group : the group to remove
     * @return Returns true if the group was founded and removed
     */
    bool removeGroup(GroupType* group);

    /**
     * @brief Add a new item to this model
     * @param item : the item to add
     */
    void addItem(ItemT* item);

    /**
     * @brief Remove an item from this model and from memory
     * @param item : the item to remove
     * @return Returns true if the item was founded and removed
     */
    bool removeItem(ItemT* item);

    /**
     * @brief Returns a const reference to the collection of groups
     */
    const GroupCollection& groups() const;

    /**
     * @brief Call it to visit the childrens group of this model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  GroupType and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitGroups(const GroupsVisitor& visitor) const;

    /**
     * @brief Call it to visit the childrens item of this model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  ItemType and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitItems(const ItemsVisitor& visitor) const;

    /**
     * @brief Returns a const reference to the collection of items
     */
    const ItemCollection& items() const;

    /**
     * @brief Remove all groups from this model and from memory
     */
    void clearGroups();

    /**
     * @brief Remove all items from this model and from memory
     */
    void clearItems();

    /**
     * @brief Remove all groups and items from this model and from memory
     */
    void clear();

    // INHERITED FROM CT_AbstractModel
    bool visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const override;
    bool isEmpty() const override;

protected:
    /**
     * @brief Called when a group is added
     */
    virtual void groupAdded(const GroupType* group) { Q_UNUSED(group) }

    /**
     * @brief Called when a item is added
     */
    virtual void itemAdded(const ItemT* item) { Q_UNUSED(item) }

    /**
     * @brief Called when a group will be removed
     */
    virtual void groupRemoved(const GroupType* group) { Q_UNUSED(group) }

    /**
     * @brief Called when a item will be removed
     */
    virtual void itemRemoved(const ItemT* item) { Q_UNUSED(item) }

private:
    GroupCollection     _groups;
    ItemCollection      _items;
};

// Includes the template implementations
#include "ct_abstractgroupmodelt.hpp"

#endif // CT_ABSTRACTGROUPMODELT_H
