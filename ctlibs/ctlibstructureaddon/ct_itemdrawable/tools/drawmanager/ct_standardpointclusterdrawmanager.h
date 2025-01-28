#ifndef CT_STANDARDPOINTCLUSTERDRAWMANAGER_H
#define CT_STANDARDPOINTCLUSTERDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardPointClusterDrawManager : public CT_StandardAbstractItemDrawableWithPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithPointCloudDrawManager;

public:
    CT_StandardPointClusterDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view, PainterInterface& painter, const CT_AbstractItemDrawable& itemDrawable) const override;

protected:
    static const QString INDEX_CONFIG_BARYCENTER_VISIBLE;
    static const QString INDEX_CONFIG_LINES_VISIBLE;

    static QString staticInitConfigBarycenterVisible();
    static QString staticInitConfigLinesVisible();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDPOINTCLUSTERDRAWMANAGER_H
