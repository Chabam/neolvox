#include "ct_standardline2ddrawmanager.h"
#include "ct_itemdrawable/ct_line2d.h"

#include "painterinterface.h"

const QString CT_StandardLine2DDrawManager::INDEX_CONFIG_DRAW_POINTS = CT_StandardLine2DDrawManager::staticInitConfigDrawPoints();
const QString CT_StandardLine2DDrawManager::INDEX_CONFIG_DRAW_LINE = CT_StandardLine2DDrawManager::staticInitConfigDrawLine();

CT_StandardLine2DDrawManager::CT_StandardLine2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Line2D::staticName() : drawConfigurationName)
{
}

void CT_StandardLine2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Line2D &item = dynamic_cast<const CT_Line2D&>(itemDrawable);

    const bool drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINTS).toBool();
    const bool drawLine = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_LINE).toBool();
    const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
    const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();

    const double zPlane = useAltZVal ? zVal : CT_Line2D::Z_PLANE_FOR_2D_SHAPES;

    if(drawPoints)
    {
        painter.drawPoint(item.x1(), item.y1(), zPlane);
        painter.drawPoint(item.x2(), item.y2(), zPlane);
    }

    if(drawLine)
    {
        painter.drawLine(item.x1(), item.y1(), zPlane, item.x2(), item.y2(), zPlane);
    }
}

CT_ItemDrawableConfiguration CT_StandardLine2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardLine2DDrawManager::staticInitConfigDrawPoints() ,QObject::tr("Dessiner les points"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardLine2DDrawManager::staticInitConfigDrawLine() ,QObject::tr("Dessiner la ligne"), CT_ItemDrawableConfiguration::Bool, true);

    return item;
}

// PROTECTED //

QString CT_StandardLine2DDrawManager::staticInitConfigDrawPoints()
{
    return "L2D_PT";
}

QString CT_StandardLine2DDrawManager::staticInitConfigDrawLine()
{
    return "L2D_LI";
}
