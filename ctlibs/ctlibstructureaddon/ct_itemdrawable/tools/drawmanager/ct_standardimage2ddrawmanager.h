#ifndef CT_STANDARDIMAGE2DDRAWMANAGER_H
#define CT_STANDARDIMAGE2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

template< typename DataT>
class CT_StandardImage2DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardImage2DDrawManager(QString drawConfigurationName = "", bool mapMode = true, bool scale = false);

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_3D_MODE_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_LINK_POINTS_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_HEIGHT_MAP_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_SCALING_ENABLED;
    const static QString INDEX_CONFIG_3D_MODE_ZMIN_SCALE_VALUE;
    const static QString INDEX_CONFIG_3D_MODE_ZMAX_SCALE_VALUE;
    const static QString INDEX_CONFIG_MAP_MODE_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_ZLEVEL_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_ZLEVEL_VALUE;
    const static QString INDEX_CONFIG_MAP_MODE_SHOW_GRID;
    const static QString INDEX_CONFIG_MAP_MODE_SHOW_NA;
    const static QString INDEX_CONFIG_MAP_MODE_CLUSTER_MODE;
    const static QString INDEX_CONFIG_MAP_MODE_APPLY_THRESHOLD;
    const static QString INDEX_CONFIG_MAP_MODE_THRESHOLD;
    const static QString INDEX_CONFIG_USE_SELECTED_GRADIENT;


    static QString staticInitConfig3DModeEnabled();
    static QString staticInitConfig3DModeLinkPointsEnabled();
    static QString staticInitConfig3DModeHeightMapEnabled();
    static QString staticInitConfig3DModeScalingEnabled();
    static QString staticInitConfig3DModeZMinScaleValue();
    static QString staticInitConfig3DModeZMaxScaleValue();
    static QString staticInitConfigMapModeEnabled();
    static QString staticInitConfigMapModeZLevelEnabled();
    static QString staticInitConfigMapModeZLevelValue();
    static QString staticInitConfigMapModeShowGrid();
    static QString staticInitConfigMapModeShowNA();
    static QString staticInitConfigMapModeClusterMode();
    static QString staticInitConfigMapModeApplyThreshold();
    static QString staticInitConfigMapModeThreshold();
    static QString staticInitConfigUseSelectedGradient();


    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

private:
    bool    _defaultMapMode;
    bool    _defaultScaleState;
};

#include "ct_itemdrawable/tools/drawmanager/ct_standardimage2ddrawmanager.hpp"

#endif // CT_STANDARDIMAGE2DDRAWMANAGER_H
