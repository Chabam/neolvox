#ifndef CT_STANDARDGRID2DYZDRAWMANAGER_HPP
#define CT_STANDARDGRID2DYZDRAWMANAGER_HPP

#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid2dyzdrawmanager.h"
#include "ct_itemdrawable/ct_grid2dyz.h"
#include <typeinfo>

#include <QObject>

template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_ENABLED = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeEnabled();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeLinkPointsEnabled();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_SCALING_ENABLED = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeScalingEnabled();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_XMIN_SCALE_VALUE = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeXMinScaleValue();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_3D_MODE_XMAX_SCALE_VALUE = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeXMaxScaleValue();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_ENABLED = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeEnabled();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_XLEVEL_ENABLED = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeXLevelEnabled();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_XLEVEL_VALUE = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeXLevelValue();
template< typename DataT > const QString CT_StandardGrid2DYZDrawManager<DataT>::INDEX_CONFIG_MAP_MODE_SHOW_GRID = CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeShowGrid();

template< typename DataT >
CT_StandardGrid2DYZDrawManager<DataT>::CT_StandardGrid2DYZDrawManager(QString drawConfigurationName, bool mapMode, bool scale)
    : CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(drawConfigurationName.isEmpty() ? CT_Grid2DYZ<DataT>::staticName() : drawConfigurationName)
{
    _defaultMapMode = mapMode;
    _defaultScaleState = scale;
}

template< typename DataT >
CT_StandardGrid2DYZDrawManager<DataT>::~CT_StandardGrid2DYZDrawManager()
{
}

template< typename DataT >
void CT_StandardGrid2DYZDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Grid2DYZ<DataT> &item = dynamic_cast<const CT_Grid2DYZ<DataT>&>(itemDrawable);

    bool mode3D = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_ENABLED).toBool();
    bool relier = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED).toBool();
    bool echelle = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_SCALING_ENABLED).toBool();
    double xmin = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_XMIN_SCALE_VALUE).toDouble();
    double xmax = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_3D_MODE_XMAX_SCALE_VALUE).toDouble();
    bool modeMap = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_ENABLED).toBool();
    bool fixerX = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_XLEVEL_ENABLED).toBool();
    double x = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_XLEVEL_VALUE).toDouble();
    bool show_grid = getDrawConfiguration()->getVariableValue(INDEX_CONFIG_MAP_MODE_SHOW_GRID).toDouble();

    double amplitude = item.dataMax() - item.dataMin();
    double scaling = (xmax - xmin) / amplitude;

    if (mode3D)
    {
        double lastx = 0;
        double lasty = 0;
        double lastz = 0;
        for (size_t cy = 0 ; cy < item.colDim() ; cy++)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                double value = item.value(cy, lz);

                double y = (cy + 0.5) * item.resolution() + item.minY();
                double z = (lz + 0.5) * item.resolution() + item.minZ();

                if (value != item.NA())
                {
                    if (echelle) {value = (value - item.dataMin()) * scaling + xmin;}

                    // Dessin des lignes selon z
                    if (relier && (lz > 0) && (lastz!=item.NA()))
                    {
                        painter.drawLine(lastx, lasty, lastz, value, y, z);
                    } else if (!relier) {
                        // sinon juste dessin des points
                        painter.drawPoint(value, y, z);
                    }
                }

                lastx = value;
                lasty = y;
                lastz = z;
            }
        }

        // Dessin des ligne selon x
        if (relier)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                for (size_t cy = 0 ; cy < item.colDim() ; cy++)
                {
                    double value = item.value(cy, lz);
                    double y = (cy + 0.5) * item.resolution() + item.minY();
                    double z = (lz + 0.5) * item.resolution() + item.minZ();

                    if (value != item.NA())
                    {
                        if (echelle) {value = (value - (double)item.dataMin()) * scaling + xmin;}

                        if ((cy > 0) && (lastz!=item.NA()))
                        {
                            painter.drawLine(lastx, lasty, lastz, value, y, z);
                        }
                    }
                    lastx = value;
                    lasty = y;
                    lastz = z;
                }
            }
        }

    }

    if (modeMap) {
        double x_val = item.level();
        if (fixerX) {x_val = x;}

        for (size_t cy = 0 ; cy < item.colDim() ; cy++)
        {
            for (size_t lz = 0 ; lz < item.linDim() ; lz++)
            {
                double y = (cy + 0.5) * item.resolution() + item.minY();
                double z = (lz + 0.5) * item.resolution() + item.minZ();
                double value = item.value(cy, lz);

                if (value == item.NA())
                {
                    painter.setColor(QColor(125, 0, 0));
                } else {
                    int colorLevel = ((value - (double)item.dataMin()) / amplitude)*255;
                    painter.setColor(QColor(colorLevel, colorLevel, colorLevel));
                }

                Eigen::Vector2d tLeft(y - 0.5*item.resolution(), z - 0.5*item.resolution());
                Eigen::Vector2d bRight(tLeft(0)+item.resolution(), tLeft(1)+item.resolution());

                painter.fillRectYZ(tLeft, bRight, x_val);

                if (show_grid)
                {
                    painter.setColor(QColor(255, 255, 255));
                    painter.drawRectYZ(tLeft, bRight, x_val);
                }
            }
        }
    }
}

