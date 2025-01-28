#ifndef CT_ITEMDRAWABLEHIERARCHYCOLLECTIONSELECTIONMODEL_H
#define CT_ITEMDRAWABLEHIERARCHYCOLLECTIONSELECTIONMODEL_H

#include "ct_itemdrawable/tools/collection/ct_itemdrawablecollectionhierarchystep.h"

/**
 * @brief Use this class to set a collection of CT_ItemDrawableCollectionHierarchyStep (itemdrawable sorted by
 *        step and result that can be selected). The selection is not exclusive. If you want that the selection is
 *        exclusive show information on method "addExcludeModel"
 */
class CTLIBSTRUCTUREWIDGET_EXPORT CT_ItemDrawableHierarchyCollectionSelectionModel
{
public:
    /**
     * @brief Create a new selection model that will be show with the title : name
     *
     * NAME
     *  |
     *  |-- StepA
     *  |     |---- ResultA
     *  |     |       |--- Item1
     *  |     |       |--- Item2
     *  |-- StepB
     *  |     |---- ResultX
     *  |     |       |--- Item8
     *  etc....
     */
    CT_ItemDrawableHierarchyCollectionSelectionModel(const QString &name);

    /**
     * @brief The name to use in the widget to show the selection model
     */
    QString name() const;

    /**
     * @brief Set the collection of itemdrawable sorted by result and step
     */
    void setCollection(const QList<CT_ItemDrawableCollectionHierarchyStep> &collection);
    const QList<CT_ItemDrawableCollectionHierarchyStep>& collection() const;

    /**
     * @brief Exclude a selection model.
     *
     *        When you add a selection model to the collection (CT_ItemDrawableHierarchyCollectionModel) it will be indexed (index in the list). You
     *        can choose to exclude certains selection model when a item of this selection model is checked.
     *
     *        INFORMATION : Add '-1' to exclude model if you want that when an element is selected on this selection model, the selection model for
     *                      color cloud is deselected.
     *
     *                      Add 'index of this selection model' to exclude model if you want that elements of this selection model must be
     *                      exclusive (Only one element of this selection model is selected at one time).
     *
     * @example Create three selection model and you want that when an itemdrawable of the first selection model is selected all items from the third
     *          selection model is deselected. You must add '0' on exclude model of the third selection model.
     */
    void addExcludeModel(const int &index);

    /**
     * @brief Clear all excluded model
     */
    void clearExcludeModel();

    /**
     * @brief Returns true if must exclude model passed in parameter
     */
    bool mustExcludeModel(const int &index) const;

    /**
     * @brief Returns the list of excluded model
     */
    QList<int> excludedModels() const;

private:
    QList<CT_ItemDrawableCollectionHierarchyStep>   m_collection;
    QList<int>                                      m_exclude;
    QString                                         m_name;
};

#endif // CT_ITEMDRAWABLEHIERARCHYCOLLECTIONSELECTIONMODEL_H
