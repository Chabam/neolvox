#include "ct_standardellipsedrawmanager.h"

#include "ct_itemdrawable/ct_ellipse.h"
#include "painterinterface.h"

CT_StandardEllipseDrawManager::CT_StandardEllipseDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Ellipse::staticName() : drawConfigurationName)
{
}

void CT_StandardEllipseDrawManager::draw(GraphicsViewInterface& view,
                                         PainterInterface& painter,
                                         const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Ellipse& item = static_cast<const CT_Ellipse&>(itemDrawable);

    painter.drawEllipse(item.centerX(), item.centerY(), item.centerZ(), item.getAxisA().length()/2.0, item.getAxisB().length()/2.0);
}
