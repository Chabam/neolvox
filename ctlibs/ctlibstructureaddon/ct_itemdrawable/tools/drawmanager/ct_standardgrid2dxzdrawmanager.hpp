#ifndef CT_STANDARDGRID2DXZDRAWMANAGER_HPP
#define CT_STANDARDGRID2DXZDRAWMANAGER_HPP

#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid2dxzdrawmanager.h"
#include "ct_itemdrawable/ct_grid2dxz.h"
#include <typeinfo>

#include <QObject>

template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_ENABLED = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeEnabled();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeLinkPointsEnabled();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_SCALING_ENABLED = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeScalingEnabled();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_YMIN_SCALE_VALUE = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeYMinScaleValue();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_YMAX_SCALE_VALUE = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeYMaxScaleValue();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_ENABLED = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeEnabled();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_YLEVEL_ENABLED = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeYLevelEnabled();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_YLEVEL_VALUE = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeYLevelValue();
template< typename DataT > const QString CT_StandardGrid2DXZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_SHOW_GRID = CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeShowGrid();

template< typename DataT >
CT_StandardGrid2DXZDrawManager<DataT>::CT_StandardGrid2DXZDrawManager(QString drawConfigurationName, bool mapMode, bool scale)
    : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? CT_Grid2DXZ<DataT>::staticName() : drawConfigurationName)
{
    _defaultMapMode = mapMode;
    _defaultScaleState = scale;
}

template< typename DataT >
CT_StandardGrid2DXZDrawManager<DataT>::~CT_StandardGrid2DXZDrawManager()
{
}

template< typename DataT >
void CT_StandardGrid2DXZDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Grid2DXZ<DataT> &item = dynamic_cast<const CT_Grid2DXZ<DataT>&>(itemDrawable);

    bool mode3D = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_ENABLED).toBool();
    bool relier = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED).toBool();
    bool echelle = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_SCALING_ENABLED).toBool();
    double ymin = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_YMIN_SCALE_VALUE).toDouble();
    double ymax = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_YMAX_SCALE_VALUE).toDouble();
    bool modeMap = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_ENABLED).toBool();
    bool fixerY = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_YLEVEL_ENABLED).toBool();
    double y = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_YLEVEL_VALUE).toDouble();
    bool show_grid = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_SHOW_GRID).toDouble();

    double amplitude = item.dataMax() - item.dataMin();
    double scaling = (ymax - ymin) / amplitude;

    if (mode3D)
    {
        double lastx = 0;
        double lasty = 0;
        double lastz = 0;
        for (size_t cx = 0 ; cx < item.colDim() ; cx++)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                double value = item.value(cx, lz);

                double x = (cx + 0.5) * item.resolution() + item.minX();
                double z = (lz + 0.5) * item.resolution() + item.minZ();

                if (value != item.NA())
                {
                    if (echelle) {value = (value - item.dataMin()) * scaling + ymin;}

                    // Dessin des lignes selon z
                    if (relier && (lz > 0) && (lastz!=item.NA()))
                    {
                        painter.drawLine(lastx, lasty, lastz, x, value, z);
                    } else if (!relier) {
                        // sinon juste dessin des points
                        painter.drawPoint(x, value, z);
                    }
                }

                lastx = x;
                lasty = value;
                lastz = z;
            }
        }

        // Dessin des ligne selon x
        if (relier)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                for (size_t cx = 0 ; cx < item.colDim() ; cx++)
                {
                    double value = item.value(cx, lz);
                    double x = (cx + 0.5) * item.resolution() + item.minX();
                    double z = (lz + 0.5) * item.resolution() + item.minZ();

                    if (value != item.NA())
                    {
                        if (echelle) {value = (value - (double)item.dataMin()) * scaling + ymin;}

                        if ((cx > 0) && (lastz!=item.NA()))
                        {
                            painter.drawLine(lastx, lasty, lastz, x, value, z);
                        }
                    }
                    lastx = x;
                    lasty = value;
                    lastz = z;
                }
            }
        }

    }

    if (modeMap) {
        double y_val = item.level();
        if (fixerY) {y_val = y;}

        for (size_t cx = 0 ; cx < item.colDim() ; cx++)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                double x = (cx + 0.5) * item.resolution() + item.minX();
                double z = (lz + 0.5) * item.resolution() + item.minZ();
                double value = item.value(cx, lz);

                if (value == item.NA())
                {
                    painter.setColor(QColor(125, 0, 0));
                } else {
                    int colorLevel = ((value - (double)item.dataMin()) / amplitude)*255;
                    painter.setColor(QColor(colorLevel, colorLevel, colorLevel));
                }

                Eigen::Vector2d tLeft(x - 0.5*item.resolution(), z - 0.5*item.resolution());
                Eigen::Vector2d bRight(tLeft(0)+item.resolution(), tLeft(1)+item.resolution());

                painter.fillRectXZ(tLeft, bRight, y_val);

                if (show_grid)
                {
                    painter.setColor(QColor(255, 255, 255));
                    painter.drawRectXZ(tLeft, bRight, y_val);
                }
            }
        }
    }
}

