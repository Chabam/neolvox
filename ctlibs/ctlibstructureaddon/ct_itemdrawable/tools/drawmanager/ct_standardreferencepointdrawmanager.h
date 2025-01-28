#ifndef CT_STANDARDREFERENCEPOINTDRAWMANAGER_H
#define CT_STANDARDREFERENCEPOINTDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardReferencePointDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardReferencePointDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_BUFFER_VISIBLE;
    const static QString INDEX_CONFIG_FACTOR;
    const static QString INDEX_CONFIG_POINT_SIZE;

    static QString staticInitConfigBufferVisible();
    static QString staticInitConfigFactor();
    static QString staticInitConfigPointSize();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDREFERENCEPOINTDRAWMANAGER_H
