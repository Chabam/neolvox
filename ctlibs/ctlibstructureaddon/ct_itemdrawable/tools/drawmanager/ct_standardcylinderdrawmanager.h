#ifndef CT_STANDARDCYLINDERDRAWMANAGER_H
#define CT_STANDARDCYLINDERDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshapedrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardCylinderDrawManager : public CT_StandardAbstractShapeDrawManager
{
    using SuperClass = CT_StandardAbstractShapeDrawManager;

public:
    CT_StandardCylinderDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;

protected:
    const static QString INDEX_CONFIG_DRAW_CYLINDER;
    const static QString INDEX_CONFIG_DRAW_AXE;
    const static QString INDEX_CONFIG_DRAW_MEDIAN_CIRCLE;
    const static QString INDEX_CONFIG_TRANSPARENCY_LEVEL;

    static QString staticInitConfigDrawCylinder();
    static QString staticInitConfigDrawAxe();
    static QString staticInitConfigDrawMedianCircle();
    static QString staticInitConfigTransparencyLevel();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDCYLINDERDRAWMANAGER_H
