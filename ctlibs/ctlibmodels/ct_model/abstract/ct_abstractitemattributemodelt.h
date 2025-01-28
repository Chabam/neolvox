#ifndef CT_ABSTRACTITEMATTRIBUTEMODELT_H
#define CT_ABSTRACTITEMATTRIBUTEMODELT_H

#include "ct_abstractmodel.h"

/**
 * @brief Represent a model for an item attribute (IN or OUT). A item attribute has no children so the method isEmpty returns always true.
 *
 *        Use the template parameters to define if you want a IN or OUT model
 */
template<class ItemModelT, class InheritedT = CT_AbstractModel>
class CT_AbstractItemAttributeModelT : public InheritedT
{
public:
    CT_AbstractItemAttributeModelT(const QString& displayableName);
    CT_AbstractItemAttributeModelT(const CT_AbstractItemAttributeModelT<ItemModelT, InheritedT>& other) : InheritedT(other) {}

    // INHERITED FROM CT_AbstractModel
    /**
     * @brief Redefined to do nothing and returns true because item attributes doesn't have childrens
     */
    bool visitChildrens(const typename InheritedT::ChildrenVisitor& visitor) const override;

    /**
     * @brief Redefined to returns true because item attributes doesn't have childrens
     */
    bool isEmpty() const override;
};

#include "ct_abstractitemattributemodelt.hpp"

#endif // CT_ABSTRACTITEMATTRIBUTEMODELT_H
