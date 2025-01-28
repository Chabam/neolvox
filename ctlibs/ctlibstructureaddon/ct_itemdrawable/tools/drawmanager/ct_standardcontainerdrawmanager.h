#ifndef CT_STANDARDCONTAINERDRAWMANAGER_H
#define CT_STANDARDCONTAINERDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h"

class CTLIBCORE_EXPORT CT_StandardContainerDrawManager : public CT_AbstractItemDrawableDrawManager
{
public:
    CT_StandardContainerDrawManager(QString drawConfigurationName = "");
    virtual ~CT_StandardContainerDrawManager();

    virtual void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const;
};

#endif // CT_STANDARDCONTAINERDRAWMANAGER_H
