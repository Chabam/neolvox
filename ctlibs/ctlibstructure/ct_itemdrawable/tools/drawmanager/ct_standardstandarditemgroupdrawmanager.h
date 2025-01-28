#ifndef CT_STANDARDSTANDARDITEMGROUPDRAWMANAGER_H
#define CT_STANDARDSTANDARDITEMGROUPDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h"

class CTLIBSTRUCTURE_EXPORT CT_StandardStandardItemGroupDrawManager : public CT_AbstractItemDrawableDrawManager
{
public:
    CT_StandardStandardItemGroupDrawManager(QString drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

protected:
    const static QString INDEX_CONFIG_ITEMS_BACKUP_VISIBLE;
    const static QString INDEX_CONFIG_ITEMS_NEW_VISIBLE;

    virtual CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;
};

#endif // CT_STANDARDSTANDARDITEMGROUPDRAWMANAGER_H
