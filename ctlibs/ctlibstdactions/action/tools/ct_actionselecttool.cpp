#include "ct_actionselecttool.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QPainter>
#define _USE_MATH_DEFINES
#include <math.h>

#include "views/actions/ct_actionselectitemdrawablegvoptions.h"


CT_ActionSelectTool::CT_ActionSelectTool()
{
    m_status = 0;
    m_selectionMode = GraphicsViewInterface::SELECT_ONE;
    m_selectionTool = Point;
    m_keyModifiers = Qt::NoModifier;

    m_view = nullptr;
    m_option = nullptr;

    connect(&m_rectangleTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()), Qt::DirectConnection);
    connect(&m_polygonTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()), Qt::DirectConnection);
}

CT_ActionSelectTool::~CT_ActionSelectTool()
{
    disconnect(&m_rectangleTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()));
    disconnect(&m_polygonTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()));
}

void CT_ActionSelectTool::init(GraphicsViewInterface* view, CT_ActionSelectItemDrawableGVOptions* option)
{
    m_view = view;
    m_option = option;

}

bool CT_ActionSelectTool::mousePressEvent(QMouseEvent *e)
{
    if((m_status == 0) && (e->button() != Qt::LeftButton))
        return false;

    bool ok = false;

    if(selectionTool() == Polygon)
        ok = m_polygonTools.mousePressEvent(e);
    else if(selectionTool() == Rectangle)
        ok = m_rectangleTools.mousePressEvent(e);
    else if(e->button() == Qt::LeftButton) {
        m_lastMousePos = e->pos();
        m_status = 1;
        return false;
    }

    m_status = ok ? 1 : 0;

    return ok;
}

bool CT_ActionSelectTool::mouseMoveEvent(QMouseEvent *e)
{
    if(hasUserBeginAnAction())
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.mouseMoveEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.mouseMoveEvent(e);
        else {
            m_status = 0;
            return false;
        }

        return true;
    }

    return false;
}

bool CT_ActionSelectTool::mouseReleaseEvent(QMouseEvent *e)
{
    if(hasUserBeginAnAction())
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.mouseReleaseEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.mouseReleaseEvent(e);
        else {
            pick();
            return false;
        }
        return true;
    }

    return false;
}

bool CT_ActionSelectTool::wheelEvent(QWheelEvent *e)
{
    if(hasUserBeginAnAction())
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.wheelEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.wheelEvent(e);

        return false;
    }

    return false;
}

bool CT_ActionSelectTool::keyPressEvent(QKeyEvent *e)
{
    setKeyModifiers(e->modifiers());

    if(hasUserBeginAnAction()) {
        if(selectionTool() == Polygon)
            return m_polygonTools.keyPressEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.keyPressEvent(e);
    }

    return false;
}

bool CT_ActionSelectTool::keyReleaseEvent(QKeyEvent *e)
{
    setKeyModifiers(e->modifiers());

    if(hasUserBeginAnAction()) {

        if(selectionTool() == Polygon) {

            if(e->key() == Qt::Key_Delete) {

                m_polygonTools.removeLastPoint();
                return true;

            } else if((e->key() == Qt::Key_Enter)
                      || (e->key() == Qt::Key_Return)) {

                if(!m_polygonTools.isPolygonClosed()) {
                    m_polygonTools.closePolygon();
                    return true;
                }

            }
        }

        if((selectionTool() == Rectangle)
                || (selectionTool() == Polygon)) {

            if(e->key() == Qt::Key_Escape) {
                m_polygonTools.clear();
                m_rectangleTools.clear();
                m_status = 0;
                return true;
            } else if((e->key() == Qt::Key_Enter)
                    || (e->key() == Qt::Key_Return)) {

                pick();
                return true;
            }
        }
    }

    return false;
}

void CT_ActionSelectTool::drawOverlay(GraphicsViewInterface &view, QPainter &painter)
{
    Q_UNUSED(view)

    if(hasUserBeginAnAction())
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.drawOverlay(painter);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.drawOverlay(painter);

    }
}


