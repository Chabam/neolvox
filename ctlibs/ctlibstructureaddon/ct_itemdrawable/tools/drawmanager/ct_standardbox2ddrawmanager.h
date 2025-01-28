#ifndef CT_STANDARDBOX2DDRAWMANAGER_H
#define CT_STANDARDBOX2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshape2ddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardBox2DDrawManager : public CT_StandardAbstractShape2DDrawManager
{
    using SuperClass = CT_StandardAbstractShape2DDrawManager;

public:
    CT_StandardBox2DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;

protected:

    static const QString INDEX_CONFIG_DRAW_BOX;
    static const QString INDEX_CONFIG_FILL_BOX;

    static QString staticInitConfigDrawBox();
    static QString staticInitConfigFillBox();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDBOX2DDRAWMANAGER_H
