#include "ct_standardimage2d_pointdrawmanager.h"

#include "ct_itemdrawable/ct_image2d_points.h"
#include "painterinterface.h"

CT_StandardImage2D_PointDrawManager::CT_StandardImage2D_PointDrawManager(QString drawConfigurationName) : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? CT_Image2D_Points::staticName() : drawConfigurationName)
{
}

void CT_StandardImage2D_PointDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Image2D_Points &item = dynamic_cast<const CT_Image2D_Points&>(itemDrawable);

    QColor color = painter.getColor();

    double demiRes = item.resolution() / 2.0;

    double xmin, ymin, xmax, ymax;
    int xx = 0, yy = 0; // Default value to avoid uninitialized warning

    // For each voxel of the grid
    QList<size_t> indices;
    item.getIndicesWithPoints(indices);

    for (int i = 0 ; i < indices.size() ; i++)
    {
        size_t index = indices.at(i);

        item.indexToGrid(index, xx, yy);

        painter.setColor(QColor(0,255,0));

        xmin = item.getCellCenterColCoord(xx) - demiRes;
        ymin = item.getCellCenterLinCoord(yy) - demiRes;
        xmax = item.getCellCenterColCoord(xx) + demiRes;
        ymax = item.getCellCenterLinCoord(yy) + demiRes;

        painter.drawRectXY(Eigen::Vector2d(xmin, ymax), Eigen::Vector2d(xmax, ymin), 0);

    }

    painter.setColor(color);

}

CT_ItemDrawableConfiguration CT_StandardImage2D_PointDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));

    return item;
}

