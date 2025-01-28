#ifndef CT_STANDARDCOLORCOMPOSITEDRAWMANAGER_H
#define CT_STANDARDCOLORCOMPOSITEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardColorCompositeDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardColorCompositeDrawManager(QString drawConfigurationName = "", bool mapMode = true, bool scale = false);

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_MIN_RED;
    const static QString INDEX_CONFIG_MAX_RED;
    const static QString INDEX_CONFIG_3D_MODE_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_ZLEVEL_ENABLED;
    const static QString INDEX_CONFIG_MAP_MODE_ZLEVEL_VALUE;

    static QString staticInitConfigMinRed();
    static QString staticInitConfigMaxRed();
    static QString staticInitConfig3DModeEnabled();
    static QString staticInitConfigMapModeEnabled();
    static QString staticInitConfigMapModeZLevelEnabled();
    static QString staticInitConfigMapModeZLevelValue();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

private:
    bool    _defaultMapMode;
    bool    _defaultScaleState;
};

#endif // CT_STANDARDCOLORCOMPOSITEDRAWMANAGER_H