bool CT_ActionSelectTool::setSelectionMode(GraphicsViewInterface::SelectionMode mode)
{
    if(m_selectionMode != mode) {
        m_selectionMode = mode;

        emit selectionModeChanged(selectionMode());
    }

    return true;
}

bool CT_ActionSelectTool::setSelectionTool(CT_ActionSelectTool::SelectionTool tool)
{
    m_polygonTools.clear();
    m_rectangleTools.clear();

    m_selectionTool = tool;
    return true;
}

void CT_ActionSelectTool::toggleSelection()
{
    m_view->setSelectionMode(selectionMode());

    if(m_view->mustSelectPoints())
        m_view->togglePointsSelected();
    else if(m_view->mustSelectEdges())
        m_view->toggleEdgesSelected();
    else if(m_view->mustSelectFaces())
        m_view->toggleFacesSelected();
    else if(m_view->mustSelectItems())
        m_view->toggleItemsSelected();
}

GraphicsViewInterface::SelectionMode CT_ActionSelectTool::selectionMode() const
{
    GraphicsViewInterface::SelectionMode basic = selectionModeToBasic(m_selectionMode);

    if(m_keyModifiers.testFlag(Qt::ShiftModifier)) {

        if((basic == GraphicsViewInterface::SELECT) || (basic == GraphicsViewInterface::SELECT_ONE))
           return GraphicsViewInterface::SelectionMode(m_selectionMode + 1); // SELECT -> ADD
        else if((basic == GraphicsViewInterface::REMOVE) || (basic == GraphicsViewInterface::REMOVE_ONE))
            return GraphicsViewInterface::SelectionMode(m_selectionMode - 1); // REMOVE -> ADD

    } else if(m_keyModifiers.testFlag(Qt::ControlModifier)) {
        if((basic == GraphicsViewInterface::SELECT) || (basic == GraphicsViewInterface::SELECT_ONE))
           return GraphicsViewInterface::SelectionMode(m_selectionMode + 2); // SELECT -> REMOVE
        else if((basic == GraphicsViewInterface::ADD) || (basic == GraphicsViewInterface::ADD_ONE))
            return GraphicsViewInterface::SelectionMode(m_selectionMode + 1); // ADD -> REMOVE
    }

    return m_selectionMode;
}

CT_ActionSelectTool::SelectionTool CT_ActionSelectTool::selectionTool() const
{
    return m_selectionTool;
}

bool CT_ActionSelectTool::hasUserBeginAnAction() const
{
    return m_status > 0;
}

GraphicsViewInterface::SelectionMode CT_ActionSelectTool::selectionModeToBasic(GraphicsViewInterface::SelectionMode mode) const
{
    int m = mode;

    while(m > GraphicsViewInterface::REMOVE_ONE)
        m -= GraphicsViewInterface::REMOVE_ONE;

    return (GraphicsViewInterface::SelectionMode)m;
}

void CT_ActionSelectTool::setKeyModifiers(Qt::KeyboardModifiers m)
{
    if(m_keyModifiers != m) {
        m_keyModifiers = m;
        emit selectionModeChanged(selectionMode());
    }
}

void CT_ActionSelectTool::pick()
{
    QPolygon final;

    if(selectionTool() == Rectangle)
        final = m_rectangleTools.getPolygon();
    else if(selectionTool() == Polygon)
        final = m_polygonTools.getPolygon();
    else {
        QRect r(0, 0, 3, 3);
        r.moveCenter(m_lastMousePos);
        final = r;
    }

    m_polygonTools.clear();
    m_rectangleTools.clear();

    setSelectionTool(CT_ActionSelectTool::Point);

    if(m_option != nullptr)
        m_option->setSelectionTool(CT_ActionSelectItemDrawableGV::Point);

    m_view->setSelectionMode(selectionMode());
    m_view->select(final);

    m_status = 0;

    emit pickingFinished();
}

void CT_ActionSelectTool::redrawOverlay()
{
    emit needRedrawOverlay();
}
