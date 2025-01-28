#include "ct_standardbox2ddrawmanager.h"
#include "ct_itemdrawable/ct_box2d.h"
#include "painterinterface.h"

const QString CT_StandardBox2DDrawManager::INDEX_CONFIG_DRAW_BOX = CT_StandardBox2DDrawManager::staticInitConfigDrawBox();
const QString CT_StandardBox2DDrawManager::INDEX_CONFIG_FILL_BOX = CT_StandardBox2DDrawManager::staticInitConfigFillBox();

CT_StandardBox2DDrawManager::CT_StandardBox2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Box2D::staticName() : drawConfigurationName)
{
}

void CT_StandardBox2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const bool drawBox = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_BOX).toBool();

    if(drawBox)
    {
        const CT_Box2D& item = static_cast<const CT_Box2D&>(itemDrawable);

        const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
        const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();
        const bool filled = drawConfiguration()->variableValue(INDEX_CONFIG_FILL_BOX).toBool();

        const double zPlane = (item.isZValueDefined() ? item.zValue() : (useAltZVal ? zVal : CT_Box2D::Z_PLANE_FOR_2D_SHAPES));

        Eigen::Vector3d min, max;
        item.boundingBox(min, max);

        if (filled)
            painter.fillRectXY(min.head(2), max.head(2), zPlane);
        else
            painter.drawRectXY(min.head(2), max.head(2), zPlane);
    }
}

CT_ItemDrawableConfiguration CT_StandardBox2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardBox2DDrawManager::staticInitConfigDrawBox() ,QObject::tr("Dessiner le rectangle"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardBox2DDrawManager::staticInitConfigFillBox() ,QObject::tr("Remplir"), CT_ItemDrawableConfiguration::Bool, false);

    return item;
}

// PROTECTED //

QString CT_StandardBox2DDrawManager::staticInitConfigDrawBox()
{
    return "B2D_BX";
}

QString CT_StandardBox2DDrawManager::staticInitConfigFillBox()
{
    return "B2D_FL";
}
