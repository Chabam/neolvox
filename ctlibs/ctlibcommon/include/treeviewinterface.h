#ifndef TREEVIEWINTERFACE_H
#define TREEVIEWINTERFACE_H

#include "indocumentviewinterface.h"
#include <QAbstractItemView>

class CT_AbstractItemDrawable;

/**
 * @brief Represent a view that show a tree model
 */
class TreeViewInterface : public InDocumentViewInterface
{
public:
    virtual ~TreeViewInterface() {}

    /**
     * @brief Convert a QModelIndex to a CT_AbstractItemDrawable. Can return nullptr if the QModelIndex is not valid.
     */
    virtual CT_AbstractItemDrawable* itemDrawableFromIndex(const QModelIndex &index) const = 0;

    /**
     * @brief Returns the model index of the item at the viewport coordinates point.
     */
    virtual QModelIndex indexAt(const QPoint &point) const = 0;

    /**
     * @brief Returns the viewport widget.
     */
    virtual QWidget* treeViewport() const = 0;

    /**
     * @brief This method controls whether the user can select one or many items
     *        and, in many-item selections, whether the selection must
     *        be a continuous range of items.
     */
    virtual void setSelectionMode(QAbstractItemView::SelectionMode mode) = 0;

    /**
     * @brief Returns the current selection mode
     */
    virtual QAbstractItemView::SelectionMode selectionMode() const = 0;

    /**
     * @brief Returns selected indexes. The list contains no duplicates, and is not sorted.
     */
    virtual QModelIndexList selectedIndexes () const = 0;

    /**
     * @brief Returns the current selection model.
     */
    virtual QItemSelectionModel* selectionModel() const = 0;

    /**
     * @brief Refresh the tree view
     */
    virtual void refreshAll() = 0;

    /**
     * @brief Refresh only children of the list of QModelIndex passed in parameter
     */
    virtual void refreshItems(const QList<QModelIndex> &indexes) = 0;

    /**
     * @brief Search the QModelIndex corresponding to the ItemDrawable passed in parameter
     *        and refresh children for all ItemDrawable in the list
     * @warning slow ! prefer the method "refreshItems" with QModelIndex or "refreshAll"
     */
    virtual void refreshItems(const QList<CT_AbstractItemDrawable*> &items) = 0;
};

#endif //TREEVIEWINTERFACE_H
