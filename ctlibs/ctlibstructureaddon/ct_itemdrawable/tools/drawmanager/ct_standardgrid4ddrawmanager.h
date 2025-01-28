#ifndef CT_STANDARDGRID4DDRAWMANAGER_H
#define CT_STANDARDGRID4DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"
#include "ct_itemdrawable/ct_grid4d.h"

template< typename DataT>
class CT_StandardGrid4DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
//**********************************************//
//           Constructors/Destructors           //
//**********************************************//
    CT_StandardGrid4DDrawManager(QString drawConfigurationName = "");

//**********************************************//
//             Drawing item drawables           //
//**********************************************//
    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

//**********************************************//
//          Drawing configuration tools         //
//**********************************************//
    static QString staticInitConfigWDisplayValue();
    static QString staticInitConfigWireModeEnabled();
    static QString staticInitConfigLowThresholdsEnabled();
    static QString staticInitConfigHighThresholdsEnabled();
    static QString staticInitConfigLowThresholdValue();
    static QString staticInitConfigHighThresholdValue();
    static QString staticInitConfigReductionCoef();
    static QString staticInitConfigEnableTransparency();
    static QString staticInitConfigTransparencyValue();
    static QString staticInitConfigXinf();
    static QString staticInitConfigXsup();
    static QString staticInitConfigYinf();
    static QString staticInitConfigYsup();
    static QString staticInitConfigZinf();
    static QString staticInitConfigZsup();
    static QString staticInitConfigShowTrueOnly();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

//**********************************************//
//                  Attributes                  //
//**********************************************//
protected :
    // Drawing configuration
    const static QString INDEX_CONFIG_W_DISPLAY_VALUE;
    const static QString INDEX_CONFIG_WIRE_MODE_ENABLED;
    const static QString INDEX_CONFIG_LOW_THRESHOLDS_ENABLED;
    const static QString INDEX_CONFIG_HIGH_THRESHOLDS_ENABLED;
    const static QString INDEX_CONFIG_LOW_THRESHOLDS_VALUE;
    const static QString INDEX_CONFIG_HIGH_THRESHOLDS_VALUE;
    const static QString INDEX_CONFIG_REDUCTION_COEF;
    const static QString INDEX_CONFIG_TRANSPARENCY_ENABLED;
    const static QString INDEX_CONFIG_TRANSPARENCY_VALUE;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_XINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_XSUP;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_YINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_YSUP;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_ZINF;
    const static QString INDEX_CONFIG_HIDE_PLANE_NB_ZSUP;
    const static QString INDEX_CONFIG_SHOW_TRUES_ONLY;
};

template<>
CTLIBSTRUCTUREADDON_EXPORT void CT_StandardGrid4DDrawManager<bool>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT CT_ItemDrawableConfiguration CT_StandardGrid4DDrawManager<bool>::createDrawConfiguration(QString drawConfigurationName) const;

// Including template implementations
#include "ct_standardgrid4ddrawmanager.hpp"

#endif // CT_STANDARDGRID4DDRAWMANAGER_H
