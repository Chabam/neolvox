#include "ct_helpgraphicsitem.h"

#include <QPainter>
#include <QIcon>

CT_HelpGraphicsItem::CT_HelpGraphicsItem()
{
    mHelpIcon = QIcon(":/Icones/Icones/help.png").pixmap(64);
}

QRectF CT_HelpGraphicsItem::boundingRect() const
{
    return mHelpIcon.rect();
}

void CT_HelpGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    painter->drawPixmap(0, 0, mHelpIcon);
}
