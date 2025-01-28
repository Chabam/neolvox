#include "ct_itemdrawablehierarchycollectionmodel.h"

CT_ItemDrawableHierarchyCollectionModel::CT_ItemDrawableHierarchyCollectionModel()
{
}

CT_ItemDrawableHierarchyCollectionModel::~CT_ItemDrawableHierarchyCollectionModel()
{
    qDeleteAll(m_models.begin(), m_models.end());
}

void CT_ItemDrawableHierarchyCollectionModel::addModel(CT_ItemDrawableHierarchyCollectionSelectionModel *model)
{
    m_models.append(model);
}

const QList<CT_ItemDrawableHierarchyCollectionSelectionModel *> &CT_ItemDrawableHierarchyCollectionModel::models() const
{
    return m_models;
}
