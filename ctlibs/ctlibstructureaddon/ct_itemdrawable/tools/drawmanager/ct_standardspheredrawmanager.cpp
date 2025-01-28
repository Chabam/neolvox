#include "ct_standardspheredrawmanager.h"
#include "ct_itemdrawable/ct_sphere.h"
#include "painterinterface.h"

#include <QtMath>

CT_StandardSphereDrawManager::CT_StandardSphereDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Sphere::staticName() : drawConfigurationName)
{
}

void CT_StandardSphereDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Sphere &item = dynamic_cast<const CT_Sphere&>(itemDrawable);

    painter.drawPartOfSphere(item.centerX(), item.centerY(), item.centerZ(), item.getRadius(), -M_PI, M_PI, -M_PI, M_PI, true);
}
