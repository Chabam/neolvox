#ifndef CT_STANDARDRASTER2DYZDRAWMANAGER_H
#define CT_STANDARDRASTER2DYZDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

template< typename DataT>
class CT_StandardGrid2DYZDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
public:
    CT_StandardGrid2DYZDrawManager(QString drawConfigurationName = "", bool mapMode = true, bool scale = true);
    virtual ~CT_StandardGrid2DYZDrawManager();

    virtual void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

protected:

    const static QString INDEX_CONFIG_3D_MODE_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_SCALING_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_XMIN_SCALE_VALUE;
    const static QString INDEX_CONFIG_3D_MODE_XMAX_SCALE_VALUE;
    const static QString INDEX_CONFIG_MAP_MODE_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_XLEVEL_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_XLEVEL_VALUE;
    const static QString INDEX_CONFIG_MAP_MODE_SHOW_GRID;

    static QString staticInitConfig3DModeEnabled();
    static QString staticInitConfig3DModeLinkPointsEnabled();
    static QString staticInitConfig3DModeScalingEnabled();
    static QString staticInitConfig3DModeXMinScaleValue();
    static QString staticInitConfig3DModeXMaxScaleValue();
    static QString staticInitConfigMapModeEnabled();
    static QString staticInitConfigMapModeXLevelEnabled();
    static QString staticInitConfigMapModeXLevelValue();
    static QString staticInitConfigMapModeShowGrid();

    virtual CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const;

private:
    bool    _defaultMapMode;
    bool    _defaultScaleState;
};

#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid2dyzdrawmanager.hpp"

#endif // CT_STANDARDRASTER2DYZDRAWMANAGER_H
