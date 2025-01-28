#include "ct_inabstractitemmodel.h"
#include <QDebug>

#include "ct_model/outModel/abstract/ct_outabstractitemmodel.h"

CT_InAbstractItemModel::CT_InAbstractItemModel(const QString& displayableName) : SuperClass(displayableName)
{
}

void CT_InAbstractItemModel::setItemType(const QString& itemType)
{
    MODELS_ASSERT_X(!itemType.isEmpty(), "setItemType", "You pass an empty item type !");
    if (itemType.isEmpty()) {qDebug() << "CT_InAbstractItemModel::setItemType" << ", " <<  "You pass an empty item type !"; return;}

    MODELS_ASSERT(itemType.at(itemType.size()-1) != '/');
    if (itemType.at(itemType.size()-1) == '/') {qDebug() << "CT_InAbstractItemModel::setItemType" << ", " <<  "itemType.at(itemType.size()-1) == '/'"; return;}

    m_itemType = itemType;
    m_itemShortType = itemType.split("/").last();
}

void CT_InAbstractItemModel::setItemNameFromType(const QString& itemNameFromType)
{
    m_itemNameFromType = itemNameFromType;
}

QString CT_InAbstractItemModel::itemType() const
{
    return m_itemType;
}

QString CT_InAbstractItemModel::itemNameFromType() const
{
    if(m_itemNameFromType.isEmpty())
        return m_itemType;

    return m_itemNameFromType;
}

QString CT_InAbstractItemModel::itemShortType() const
{
    return m_itemShortType;
}

void CT_InAbstractItemModel::saveSettings(SettingsWriterInterface& writer) const
{
    SuperClass::saveSettings(writer);

    writer.addParameter(this, "ItemType", itemType());
}

bool CT_InAbstractItemModel::restoreSettings(SettingsReaderInterface& reader)
{
    if(!SuperClass::restoreSettings(reader))
        return false;

    QVariant value;
    if(!reader.parameter(this, "ItemType", value) || (value.toString() != itemType()))
        return false;

    return true;
}

bool CT_InAbstractItemModel::canBeAPossibility(const CT_OutAbstractModel& model) const
{
    const CT_OutAbstractItemModel* outModel = dynamic_cast<const CT_OutAbstractItemModel*>(&model);

    if(outModel == nullptr)
        return false;

    const QString type = outModel->itemDrawable()->itemToolForModel()->itemInheritPath() + "/";
    const QString myType = itemType() + "/";

    // we accept the out model to be a possibility if the item of the out model start with the type of the item of this model
    return type.startsWith(myType);
}

