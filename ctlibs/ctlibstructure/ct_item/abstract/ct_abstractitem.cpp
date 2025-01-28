#include "ct_abstractitem.h"

#include "ct_result/abstract/ct_abstractresult.h"

CT_AbstractItem::CT_AbstractItem()
{
    m_result = nullptr;
    m_model = nullptr;
}

CT_AbstractItem::CT_AbstractItem(const CT_AbstractItem& other) : QObject(nullptr)
{
    m_result = other.m_result;
    m_model = other.m_model;
}

CT_AbstractItem::~CT_AbstractItem()
{
}

CT_OutAbstractModel* CT_AbstractItem::model() const
{
    return m_model;
}

CT_AbstractResult* CT_AbstractItem::result() const
{
    return m_result;
}

bool CT_AbstractItem::visitChildrensForTreeView(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    return visitChildrens(visitor);
}

bool CT_AbstractItem::recursiveVisitChildrens(const CT_AbstractItem::ChildrensVisitor& visitor) const
{
    const auto visitorAdapter = [&visitor](const CT_AbstractItem* child) -> bool {
        if(!visitor(child))
            return false;

        return child->recursiveVisitChildrens(visitor);
    };

    return visitChildrens(visitorAdapter);
}

CT_AbstractItem::ChildrensCollection CT_AbstractItem::childrensThatUseOutModel(const CT_OutAbstractModel* outModel) const
{
    return ChildrensCollection(createQtStyleIteratorForChildrensThatUseOutModel(outModel));
}

void CT_AbstractItem::setResult(const CT_AbstractResult* newRes)
{
    m_result = const_cast<CT_AbstractResult*>(newRes);
}

void CT_AbstractItem::willBeRemovedFromResult(const CT_AbstractResult*)
{
    // Nothing to do here
}

void CT_AbstractItem::setModel(const CT_OutAbstractModel* model)
{
    m_model = const_cast<CT_OutAbstractModel*>(model);
}
