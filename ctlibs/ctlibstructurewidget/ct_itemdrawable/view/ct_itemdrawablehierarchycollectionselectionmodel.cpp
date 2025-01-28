#include "ct_itemdrawablehierarchycollectionselectionmodel.h"

CT_ItemDrawableHierarchyCollectionSelectionModel::CT_ItemDrawableHierarchyCollectionSelectionModel(const QString &name)
{
    m_name = name;
}

QString CT_ItemDrawableHierarchyCollectionSelectionModel::name() const
{
    return m_name;
}

void CT_ItemDrawableHierarchyCollectionSelectionModel::setCollection(const QList<CT_ItemDrawableCollectionHierarchyStep> &collection)
{
    m_collection = collection;
}

const QList<CT_ItemDrawableCollectionHierarchyStep>& CT_ItemDrawableHierarchyCollectionSelectionModel::collection() const
{
    return m_collection;
}

void CT_ItemDrawableHierarchyCollectionSelectionModel::addExcludeModel(const int &index)
{
    m_exclude.append(index);
}

void CT_ItemDrawableHierarchyCollectionSelectionModel::clearExcludeModel()
{
    m_exclude.clear();
}

bool CT_ItemDrawableHierarchyCollectionSelectionModel::mustExcludeModel(const int &index) const
{
    return m_exclude.contains(index);
}

QList<int> CT_ItemDrawableHierarchyCollectionSelectionModel::excludedModels() const
{
    return m_exclude;
}
