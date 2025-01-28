#ifndef CT_STANDARDPOINT2DDRAWMANAGER_H
#define CT_STANDARDPOINT2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshape2ddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardPoint2DDrawManager : public CT_StandardAbstractShape2DDrawManager
{
    using SuperClass = CT_StandardAbstractShape2DDrawManager;

public:
    CT_StandardPoint2DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_DRAW_POINT;
    const static QString INDEX_CONFIG_POINT_SIZE;

    static QString staticInitConfigDrawPoint();
    static QString staticInitConfigPointSize();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDPOINT2DDRAWMANAGER_H
