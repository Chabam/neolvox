#include "ct_standardstandarditemgroupdrawmanager.h"

#include "ct_itemdrawable/ct_standarditemgroup.h"

#include <QObject>

const QString CT_StandardStandardItemGroupDrawManager::INDEX_CONFIG_ITEMS_BACKUP_VISIBLE = "IBV";
const QString CT_StandardStandardItemGroupDrawManager::INDEX_CONFIG_ITEMS_NEW_VISIBLE = "INV";

CT_StandardStandardItemGroupDrawManager::CT_StandardStandardItemGroupDrawManager(QString drawConfigurationName) : CT_AbstractItemDrawableDrawManager(drawConfigurationName.isEmpty() ? CT_StandardItemGroup::staticName() : drawConfigurationName)
{
}

void CT_StandardStandardItemGroupDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    const CT_StandardItemGroup& item = static_cast<const CT_StandardItemGroup&>(itemDrawable);

    //painter.enableSetColor(false);

    const auto visitorG = [&view, &painter](const CT_StandardItemGroup* group) -> bool {
        const_cast<CT_StandardItemGroup*>(group)->draw(view, painter);
        return true;
    };

    item.visitGroups(visitorG);

    const auto visitorI = [&view, &painter](const CT_AbstractSingularItemDrawable* item) -> bool {
        const_cast<CT_AbstractSingularItemDrawable*>(item)->draw(view, painter);
        return true;
    };

    if(drawConfiguration()->variableValue(INDEX_CONFIG_ITEMS_NEW_VISIBLE).toBool())
        item.visitNewSingularItems(visitorI);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_ITEMS_BACKUP_VISIBLE).toBool())
        item.visitBackupSingularItems(visitorI);

    //painter.enableSetColor(true);
}

CT_ItemDrawableConfiguration CT_StandardStandardItemGroupDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_AbstractItemDrawableDrawManager::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(INDEX_CONFIG_ITEMS_BACKUP_VISIBLE, QObject::tr("Anciens Items"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(INDEX_CONFIG_ITEMS_NEW_VISIBLE, QObject::tr("Nouveaux Items"), CT_ItemDrawableConfiguration::Bool, true);

    return item;
}
