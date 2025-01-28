#ifndef CT_ABSTRACTACTIONFORTREEVIEW_H
#define CT_ABSTRACTACTIONFORTREEVIEW_H

#include "ct_actions/abstract/ct_abstractaction.h"
#include "treeviewinterface.h"

/**
 * @brief Represent an action for a tree view
 *
 *        To convert a QModelIndex to a QStandardItem use the method "itemFromIndex" on
 *        a TreeViewInterface
 *
 *        To convert a QStandardItem to a ItemDrawable use the method "itemDrawableFromItem" on
 *        a TreeViewInterface
 *
 * @warning Test if the ItemDrawable returned is not nullptr !
 *
 *        If you want to make drag&drop see the drag&drop example http://qt-project.org/doc/qt-4.8/dnd.html
 *
 */
class CTLIBACTION_EXPORT CT_AbstractActionForTreeView : public CT_AbstractAction
{
public:
    CT_AbstractActionForTreeView();
    virtual ~CT_AbstractActionForTreeView() {}

    /**
     * @brief Define the view that show the tree model. (A document can have multiple tree view)
     *        Your action is copied and each copy is set to a TreeView
     */
    void setTreeView(const TreeViewInterface *view);

    /**
     * @brief indexActivatedEvent is called when the item specified by index is activated by the
     *        user. How to activate items depends on the platform; e.g., by single- or
     *        double-clicking the item, or by pressing the Return or Enter key when
     *        the item is current.
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool indexActivatedEvent(const QModelIndex &index) { Q_UNUSED(index) return true; }

    /**
     * @brief indexClickedEvent is called when a mouse button is clicked. The item
     *        the mouse was clicked on is specified by index.
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool indexClickedEvent(const QModelIndex &index) { Q_UNUSED(index) return true; }

    /**
     * @brief indexDoubleClickedEvent is called when a mouse button is double-clicked. The
     *        item the mouse was double-clicked on is specified by index.
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool indexDoubleClickedEvent(const QModelIndex &index) { Q_UNUSED(index) return true; }

    /**
     * @brief indexCollapsedEvent is called when the item specified by index is collapsed.
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool indexCollapsedEvent(const QModelIndex &index) { Q_UNUSED(index) return true; }

    /**
     * @brief indexExpandedEvent is called when the item specified by index is expanded.
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool indexExpandedEvent(const QModelIndex &index) { Q_UNUSED(index) return true; }

    /**
     * @brief itemDragEnterEvent is called when a drag is in progress and the mouse enters the treeview
     * @param event : you must call "acceptProposedAction" method to accept the drag.
     *
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool itemDragEnterEvent(QDragEnterEvent *event) { Q_UNUSED(event) return true; }

    /**
     * @brief itemDragMoveEvent is called if a drag is in progress, and when any of the following conditions occur: the cursor
     *        enters the treeview, the cursor moves within the treeview, or a modifier key is pressed on the keyboard while the treeview has the focus.
     *
     * @param event : same as dragEnterEvent
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool itemDragMoveEvent(QDragMoveEvent *event) { Q_UNUSED(event) return true; }

    /**
     * @brief itemDragLeaveEvent is called when a drag is in progress and the mouse leaves the treeview.
     *
     * @param event : same as dragEnterEvent
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool itemDragLeaveEvent(QDragLeaveEvent *event) { Q_UNUSED(event) return true; }

    /**
     * @brief itemDropEvent is called when the drag is dropped on the treeview.
     * @param event : same as dragEnterEvent
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool itemDropEvent(QDropEvent *event) { Q_UNUSED(event) return true; }

    /**
     * @brief redefine this method if you want to show your custom context menu
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool showContextMenu(const QPoint &point) { Q_UNUSED(point) return false; }

    /**
     * @brief redefine this method if you want to show your custom context menu on the horizontal header
     * @return Return true if you do not want the event to be propagated.
     */
    virtual bool showContextMenuOnHorizontalHeader(const QPoint &point) { Q_UNUSED(point) return false; }

    /**
     * @brief Returns the treeview used by this action
     */
    TreeViewInterface* treeView() const;

private:
    TreeViewInterface   *m_treeView;
};

#endif // CT_ABSTRACTACTIONFORTREEVIEW_H