template< typename DataT >
CT_ItemDrawableConfiguration CT_StandardGrid2DXZDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));

    // Adding lines to this config dialog box
    item.addNewConfiguration(staticInitConfigMapModeEnabled(), QObject::tr("Mode Raster"), CT_ItemDrawableConfiguration::Bool, _defaultMapMode);
    item.addNewConfiguration(staticInitConfigMapModeYLevelEnabled(), QObject::tr("Mode Raster : Fixer le niveau Y"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigMapModeYLevelValue(), QObject::tr("Mode Raster : Niveau Y (m)"), CT_ItemDrawableConfiguration::Double, 0);
    item.addNewConfiguration(staticInitConfigMapModeShowGrid(), QObject::tr("Mode Raster : Afficher grille"), CT_ItemDrawableConfiguration::Bool, false);

    item.addNewConfiguration(staticInitConfig3DModeEnabled(), QObject::tr("Mode 3D"), CT_ItemDrawableConfiguration::Bool, !_defaultMapMode);
    item.addNewConfiguration(staticInitConfig3DModeLinkPointsEnabled(), QObject::tr("Mode 3D    : Relier les centres de cases"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfig3DModeScalingEnabled(), QObject::tr("Mode 3D    : Mettre à l'échelle"), CT_ItemDrawableConfiguration::Bool, _defaultScaleState);
    item.addNewConfiguration(staticInitConfig3DModeYMinScaleValue(), QObject::tr("Mode 3D    : Y min de l'échelle (m)"), CT_ItemDrawableConfiguration::Double, 0);
    item.addNewConfiguration(staticInitConfig3DModeYMaxScaleValue(), QObject::tr("Mode 3D    : Y max de l'échelle (m)"), CT_ItemDrawableConfiguration::Double, 5);

    return item;
}

// PROTECTED //

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeEnabled()
{
    return "GR2DXZ_3DME";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeLinkPointsEnabled()
{
    return "GR2DXZ_3DMLPE";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeScalingEnabled()
{
    return "GR2DXZ_3DMSE";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeYMinScaleValue()
{
    return "GR2DXZ_3DMZMINSV";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfig3DModeYMaxScaleValue()
{
    return "GR2DXZ_3DMZMAXSV";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeEnabled()
{
    return "GR2DXZ_MME";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeYLevelEnabled()
{
    return "GR2DXZ_MMZLE";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeYLevelValue()
{
    return "GR2DXZ_MMZLV";
}

template< typename DataT >
QString CT_StandardGrid2DXZDrawManager<DataT>::staticInitConfigMapModeShowGrid()
{
    return "GR2DXZ_SHGRD";
}

#endif // CT_STANDARDGRID2DXZDRAWMANAGER_HPP
