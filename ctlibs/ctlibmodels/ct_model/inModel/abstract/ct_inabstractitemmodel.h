#ifndef CT_INABSTRACTITEMMODEL_H
#define CT_INABSTRACTITEMMODEL_H

#include "ct_model/inModel/abstract/ct_inabstractmodel.h"

/**
 * @brief Represent a model for an input item.
 */
class CTLIBMODELS_EXPORT CT_InAbstractItemModel : public CT_InAbstractModel
{
    Q_OBJECT
    using SuperClass = CT_InAbstractModel;

public:
    /**
     * @brief Construct a item model
     * @param displayableName : a displayable name
     */
    CT_InAbstractItemModel(const QString& displayableName);

    /**
     * @brief Copy constructor (doesn't copy possibilities)
     */
    CT_InAbstractItemModel(const CT_InAbstractItemModel& other) = default;

    /**
     * @brief Set the item's type (the item short type
     *        will be extracted from it automatically in the same time)
     */
    void setItemType(const QString& itemType);

    /**
     * @brief Set the item's name from type
     */
    void setItemNameFromType(const QString& itemNameFromType);

    /**
     * @brief Set the item's name type with a template parameter
     */
    template<class ItemType>
    void setItemNameFromTypeFrom() { setItemNameFromType(ItemType::nameFromType(ItemType::staticType())); }

    /**
     * @brief Set the item's type with a template parameter
     */
    template<class ItemType>
    void setItemTypeFrom() { setItemType(ItemType::staticType()); }

    /**
     * @brief Return the type of the item researched (CT_ItemXXX::staticType)
     */
    QString itemType() const;

    /**
     * @brief Return the name from type of the item researched or if
     *        empty return the result of the method "itemType()"
     */
    QString itemNameFromType() const;

    /**
     * @brief Return the short type of the item (the string after the last '/' character in the type string)
     */
    QString itemShortType() const;

    /**
     * @brief Redefined to save the item type
     */
    void saveSettings(SettingsWriterInterface& writer) const override;

    /**
     * @brief Redefined to restore the item type
     */
    bool restoreSettings(SettingsReaderInterface& reader) override;

    /**
     * @brief Redefined to returns true if the output model prototype was the same type than the "itemType()" or a super class of it
     */
    bool canBeAPossibility(const CT_OutAbstractModel& model) const override;

private:
    QString                     m_itemType;
    QString                     m_itemNameFromType;
    QString                     m_itemShortType;
};

#endif // CT_INABSTRACTITEMMODEL_H
