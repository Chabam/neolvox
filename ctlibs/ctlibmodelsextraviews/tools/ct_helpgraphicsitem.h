#ifndef CT_HELPGRAPHICSITEM_H
#define CT_HELPGRAPHICSITEM_H

#include <QGraphicsObject>

class CT_HelpGraphicsItem : public QGraphicsObject
{
    Q_OBJECT

public:
    CT_HelpGraphicsItem();

    QRectF boundingRect() const final;

    void paint(QPainter*                       painter,
               QStyleOptionGraphicsItem const* option,
               QWidget*                        widget = 0) final;

private:
    QPixmap mHelpIcon;

};

#endif // CT_HELPGRAPHICSITEM_H
