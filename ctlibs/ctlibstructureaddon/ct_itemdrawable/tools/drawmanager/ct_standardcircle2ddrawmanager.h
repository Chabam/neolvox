#ifndef CT_STANDARDCIRCLE2DDRAWMANAGER_H
#define CT_STANDARDCIRCLE2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshape2ddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardCircle2DDrawManager : public CT_StandardAbstractShape2DDrawManager
{
    using SuperClass = CT_StandardAbstractShape2DDrawManager;

public:
    CT_StandardCircle2DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;

protected:
    static const QString INDEX_CONFIG_DRAW_CIRCLE;

    static QString staticInitConfigDrawCircle();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDCIRCLE2DDRAWMANAGER_H
