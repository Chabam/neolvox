#include "ct_standardpolygon2ddrawmanager.h"
#include "ct_itemdrawable/ct_polygon2d.h"
#include "painterinterface.h"

const QString CT_StandardPolygon2DDrawManager::INDEX_CONFIG_DRAW_POINTS = CT_StandardPolygon2DDrawManager::staticInitConfigDrawPoints();
const QString CT_StandardPolygon2DDrawManager::INDEX_CONFIG_DRAW_LINES = CT_StandardPolygon2DDrawManager::staticInitConfigDrawLines();
const QString CT_StandardPolygon2DDrawManager::INDEX_CONFIG_DRAW_CENTROID = CT_StandardPolygon2DDrawManager::staticInitConfigCentroid();

CT_StandardPolygon2DDrawManager::CT_StandardPolygon2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Polygon2D::staticName() : drawConfigurationName)
{
    
}

void CT_StandardPolygon2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Polygon2D &item = dynamic_cast<const CT_Polygon2D&>(itemDrawable);

    const bool drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINTS).toBool();
    const bool drawLines = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_LINES).toBool();
    const bool drawCentroid = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_CENTROID).toBool();
    const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
    const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();
    const double zPlane = (item.isZValueDefined() ? item.zValue() : (useAltZVal ? zVal : CT_Polygon2D::Z_PLANE_FOR_2D_SHAPES));

    if(drawPoints || drawLines)
    {
        ((const CT_Polygon2DData&)item.getData()).draw(painter, drawPoints, drawLines, zPlane);
    }

    if (drawCentroid)
    {
        const Eigen::Vector2d &center = item.getCenter();
        painter.drawPoint(center(0), center(1), zPlane);
    }
}

CT_ItemDrawableConfiguration CT_StandardPolygon2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardPolygon2DDrawManager::staticInitConfigDrawPoints() , QObject::tr("Dessiner les sommets"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardPolygon2DDrawManager::staticInitConfigDrawLines() , QObject::tr("Dessiner les côtés"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardPolygon2DDrawManager::staticInitConfigCentroid() , QObject::tr("Dessiner centroid"), CT_ItemDrawableConfiguration::Bool, false);

    return item;
}

// PROTECTED //

QString CT_StandardPolygon2DDrawManager::staticInitConfigDrawPoints()
{
    return "P2D_PT";
}

QString CT_StandardPolygon2DDrawManager::staticInitConfigDrawLines()
{
    return "P2D_LI";
}

QString CT_StandardPolygon2DDrawManager::staticInitConfigCentroid()
{
    return "P2D_CTD";
}

