#include "pb_actionpickitemsinlist.h"

#include "documentinterface.h"
#include "painterinterface.h"
#include "ct_math/ct_mathpoint.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QIcon>
#include <QPainter>
#include <limits>

#include <Eigen/Core>

PB_ActionPickItemsInList::PB_ActionPickItemsInList(const QList<CT_AbstractGeometricalItem *> &itemList,
                                                   QList<CT_AbstractGeometricalItem *>* selectedItems,
                                                   double maxDist) : SuperClass()
{
    _itemList.append(itemList);
    _selectedItems = selectedItems;
    _maxDist = maxDist;
}

PB_ActionPickItemsInList::~PB_ActionPickItemsInList()
{
}

QString PB_ActionPickItemsInList::title() const
{
    return tr("Picking d'item");
}

QString PB_ActionPickItemsInList::description() const
{
    return tr("Picking d'item");
}

QIcon PB_ActionPickItemsInList::icon() const
{
    return QIcon(":/icons/select_rectangular.png");
}

QString PB_ActionPickItemsInList::type() const
{
    return CT_AbstractAction::TYPE_INFORMATION;
}

void PB_ActionPickItemsInList::init()
{
    SuperClass::init();

    if(nOptions() == 0)
    {
        // create the option widget if it was not already created
        PB_ActionPickItemsInListOptions* option = new PB_ActionPickItemsInListOptions(this);

        // add the options to the graphics view
        graphicsView()->addActionOptions(option);

        connect(option, SIGNAL(parametersChanged()), this, SLOT(redraw()));
        connect(option, SIGNAL(allClicked()), this, SLOT(selectAll()));
        connect(option, SIGNAL(noneClicked()), this, SLOT(selectNone()));

        // register the option to the superclass, so the hideOptions and showOptions
        // is managed automatically
        registerOption(option);

        document()->redrawGraphics();
    }
}

void PB_ActionPickItemsInList::redraw()
{
    setDrawing3DChanged();
}

void PB_ActionPickItemsInList::selectAll()
{
    _selectedItems->clear();
    _selectedItems->append(_itemList);
    redraw();
}

void PB_ActionPickItemsInList::selectNone()
{
    _selectedItems->clear();
    redraw();
}


bool PB_ActionPickItemsInList::mousePressEvent(QMouseEvent *e)
{
    _oldPos = e->pos();

    if (e->modifiers() & Qt::ShiftModifier)
    {
        _buttonsPressed = e->buttons();
        return true;
    } else  {
        _buttonsPressed = Qt::NoButton;
    }
    return false;
}

bool PB_ActionPickItemsInList::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    return false;
}

bool PB_ActionPickItemsInList::mouseReleaseEvent(QMouseEvent *e)
{   
    PB_ActionPickItemsInListOptions *option = static_cast<PB_ActionPickItemsInListOptions*>(optionAt(0));

    QPoint point = e->pos() - _oldPos;

    if (point.manhattanLength() <= 3)
    {
        if (_buttonsPressed == Qt::LeftButton)
        {
            CT_AbstractGeometricalItem* item = addItemToSelection(e->pos());
            if ((item != nullptr) && option->shouldAutoCenterCamera())
            {
                graphicsView()->camera()->setCX(item->centerX());
                graphicsView()->camera()->setCY(item->centerY());
                graphicsView()->camera()->setCZ(item->centerZ());
            }
            redraw();
            return true;
        } else if (_buttonsPressed == Qt::RightButton){
            CT_AbstractGeometricalItem* item = removeItemFromSelection(e->pos());

            if ((item != nullptr) && option->shouldAutoCenterCamera())
            {
                graphicsView()->camera()->setCX(item->centerX());
                graphicsView()->camera()->setCY(item->centerY());
                graphicsView()->camera()->setCZ(item->centerZ());
            }
            redraw();
            return true;
        }
    }
    return false;
}

bool PB_ActionPickItemsInList::wheelEvent(QWheelEvent *e)
{
    Q_UNUSED(e);
    return false;

}

bool PB_ActionPickItemsInList::keyPressEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    return false;

}

bool PB_ActionPickItemsInList::keyReleaseEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    return false;
}

void PB_ActionPickItemsInList::draw(GraphicsViewInterface &view, PainterInterface &painter)
{
    Q_UNUSED(view)

    PB_ActionPickItemsInListOptions *option = static_cast<PB_ActionPickItemsInListOptions*>(optionAt(0));

    painter.save();

    QListIterator<CT_AbstractGeometricalItem*> it(_itemList);
    while (it.hasNext())
    {
        CT_AbstractGeometricalItem *item = it.next();

        if (_selectedItems->contains(item))
        {
            painter.setColor(QColor(255,0,0));
        } else {
            painter.setColor(option->getColor());
        }
        item->draw(view, painter);
    }

    painter.restore();
}

void PB_ActionPickItemsInList::drawOverlay(GraphicsViewInterface &view, QPainter &painter)
{
    Q_UNUSED(view)
    Q_UNUSED(painter)
}

CT_AbstractAction* PB_ActionPickItemsInList::createInstance() const
{
    return new PB_ActionPickItemsInList(_itemList, _selectedItems, _maxDist);
}

CT_AbstractGeometricalItem* PB_ActionPickItemsInList::addItemToSelection(const QPoint &point)
{
    Eigen::Vector3d origin, direction;
    graphicsView()->convertClickToLine(point, origin, direction);

    double minDist = std::numeric_limits<double>::max();
    CT_AbstractGeometricalItem* pickedItem = nullptr;

    QListIterator<CT_AbstractGeometricalItem*> it(_itemList);
    while (it.hasNext())
    {
        CT_AbstractGeometricalItem *item = it.next();
        const double dist = CT_MathPoint::distancePointLine(item->centerCoordinate(), direction, origin);

        if (dist < minDist && dist < _maxDist)
        {
            minDist = dist;
            pickedItem = item;
        }
    }

    if (pickedItem != nullptr && !_selectedItems->contains(pickedItem))
    {
        _selectedItems->append(pickedItem);
    }
    return pickedItem;
}

CT_AbstractGeometricalItem *PB_ActionPickItemsInList::removeItemFromSelection(const QPoint &point)
{
    Eigen::Vector3d origin, direction;
    graphicsView()->convertClickToLine(point, origin, direction);

    double minDist = std::numeric_limits<double>::max();
    CT_AbstractGeometricalItem* pickedItem = nullptr;

    QListIterator<CT_AbstractGeometricalItem*> it(*_selectedItems);
    while (it.hasNext())
    {
        CT_AbstractGeometricalItem *item = it.next();
        double dist = CT_MathPoint::distancePointLine(item->centerCoordinate(), direction, origin);

        if (dist < minDist && dist < _maxDist)
        {
            minDist = dist;
            pickedItem = item;
        }
    }

    if (pickedItem != nullptr && _selectedItems->contains(pickedItem))
    {
        _selectedItems->removeOne(pickedItem);
    }

    return pickedItem;
}
