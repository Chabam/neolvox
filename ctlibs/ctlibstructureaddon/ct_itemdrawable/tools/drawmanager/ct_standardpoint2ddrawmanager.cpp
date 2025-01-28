#include "ct_standardpoint2ddrawmanager.h"
#include "ct_itemdrawable/ct_point2d.h"
#include "painterinterface.h"

#include <QObject>


const QString CT_StandardPoint2DDrawManager::INDEX_CONFIG_DRAW_POINT = CT_StandardPoint2DDrawManager::staticInitConfigDrawPoint();
const QString CT_StandardPoint2DDrawManager::INDEX_CONFIG_POINT_SIZE = CT_StandardPoint2DDrawManager::staticInitConfigPointSize();

CT_StandardPoint2DDrawManager::CT_StandardPoint2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Point2D::staticName() : drawConfigurationName)
{
}

void CT_StandardPoint2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Point2D& item = dynamic_cast<const CT_Point2D&>(itemDrawable);

    const bool drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINT).toBool();
    const bool useAltZVal = drawConfiguration()->variableValue(INDEX_CONFIG_USE_ALTERNATIVE_ZVALUE).toBool();
    const double zVal = drawConfiguration()->variableValue(INDEX_CONFIG_Z_VALUE).toDouble();
    const double pointSize = drawConfiguration()->variableValue(INDEX_CONFIG_POINT_SIZE).toDouble();
    const double zPlane = (item.isZValueDefined() ? item.zValue() : (useAltZVal ? zVal : CT_Point2D::Z_PLANE_FOR_2D_SHAPES));

    if (drawPoints)
    {      
        painter.setPointSize(pointSize);
        painter.drawPoint(item.x(), item.y(), zPlane);
        painter.restoreDefaultPointSize();
    }
}

CT_ItemDrawableConfiguration CT_StandardPoint2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardPoint2DDrawManager::staticInitConfigDrawPoint() ,QObject::tr("Dessiner le point"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardPoint2DDrawManager::staticInitConfigPointSize() ,QObject::tr("Taille du point"), CT_ItemDrawableConfiguration::Double, 5);

    return item;
}

// PROTECTED //

QString CT_StandardPoint2DDrawManager::staticInitConfigDrawPoint()
{
    return "PT2D_PT";
}

QString CT_StandardPoint2DDrawManager::staticInitConfigPointSize()
{
    return "PT2D_SZ";
}
