#include "ct_standardcircledrawmanager.h"

#include "ct_itemdrawable/ct_circle.h"
#include "painterinterface.h"

CT_StandardCircleDrawManager::CT_StandardCircleDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Circle::staticName() : drawConfigurationName)
{
}

void CT_StandardCircleDrawManager::draw(GraphicsViewInterface& view,
                                        PainterInterface& painter,
                                        const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Circle& item = static_cast<const CT_Circle&>(itemDrawable);

    painter.drawCircle3D(item.getCenter(), item.getDirection(), item.getRadius());
}
