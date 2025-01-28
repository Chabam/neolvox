#ifndef CT_STANDARDRASTER3DDRAWMANAGER_H
#define CT_STANDARDRASTER3DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"
#include "ct_itemdrawable/ct_grid3d.h"

template< typename DataT>
class CT_StandardGrid3DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardGrid3DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_WIRE_MODE_ENABLED;
    const static QString INDEX_CONFIG_LOW_THRESHOLDS_ENABLED;
    const static QString INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED;
    const static QString INDEX_CONFIG_LOW_THRESHOLDS_VALUE;
    const static QString INDEX_CONFIG_HIGH_THRESHOLDS_VALUE;
    const static QString INDEX_CONFIG_REDUCTION_COEF;
    const static QString INDEX_CONFIG_USE_PREDEFINED_COLORS;
    const static QString INDEX_CONFIG_TRANSPARENCY_VALUE;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_XINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_XSUP;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_YINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_YSUP;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_ZINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_ZSUP;
    const static QString INDEX_CONFIG_SHOW_TRUES_ONLY;

    static QString staticInitConfigWireModeEnabled();
    static QString staticInitConfigLowThresholdsEnabled();
    static QString staticInitConfigHighThresholdsEnabled();
    static QString staticInitConfigLowThresholdValue();
    static QString staticInitConfigHighThresholdValue();
    static QString staticInitConfigReductionCoef();
    static QString staticInitConfigUsePredefinedColors();
    static QString staticInitConfigTransparencyValue();
    static QString staticInitConfigXinf();
    static QString staticInitConfigXsup();
    static QString staticInitConfigYinf();
    static QString staticInitConfigYsup();
    static QString staticInitConfigZinf();
    static QString staticInitConfigZsup();
    static QString staticInitConfigShowTrueOnly();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

template<>
CTLIBSTRUCTUREADDON_EXPORT void CT_StandardGrid3DDrawManager<bool>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT CT_ItemDrawableConfiguration CT_StandardGrid3DDrawManager<bool>::createDrawConfiguration(QString drawConfigurationName) const;

#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid3ddrawmanager.hpp"

#endif // CT_STANDARDRASTER3DDRAWMANAGER_H
