#include "ct_abstractactionfortreeview.h"

CT_AbstractActionForTreeView::CT_AbstractActionForTreeView() : CT_AbstractAction()
{
    m_treeView = nullptr;
}

void CT_AbstractActionForTreeView::setTreeView(const TreeViewInterface *view)
{
    m_treeView = const_cast<TreeViewInterface*>(view);
}

TreeViewInterface *CT_AbstractActionForTreeView::treeView() const
{
    return m_treeView;
}
