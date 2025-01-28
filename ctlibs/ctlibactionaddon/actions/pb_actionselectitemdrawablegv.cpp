#include "pb_actionselectitemdrawablegv.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QPainter>
#define _USE_MATH_DEFINES
#include <math.h>

#include "views/actions/pb_actionselectitemdrawablegvoptions.h"
#include "documentinterface.h"

PB_ActionSelectItemDrawableGV::PB_ActionSelectItemDrawableGV() : CT_AbstractActionForGraphicsView()
{
    m_status = 0;
    m_selectionMode = GraphicsViewInterface::SELECT_ONE;
    m_selectionTool = Point;
    m_keyModifiers = Qt::NoModifier;

    connect(&m_rectangleTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()), Qt::DirectConnection);
    connect(&m_polygonTools, SIGNAL(mustBeRedraw()), this, SLOT(redrawOverlay()), Qt::DirectConnection);
}

QString PB_ActionSelectItemDrawableGV::title() const
{
    return tr("Sélection");
}

QString PB_ActionSelectItemDrawableGV::description() const
{
    return tr("Sélection d'éléments");
}

QIcon PB_ActionSelectItemDrawableGV::icon() const
{
    return QIcon(":/icons/cursor.png");
}

QString PB_ActionSelectItemDrawableGV::type() const
{
    return CT_AbstractAction::TYPE_SELECTION;
}

void PB_ActionSelectItemDrawableGV::init()
{
    CT_AbstractActionForGraphicsView::init();

    if(nOptions() == 0)
    {
        // create the option widget if it was not already created
        PB_ActionSelectItemDrawableGVOptions *option = new PB_ActionSelectItemDrawableGVOptions(this);

        // add the options to the graphics view
        graphicsView()->addActionOptions(option);

        // register the option to the superclass, so the hideOptions and showOptions
        // is managed automatically
        registerOption(option);
    }
}

bool PB_ActionSelectItemDrawableGV::mousePressEvent(QMouseEvent *e)
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

bool PB_ActionSelectItemDrawableGV::mouseMoveEvent(QMouseEvent *e)
{
    if(m_status > 0)
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.mouseMoveEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.mouseMoveEvent(e);

        m_status = 0;
    }

    return false;
}

bool PB_ActionSelectItemDrawableGV::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_status > 0)
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.mouseReleaseEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.mouseReleaseEvent(e);

        pick();
    }

    return false;
}

bool PB_ActionSelectItemDrawableGV::wheelEvent(QWheelEvent *e)
{
    if(m_status > 0)
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.wheelEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.wheelEvent(e);
    }

    return false;
}

bool PB_ActionSelectItemDrawableGV::keyPressEvent(QKeyEvent *e)
{
    setKeyModifiers(e->modifiers());

    if(m_status > 0) {
        if(selectionTool() == Polygon)
            return m_polygonTools.keyPressEvent(e);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.keyPressEvent(e);
    }

    return false;
}

bool PB_ActionSelectItemDrawableGV::keyReleaseEvent(QKeyEvent *e)
{
    setKeyModifiers(e->modifiers());

    if(m_status > 0) {

        if(selectionTool() == Polygon)
        {
            if(e->key() == Qt::Key_Delete)
            {
                m_polygonTools.removeLastPoint();
                return true;

            }

            if((e->key() == Qt::Key_Enter)
                      || (e->key() == Qt::Key_Return))
            {
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
            }

            if((e->key() == Qt::Key_Enter)
                    || (e->key() == Qt::Key_Return)) {
                pick();
                return true;
            }
        }
    }

    return false;
}

void PB_ActionSelectItemDrawableGV::drawOverlay(GraphicsViewInterface &view, QPainter &painter)
{
    Q_UNUSED(view)

    if(m_status > 0)
    {
        if(selectionTool() == Polygon)
            return m_polygonTools.drawOverlay(painter);
        else if(selectionTool() == Rectangle)
            return m_rectangleTools.drawOverlay(painter);
    }
}

bool PB_ActionSelectItemDrawableGV::setSelectionMode(GraphicsViewInterface::SelectionMode mode)
{
    if(m_selectionMode != mode) {
        m_selectionMode = mode;

        emit selectionModeChanged(selectionMode());
    }

    return true;
}

bool PB_ActionSelectItemDrawableGV::setSelectionTool(PB_ActionSelectItemDrawableGV::SelectionTool tool)
{
    m_polygonTools.clear();
    m_rectangleTools.clear();

    m_selectionTool = tool;
    return true;
}

void PB_ActionSelectItemDrawableGV::toggleSelection()
{
    graphicsView()->setSelectionMode(selectionMode());

    if(graphicsView()->mustSelectPoints())
        graphicsView()->togglePointsSelected();
    else if(graphicsView()->mustSelectEdges())
        graphicsView()->toggleEdgesSelected();
    else if(graphicsView()->mustSelectFaces())
        graphicsView()->toggleFacesSelected();
    else if(graphicsView()->mustSelectItems())
        graphicsView()->toggleItemsSelected();
}

GraphicsViewInterface::SelectionMode PB_ActionSelectItemDrawableGV::selectionMode() const
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

PB_ActionSelectItemDrawableGV::SelectionTool PB_ActionSelectItemDrawableGV::selectionTool() const
{
    return m_selectionTool;
}

GraphicsViewInterface::SelectionMode PB_ActionSelectItemDrawableGV::selectionModeToBasic(GraphicsViewInterface::SelectionMode mode) const
{
    int m = mode;

    while(m > GraphicsViewInterface::REMOVE_ONE)
        m -= GraphicsViewInterface::REMOVE_ONE;

    return static_cast<GraphicsViewInterface::SelectionMode>(m);
}

void PB_ActionSelectItemDrawableGV::setKeyModifiers(Qt::KeyboardModifiers m)
{
    if(m_keyModifiers != m) {
        m_keyModifiers = m;
        emit selectionModeChanged(selectionMode());
    }
}

void PB_ActionSelectItemDrawableGV::pick()
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

    graphicsView()->setSelectionMode(selectionMode());
    graphicsView()->select(final);

    m_status = 0;
}

void PB_ActionSelectItemDrawableGV::redrawOverlay()
{
    document()->redrawGraphics();
}
