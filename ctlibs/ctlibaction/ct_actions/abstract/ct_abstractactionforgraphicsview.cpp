#include "ct_abstractactionforgraphicsview.h"

#include "ct_actions/view/abstract/ct_gabstractactionforgraphicsviewoptions.h"

CT_AbstractActionForGraphicsView::CT_AbstractActionForGraphicsView() : CT_AbstractAction()
{
    m_gView = nullptr;
}

void CT_AbstractActionForGraphicsView::setGraphicsView(const GraphicsViewInterface *view)
{
    if(m_gView != view)
    {
        m_gView = const_cast<GraphicsViewInterface*>(view);

        graphicsViewChangedEvent();
    }
}

GraphicsViewInterface* CT_AbstractActionForGraphicsView::graphicsView() const
{
    return m_gView;
}

void CT_AbstractActionForGraphicsView::registerOption(CT_GAbstractActionOptions *options)
{
    CT_GAbstractActionForGraphicsViewOptions *gvOptions = dynamic_cast<CT_GAbstractActionForGraphicsViewOptions*>(options);

    if(gvOptions != nullptr)
        connect(gvOptions, SIGNAL(drawing3DChanged()), this, SLOT(setDrawing3DChanged()));

    CT_AbstractAction::registerOption(options);
}

void CT_AbstractActionForGraphicsView::setDrawing3DChanged()
{
    emit drawing3DChanged();
}

void CT_AbstractActionForGraphicsView::setNew3DElementDraw(const quint32 &uniqueIndex)
{
    emit new3DElementDraw(uniqueIndex);
}