template< typename DataT >
CT_ItemDrawableConfiguration CT_StandardGrid2DYZDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));

    // Adding lines to this config dialog box
    item.addNewConfiguration(staticInitConfigMapModeEnabled(), QObject::tr("Mode Raster"), CT_ItemDrawableConfiguration::Bool, _defaultMapMode);
    item.addNewConfiguration(staticInitConfigMapModeXLevelEnabled(), QObject::tr("Mode Raster : Fixer le niveau X"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigMapModeXLevelValue(), QObject::tr("Mode Raster : Niveau X (m)"), CT_ItemDrawableConfiguration::Double, 0);
    item.addNewConfiguration(staticInitConfigMapModeShowGrid(), QObject::tr("Mode Raster : Afficher grille"), CT_ItemDrawableConfiguration::Bool, false);

    item.addNewConfiguration(staticInitConfig3DModeEnabled(), QObject::tr("Mode 3D"), CT_ItemDrawableConfiguration::Bool, !_defaultMapMode);
    item.addNewConfiguration(staticInitConfig3DModeLinkPointsEnabled(), QObject::tr("Mode 3D    : Relier les centres de cases"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfig3DModeScalingEnabled(), QObject::tr("Mode 3D    : Mettre à l'échelle"), CT_ItemDrawableConfiguration::Bool, _defaultScaleState);
    item.addNewConfiguration(staticInitConfig3DModeXMinScaleValue(), QObject::tr("Mode 3D    : X min de l'échelle (m)"), CT_ItemDrawableConfiguration::Double, 0);
    item.addNewConfiguration(staticInitConfig3DModeXMaxScaleValue(), QObject::tr("Mode 3D    : X max de l'échelle (m)"), CT_ItemDrawableConfiguration::Double, 5);

    return item;
}

// PROTECTED //

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeEnabled()
{
    return "GR2DYZ_3DME";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeLinkPointsEnabled()
{
    return "GR2DYZ_3DMLPE";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeScalingEnabled()
{
    return "GR2DYZ_3DMSE";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeXMinScaleValue()
{
    return "GR2DYZ_3DMZMINSV";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfig3DModeXMaxScaleValue()
{
    return "GR2DYZ_3DMZMAXSV";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeEnabled()
{
    return "GR2DYZ_MME";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeXLevelEnabled()
{
    return "GR2DYZ_MMZLE";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeXLevelValue()
{
    return "GR2DYZ_MMZLV";
}

template< typename DataT >
QString CT_StandardGrid2DYZDrawManager<DataT>::staticInitConfigMapModeShowGrid()
{
    return "GR2DYZ_SHGRD";
}

#endif // CT_STANDARDGRID2DYZDRAWMANAGER_HPP
