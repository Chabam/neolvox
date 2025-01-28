#include "ct_standardabstractitemdrawablewithpointclouddrawmanager.h"

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "painterinterface.h"

const QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::INDEX_CONFIG_POINTS_VISIBLE = CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigPointsVisible();
const QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::INDEX_CONFIG_BOUNDING_SHAPE_VISIBLE = CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeVisible();
const QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::INDEX_CONFIG_BOUNDING_SHAPE_CENTER_POINT_VISIBLE = CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeCenterPointVisible();

CT_StandardAbstractItemDrawableWithPointCloudDrawManager::CT_StandardAbstractItemDrawableWithPointCloudDrawManager(QString drawConfigurationName) : CT_AbstractItemDrawableDrawManager(drawConfigurationName.isEmpty() ? CT_AbstractItemDrawableWithPointCloud::staticName() : drawConfigurationName)
{
    
}

void CT_StandardAbstractItemDrawableWithPointCloudDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    Q_UNUSED(view)

    const CT_AbstractItemDrawableWithPointCloud &item = dynamic_cast<const CT_AbstractItemDrawableWithPointCloud&>(itemDrawable);

    if(item.pointCloudIndex() == nullptr)
        return;

    if(drawConfiguration()->variableValue(INDEX_CONFIG_POINTS_VISIBLE).toBool())
        painter.drawPointCloud(item.pointCloudIndex());

    if(drawConfiguration()->variableValue(INDEX_CONFIG_BOUNDING_SHAPE_VISIBLE).toBool() && item.doesBoundingBoxAppearToBeValid())
    {
        painter.drawPoint(item.minX(), item.minY(), item.minZ());
        painter.drawPoint(item.maxX(), item.minY(), item.minZ());
        painter.drawPoint(item.minX(), item.maxY(), item.minZ());
        painter.drawPoint(item.minX(), item.minY(), item.maxZ());
        painter.drawPoint(item.maxX(), item.maxY(), item.minZ());
        painter.drawPoint(item.minX(), item.maxY(), item.maxZ());
        painter.drawPoint(item.maxX(), item.minY(), item.maxZ());
        painter.drawPoint(item.maxX(), item.maxY(), item.maxZ());

        // B
        painter.drawLine(item.minX(), item.minY(), item.minZ(), item.maxX(), item.minY(), item.minZ());
        painter.drawLine(item.maxX(), item.minY(), item.minZ(), item.maxX(), item.maxY(), item.minZ());
        painter.drawLine(item.maxX(), item.maxY(), item.minZ(), item.minX(), item.maxY(), item.minZ());
        painter.drawLine(item.minX(), item.maxY(), item.minZ(), item.minX(), item.minY(), item.minZ());

        // H
        painter.drawLine(item.minX(), item.minY(), item.maxZ(), item.maxX(), item.minY(), item.maxZ());
        painter.drawLine(item.maxX(), item.minY(), item.maxZ(), item.maxX(), item.maxY(), item.maxZ());
        painter.drawLine(item.maxX(), item.maxY(), item.maxZ(), item.minX(), item.maxY(), item.maxZ());
        painter.drawLine(item.minX(), item.maxY(), item.maxZ(), item.minX(), item.minY(), item.maxZ());

        // G
        painter.drawLine(item.minX(), item.minY(), item.minZ(), item.minX(), item.minY(), item.maxZ());
        painter.drawLine(item.minX(), item.minY(), item.maxZ(), item.minX(), item.maxY(), item.maxZ());
        painter.drawLine(item.minX(), item.maxY(), item.maxZ(), item.minX(), item.maxY(), item.minZ());
        painter.drawLine(item.minX(), item.maxY(), item.minZ(), item.minX(), item.minY(), item.minZ());

        // D
        painter.drawLine(item.maxX(), item.minY(), item.minZ(), item.maxX(), item.minY(), item.maxZ());
        painter.drawLine(item.maxX(), item.minY(), item.maxZ(), item.maxX(), item.maxY(), item.maxZ());
        painter.drawLine(item.maxX(), item.maxY(), item.maxZ(), item.maxX(), item.maxY(), item.minZ());
        painter.drawLine(item.maxX(), item.maxY(), item.minZ(), item.maxX(), item.minY(), item.minZ());
    }

    if(drawConfiguration()->variableValue(INDEX_CONFIG_BOUNDING_SHAPE_CENTER_POINT_VISIBLE).toBool() && item.doesBoundingBoxAppearToBeValid())
    {
        painter.drawPoint(item.centerX(),
                          item.centerY(),
                          item.centerZ());
    }
}

CT_ItemDrawableConfiguration CT_StandardAbstractItemDrawableWithPointCloudDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_AbstractItemDrawableDrawManager::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigPointsVisible(),QObject::tr("Points"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeVisible(),QObject::tr("Bounding Shape"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeCenterPointVisible(), QObject::tr("Centre de la Bounding Shape"), CT_ItemDrawableConfiguration::Bool, false);

    return item;
}

// PROTECTED //

QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigPointsVisible()
{
    return "WPC_PV";
}

QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeVisible()
{
    return "WPC_BSV";
}

QString CT_StandardAbstractItemDrawableWithPointCloudDrawManager::staticInitConfigBoundingShapeCenterPointVisible()
{
    return "WPC_BSCPS";
}
