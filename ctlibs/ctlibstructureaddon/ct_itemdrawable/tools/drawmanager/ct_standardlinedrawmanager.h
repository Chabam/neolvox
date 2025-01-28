#ifndef CT_STANDARDLINEDRAWMANAGER_H
#define CT_STANDARDLINEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshapedrawmanager.h"

#include <QPen>

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardLineDrawManager : public CT_StandardAbstractShapeDrawManager
{
    using SuperClass = CT_StandardAbstractShapeDrawManager;

public:
    CT_StandardLineDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

    void configure(const QPen &pen);

protected:

    const static QString INDEX_CONFIG_LINE_SIZE;
    const static QString INDEX_CONFIG_LINE_STYLE;

    static QString staticInitConfigLineSize();
    static QString staticInitConfigLineStyle();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

private:
    QPen            _pen;
    bool            _forcedStyle;

};

#endif // CT_STANDARDLINEDRAWMANAGER_H
