#include "ct_standardcircle2ddrawmanager.h"
#include "ct_itemdrawable/ct_circle2d.h"
#include "painterinterface.h"

const QString CT_StandardCircle2DDrawManager::INDEX_CONFIG_DRAW_CIRCLE = CT_StandardCircle2DDrawManager::staticInitConfigDrawCircle();

CT_StandardCircle2DDrawManager::CT_StandardCircle2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Circle2D::staticName() : drawConfigurationName)
{
}

void CT_StandardCircle2DDrawManager::draw(GraphicsViewInterface& view,
                                          PainterInterface& painter,
                                          const CT_AbstractItemDrawable& itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const bool drawCircle = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_CIRCLE).toBool();

    if(drawCircle)
    {
        const CT_Circle2D& item = static_cast<const CT_Circle2D&>(itemDrawable);

        const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
        const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();
        const double zPlane = (item.isZValueDefined() ? item.zValue() : (useAltZVal ? zVal : CT_Circle2D::Z_PLANE_FOR_2D_SHAPES));

        painter.drawCircle(item.centerX(), item.centerY(), zPlane, item.getRadius());
    }
}

CT_ItemDrawableConfiguration CT_StandardCircle2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardCircle2DDrawManager::staticInitConfigDrawCircle() ,QObject::tr("Dessiner le cercle"), CT_ItemDrawableConfiguration::Bool, true);

    return item;
}

// PROTECTED //

QString CT_StandardCircle2DDrawManager::staticInitConfigDrawCircle()
{
    return "C2D_CR";
}
