#include "ct_stditemattributewrappert.h"

template <class ClassType, typename VType>
CT_StdItemAttributeWrapperT<ClassType,VType>::CT_StdItemAttributeWrapperT() : SuperClass(),
    m_gMethod(nullptr)
{
}

template <class ClassType, typename VType>
CT_StdItemAttributeWrapperT<ClassType,VType>::CT_StdItemAttributeWrapperT(const CT_AbstractCategory* category,
                                                                          getter gMethod) : SuperClass(category),
    m_gMethod(gMethod)
{
    static_assert(std::is_convertible<ClassType*, CT_AbstractItem*>::value, "CT_StdItemAttributeWrapperT is only compatible with classes that inherit from CT_AbstractItem");
}

template <class ClassType, typename VType>
CT_AbstractItemAttribute* CT_StdItemAttributeWrapperT<ClassType, VType>::copy(const DEF_CT_OutAbstractIAModel* model, const CT_AbstractResult* result) const
{
    auto c = new CT_StdItemAttributeWrapperT<ClassType,VType>(*this);
    c->setModel(model);
    c->setResult(result);

    return c;
}

template <class ClassType, typename VType>
VType CT_StdItemAttributeWrapperT<ClassType,VType>::data(const CT_AbstractItem* item) const
{
    return (static_cast<const ClassType*>(item)->*m_gMethod)();
}
