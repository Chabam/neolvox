#include "ct_abstractitemattributemodelt.h"

template <class ItemModelT, class InheritedT>
CT_AbstractItemAttributeModelT<ItemModelT, InheritedT>::CT_AbstractItemAttributeModelT(const QString &displayableName) : InheritedT(displayableName)
{
}

template <class ItemModelT, class InheritedT>
bool CT_AbstractItemAttributeModelT<ItemModelT, InheritedT>::visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const
{
    Q_UNUSED(visitor)

    return true;
}

template <class ItemModelT, class InheritedT>
bool CT_AbstractItemAttributeModelT<ItemModelT, InheritedT>::isEmpty() const
{
    return true;
}
