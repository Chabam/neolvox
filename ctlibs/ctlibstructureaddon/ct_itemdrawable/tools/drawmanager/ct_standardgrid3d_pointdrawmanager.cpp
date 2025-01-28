#include "ct_standardgrid3d_pointdrawmanager.h"

#include "ct_itemdrawable/ct_grid3d_points.h"
#include "painterinterface.h"

CT_StandardGrid3D_PointDrawManager::CT_StandardGrid3D_PointDrawManager(QString drawConfigurationName) : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? CT_Grid3D_Points::staticName() : drawConfigurationName)
{
}

void CT_StandardGrid3D_PointDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Grid3D_Points &item = dynamic_cast<const CT_Grid3D_Points&>(itemDrawable);

    QColor color = painter.getColor();

    double demiRes = item.resolution() / 2.0;

    double xmin, ymin, zmin, xmax, ymax, zmax;
    int xx = 0, yy = 0, zz = 0; // Default value to avoid uninitialized warning

    // For each voxel of the grid
    QList<size_t> indices;
    item.getIndicesWithPoints(indices);

    for (int i = 0 ; i < indices.size() ; i++)
    {
        size_t index = indices.at(i);

        item.indexToGrid(index, xx, yy, zz);

        painter.setColor(QColor(0,255,0));

        xmin = item.getCellCenterX(xx) - demiRes;
        ymin = item.getCellCenterY(yy) - demiRes;
        zmin = item.getCellCenterZ(zz) - demiRes;
        xmax = item.getCellCenterX(xx) + demiRes;
        ymax = item.getCellCenterY(yy) + demiRes;
        zmax = item.getCellCenterZ(zz) + demiRes;

        painter.drawCube( xmin, ymin, zmin, xmax, ymax, zmax, GL_FRONT_AND_BACK, GL_LINE);

    }

    painter.setColor(color);

}

CT_ItemDrawableConfiguration CT_StandardGrid3D_PointDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));

    return item;
}

