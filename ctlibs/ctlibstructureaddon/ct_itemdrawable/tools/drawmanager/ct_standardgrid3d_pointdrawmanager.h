#ifndef CT_STANDARDGRID3D_POINTDRAWMANAGER_H
#define CT_STANDARDGRID3D_POINTDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardGrid3D_PointDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
public:
    CT_StandardGrid3D_PointDrawManager(QString drawConfigurationName = "");

    virtual void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;

protected:

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const;

};

#endif // CT_STANDARDGRID3D_POINTDRAWMANAGER_H
