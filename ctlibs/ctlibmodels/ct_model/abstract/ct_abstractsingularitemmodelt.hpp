#include "ct_abstractsingularitemmodelt.h"
#include <QDebug>

template <class GroupModelT, class AttributModelT, class InheritedT>
CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::CT_AbstractSingularItemModelT(const QString &displayableName) : InheritedT(displayableName)
{
}

template <class GroupModelT, class AttributModelT, class InheritedT>
CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::~CT_AbstractSingularItemModelT()
{
    clearItemAttributes();
}

template <class GroupModelT, class AttributModelT, class InheritedT>
void CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::addItemAttribute(AttributModelT* attribute)
{
    MODELS_ASSERT(attribute != nullptr);
    if (attribute == nullptr) {qDebug() << "CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::addItemAttribute" << ", " <<  "attribute == nullptr"; return;}

    attribute->setParentModel(this);
    m_attributes.append(attribute);
    itemAttributeAdded(attribute);
}

template <class GroupModelT, class AttributModelT, class InheritedT>
bool CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::removeItemAttribute(AttributModelT *attribute)
{
    MODELS_ASSERT(attribute != nullptr);
    if (attribute == nullptr) {qDebug() << "CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::removeItemAttribute" << ", " <<  "attribute == nullptr"; return false;}

    if(m_attributes.removeOne(attribute))
    {
        itemAttributeRemoved(attribute);

        delete attribute;
        return true;
    }

    return false;
}

template <class GroupModelT, class AttributModelT, class InheritedT>
bool CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::visitAttributes(const typename InheritedT::AttributeVisitor& visitor) const
{
    for(const AttributModelT* attribute : m_attributes) {
        if(!visitor(attribute))
            return false;
    }

    return true;
}

template <class GroupModelT, class AttributModelT, class InheritedT>
void CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::clearItemAttributes()
{
    for(const AttributModelT* attribute : m_attributes) {
        itemAttributeRemoved(attribute);
        delete attribute;
    }

    m_attributes.clear();
}

template <class GroupModelT, class AttributModelT, class InheritedT>
GroupModelT* CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::parentGroup() const
{
    return static_cast<GroupModelT*>(this->parentModel());
}

template <class GroupModelT, class AttributModelT, class InheritedT>
bool CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const
{
    for(const AttributModelT* attribute : m_attributes) {
        if(!visitor(attribute))
            return false;
    }

    return true;
}

template <class GroupModelT, class AttributModelT, class InheritedT>
bool CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::isEmpty() const
{
    return m_attributes.isEmpty();
}

template <class GroupModelT, class AttributModelT, class InheritedT>
GroupModelT* CT_AbstractSingularItemModelT<GroupModelT, AttributModelT, InheritedT>::rootGroup() const
{
    GroupModelT* pGroup = parentGroup();

    if(pGroup == nullptr)
        return nullptr;

    return static_cast<GroupModelT*>(pGroup->rootGroup());
}
