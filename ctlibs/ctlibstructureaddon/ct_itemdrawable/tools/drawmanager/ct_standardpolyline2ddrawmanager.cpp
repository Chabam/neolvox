#include "ct_standardpolyline2ddrawmanager.h"
#include "ct_itemdrawable/ct_polyline2d.h"
#include "painterinterface.h"

const QString CT_StandardPolyline2DDrawManager::INDEX_CONFIG_DRAW_POINTS = CT_StandardPolyline2DDrawManager::staticInitConfigDrawPoints();
const QString CT_StandardPolyline2DDrawManager::INDEX_CONFIG_DRAW_LINES = CT_StandardPolyline2DDrawManager::staticInitConfigDrawLines();

CT_StandardPolyline2DDrawManager::CT_StandardPolyline2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Polyline2D::staticName() : drawConfigurationName)
{
}

void CT_StandardPolyline2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Polyline2D &item = dynamic_cast<const CT_Polyline2D&>(itemDrawable);

    const bool drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINTS).toBool();
    const bool drawLines = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_LINES).toBool();
    const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
    const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();
    const double zPlane = (item.isZValueDefined() ? item.zValue() : (useAltZVal ? zVal : CT_Polyline2D::Z_PLANE_FOR_2D_SHAPES));

    if(drawPoints || drawLines)
    {
        const QVector<Eigen::Vector2d>& vertices = item.getVertices();
        int size = vertices.size();

        for (int i = 0 ; i < size ; i++)
        {
            const Eigen::Vector2d& pt1 = vertices.at(i);

            if(drawPoints)
            {
                painter.drawPoint(pt1(0), pt1(1), zPlane);
            }
            if(drawLines && (i < size - 1))
            {
                const Eigen::Vector2d& pt2 = vertices.at(i+1);
                painter.drawLine(pt1(0), pt1(1), zPlane, pt2(0), pt2(1), zPlane);
            }
        }
    }
}

CT_ItemDrawableConfiguration CT_StandardPolyline2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardPolyline2DDrawManager::staticInitConfigDrawPoints() ,QObject::tr("Dessiner les sommets"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardPolyline2DDrawManager::staticInitConfigDrawLines() ,QObject::tr("Dessiner les côtés"), CT_ItemDrawableConfiguration::Bool, true);

    return item;
}

// PROTECTED //

QString CT_StandardPolyline2DDrawManager::staticInitConfigDrawPoints()
{
    return "PL2D_PT";
}

QString CT_StandardPolyline2DDrawManager::staticInitConfigDrawLines()
{
    return "PL2D_LI";
}
