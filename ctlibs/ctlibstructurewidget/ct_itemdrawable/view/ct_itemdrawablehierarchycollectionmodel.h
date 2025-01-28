#ifndef CT_ITEMDRAWABLEHIERARCHYCOLLECTIONMODEL_H
#define CT_ITEMDRAWABLEHIERARCHYCOLLECTIONMODEL_H

#include "ct_itemdrawablehierarchycollectionselectionmodel.h"

#include <QList>

/**
 * @brief Use this class to create a list of CT_ItemDrawableHierarchyCollectionSelectionModel and use them
 *        in a CT_ItemDrawableHierarchyCollectionWidget
 */
class CTLIBSTRUCTUREWIDGET_EXPORT CT_ItemDrawableHierarchyCollectionModel
{
public:
    CT_ItemDrawableHierarchyCollectionModel();
    ~CT_ItemDrawableHierarchyCollectionModel();

    /**
     * @brief Add a selection model to the collection
     */
    void addModel(CT_ItemDrawableHierarchyCollectionSelectionModel* model);

    /**
     * @brief Returns the collection of models
     */
    const QList<CT_ItemDrawableHierarchyCollectionSelectionModel*>& models() const;

private:
    QList<CT_ItemDrawableHierarchyCollectionSelectionModel*> m_models;
};

#endif // CT_ITEMDRAWABLEHIERARCHYCOLLECTIONMODEL_H
