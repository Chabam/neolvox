#ifndef CT_STANDARDCIRCLEDRAWMANAGER_H
#define CT_STANDARDCIRCLEDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractshapedrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardCircleDrawManager : public CT_StandardAbstractShapeDrawManager
{
    using SuperClass = CT_StandardAbstractShapeDrawManager;

public:
    CT_StandardCircleDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;
};

#endif // CT_STANDARDCIRCLEDRAWMANAGER_H
