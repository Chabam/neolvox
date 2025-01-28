#ifndef PB_ACTIONPICKITEMSINLIST_H
#define PB_ACTIONPICKITEMSINLIST_H

#include "ctlibstdactions/ctlibstdactions_global.h"

#include "views/actions/pb_actionpickitemsinlistoptions.h"
#include "ct_actions/abstract/ct_abstractactionforgraphicsview.h"

#include "ct_itemdrawable/abstract/ct_abstractgeometricalitem.h"

#include <QRect>

class CTLIBSTDACTIONS_EXPORT PB_ActionPickItemsInList : public CT_AbstractActionForGraphicsView
{
    Q_OBJECT
    using SuperClass = CT_AbstractActionForGraphicsView;

public:
    PB_ActionPickItemsInList(const QList<CT_AbstractGeometricalItem*> &itemList, QList<CT_AbstractGeometricalItem*>* selectedItems, double maxDist = 1);
    ~PB_ActionPickItemsInList() override;

    QString title() const override;
    QString description() const override;
    QIcon icon() const override;
    QString type() const override;

    void init() override;

    bool mousePressEvent(QMouseEvent *e) override;
    bool mouseMoveEvent(QMouseEvent *e) override;
    bool mouseReleaseEvent(QMouseEvent *e) override;
    bool wheelEvent(QWheelEvent *e) override;

    bool keyPressEvent(QKeyEvent *e) override;
    bool keyReleaseEvent(QKeyEvent *e) override;

    void draw(GraphicsViewInterface &view, PainterInterface &painter) override;
    void drawOverlay(GraphicsViewInterface &view, QPainter &painter) override;

    CT_AbstractAction* createInstance() const override;

    CT_AbstractGeometricalItem* addItemToSelection(const QPoint &point);
    CT_AbstractGeometricalItem* removeItemFromSelection(const QPoint &point);

public slots:
    void redraw();
    void selectAll();
    void selectNone();

private:
    double                                  _maxDist;
    QPoint                                  _oldPos;
    Qt::MouseButtons                        _buttonsPressed;

    QList<CT_AbstractGeometricalItem*>      _itemList;
    QList<CT_AbstractGeometricalItem*>*     _selectedItems;
};


#endif // PB_ACTIONPICKITEMSINLIST_H
