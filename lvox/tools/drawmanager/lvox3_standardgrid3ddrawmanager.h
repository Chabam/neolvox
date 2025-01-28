#ifndef LVOX3_STANDARDRASTER3DDRAWMANAGER_H
#define LVOX3_STANDARDRASTER3DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"
#include "tools/3dgrid/lvox3_grid3d.h"

template< typename DataT>
class LVOX3_StandardGrid3DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
public:
    LVOX3_StandardGrid3DDrawManager(QString drawConfigurationName = "");
    virtual ~LVOX3_StandardGrid3DDrawManager();

    virtual void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

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

    virtual CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const;

};

template<>
void LVOX3_StandardGrid3DDrawManager<bool>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

template<>
CT_ItemDrawableConfiguration LVOX3_StandardGrid3DDrawManager<bool>::createDrawConfiguration(QString drawConfigurationName) const;


#include "lvox3_standardgrid3ddrawmanager.hpp"

#endif // LVOX3_STANDARDRASTER3DDRAWMANAGER_H
