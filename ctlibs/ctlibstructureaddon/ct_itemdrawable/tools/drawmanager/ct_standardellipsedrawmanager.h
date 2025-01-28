#ifndef CT_STANDARDELLIPSEDRAWMANAGER_H
#define CT_STANDARDELLIPSEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshapedrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardEllipseDrawManager : public CT_StandardAbstractShapeDrawManager
{
    using SuperClass = CT_StandardAbstractShapeDrawManager;

public:
    CT_StandardEllipseDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;
};

#endif // CT_STANDARDELLIPSEDRAWMANAGER_H
