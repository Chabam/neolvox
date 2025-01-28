#include "ct_abstractgroupmodelt.h"

template< class ItemT, class InheritedT >
CT_AbstractGroupModelT<ItemT,InheritedT>::CT_AbstractGroupModelT(const QString &displayableName) : InheritedT(displayableName)
{
}

template< class ItemT, class InheritedT >
CT_AbstractGroupModelT<ItemT,InheritedT>::~CT_AbstractGroupModelT()
{
    clear();
}

template< class ItemT, class InheritedT >
typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupType* CT_AbstractGroupModelT<ItemT,InheritedT>::rootGroup() const
{
    GroupType* parent = const_cast<GroupType*>(this);
    GroupType* p = nullptr;

    // on remonte tout en haut de l'arbre
    while((p = parent->parentGroup()) != nullptr)
        parent = p;

    return parent;
}

template< class ItemT, class InheritedT >
typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupType* CT_AbstractGroupModelT<ItemT,InheritedT>::parentGroup() const
{
    return dynamic_cast<GroupType*>(this->parentModel());
}

template< class ItemT, class InheritedT >
void CT_AbstractGroupModelT<ItemT,InheritedT>::addGroup(typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupType* group)
{
    group->setParentModel(this);
    _groups.append(group);

    groupAdded(group);
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::removeGroup(typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupType* group)
{
    if(_groups.removeOne(group))
    {
        groupRemoved(group);

        delete group;
        return true;
    }

    return false;
}

template< class ItemT, class InheritedT >
void CT_AbstractGroupModelT<ItemT,InheritedT>::addItem(ItemT* item)
{
    item->setParentModel(this);
    _items.append(item);

    itemAdded(item);
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::removeItem(ItemT* item)
{
    if(_items.removeOne(item))
    {
        itemRemoved(item);

        delete item;
        return true;
    }

    return false;
}

template< class ItemT, class InheritedT >
const typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupCollection& CT_AbstractGroupModelT<ItemT,InheritedT>::groups() const
{
    return _groups;
}

template< class ItemT, class InheritedT >
const typename CT_AbstractGroupModelT<ItemT,InheritedT>::ItemCollection& CT_AbstractGroupModelT<ItemT,InheritedT>::items() const
{
    return _items;
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::visitGroups(const typename CT_AbstractGroupModelT<ItemT,InheritedT>::GroupsVisitor& visitor) const
{
    for(GroupType* group : _groups) {
        if(!visitor(group))
            return false;
    }

    return true;
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::visitItems(const typename CT_AbstractGroupModelT<ItemT,InheritedT>::ItemsVisitor& visitor) const
{
    for(ItemT* item : _items) {
        if(!visitor(item))
            return false;
    }

    return true;
}

template< class ItemT, class InheritedT >
void CT_AbstractGroupModelT<ItemT,InheritedT>::clearGroups()
{
    for(GroupType* group : _groups) {
        groupRemoved(group);
        delete group;
    }

    _groups.clear();
}

template< class ItemT, class InheritedT >
void CT_AbstractGroupModelT<ItemT,InheritedT>::clearItems()
{
    for(ItemT* item : _items) {
        itemRemoved(item);
        delete item;
    }

    _items.clear();
}

template< class ItemT, class InheritedT >
void CT_AbstractGroupModelT<ItemT,InheritedT>::clear()
{
    clearGroups();
    clearItems();
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const
{
    for(GroupType* group : _groups) {
        if(!visitor(group))
            return false;
    }

    for(ItemT* item : _items) {
        if(!visitor(item))
            return false;
    }

    return true;
}

template< class ItemT, class InheritedT >
bool CT_AbstractGroupModelT<ItemT,InheritedT>::isEmpty() const
{
    return (_groups.isEmpty() && _items.isEmpty());
}
