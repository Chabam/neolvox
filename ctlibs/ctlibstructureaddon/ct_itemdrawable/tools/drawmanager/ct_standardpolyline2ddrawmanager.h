#ifndef CT_STANDARDPOLYLINE2DDRAWMANAGER_H
#define CT_STANDARDPOLYLINE2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshape2ddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardPolyline2DDrawManager : public CT_StandardAbstractShape2DDrawManager
{
    using SuperClass = CT_StandardAbstractShape2DDrawManager;
public:
    CT_StandardPolyline2DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_DRAW_POINTS;
    const static QString INDEX_CONFIG_DRAW_LINES;

    static QString staticInitConfigDrawPoints();
    static QString staticInitConfigDrawLines();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDPOLYLINE2DDRAWMANAGER_H
