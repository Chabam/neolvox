#ifndef CT_STANDARDABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUDDRAWMANAGER_H
#define CT_STANDARDABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUDDRAWMANAGER_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager : public CT_AbstractItemDrawableDrawManager
{
    using SuperClass = CT_AbstractItemDrawableDrawManager;

public:
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager(const QString& drawConfigurationName = "");

    void draw(GraphicsViewInterface& view, PainterInterface& painter, const CT_AbstractItemDrawable& itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_BOUNDING_SHAPE_VISIBLE;
    const static QString INDEX_CONFIG_BOUNDING_SHAPE_POINT_SIZE;
    const static QString INDEX_CONFIG_BOUNDING_SHAPE_CENTER_POINT_VISIBLE;

    static QString staticInitConfigBoundingShapeVisible();
    static QString staticInitConfigBoundingShapePointSize();
    static QString staticInitConfigBoundingShapeCenterPointVisible();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUDDRAWMANAGER_H
