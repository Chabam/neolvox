#ifndef CT_STANDARDPOLYGON2DDRAWMANAGER_H
#define CT_STANDARDPOLYGON2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshape2ddrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardPolygon2DDrawManager : public CT_StandardAbstractShape2DDrawManager
{
    using SuperClass = CT_StandardAbstractShape2DDrawManager;

public:
    CT_StandardPolygon2DDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:
    const static QString INDEX_CONFIG_DRAW_POINTS;
    const static QString INDEX_CONFIG_DRAW_LINES;
    const static QString INDEX_CONFIG_DRAW_CENTROID;

    static QString staticInitConfigDrawPoints();
    static QString staticInitConfigDrawLines();
    static QString staticInitConfigCentroid();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDPOLYGON2DDRAWMANAGER_H
