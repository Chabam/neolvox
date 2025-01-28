#ifndef CT_INABSTRACTITEMATTRIBUTEMODEL_H
#define CT_INABSTRACTITEMATTRIBUTEMODEL_H

#include "ct_model/inModel/abstract/ct_inabstractmodel.h"

class CTLIBMODELS_EXPORT CT_InAbstractItemAttributeModel : public CT_InAbstractModel
{
    Q_OBJECT
    using SuperClass = CT_InAbstractModel;

public:
    /**
     * @brief Construct a model that represent an item attribute
     * @param displayableName : a displayable name
     */
    CT_InAbstractItemAttributeModel(const QString &displayableName);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     */
    CT_InAbstractItemAttributeModel(const CT_InAbstractItemAttributeModel& other);

    /**
     * @brief Set the list of category type
     * @param categories : a list of categories type (per example QList<...>() << CT_AbstractCategory::DATA_X_DIMENSION << CT_AbstractCategory::DATA_X_RESOLUTION). The
     *                     list will be used with the logical operator 'OR'. Item attribute with a category type in the list will be accepted if value type is ok.
     */
    void setCategoriesType(const QStringList& categories);

    /**
     * @brief Set the value type
     * @param type : pass the value type that you want (per example CT_AbstractCategory::NUMBER if you want
     *               to be generic and accept int, double, float, etc... or CT_AbstractCategory::UINT64 for a specific type)
     */
    void setValueType(const int& type);

    /**
     * @brief Returns categories type used by this model
     */
    QList<QString> categoriesType() const;

    /**
     * @brief Returns value type of categories
     */
    int valueType() const;

    /**
     * @brief Returns value type of categories to string
     */
    QString valueTypeToString() const;

    /**
     * @brief Redefined to returns true if the output model can be dynamic_cast to a CT_OutAbstractItemAttributeModel
     */
    bool canBeComparedWith(const CT_OutAbstractModel& model) const override;

    /**
     * @brief Redefined to returns true if the output model can be dynamic_cast to a CT_OutAbstractItemAttributeModel and if
     *        value type of attribute is the same and categories was equivalent
     */
    bool canBeAPossibility(const CT_OutAbstractModel& model) const override;

private:
    QList<QString>          m_categoriesType;
    int                     m_vType;
};

#endif // CT_INABSTRACTITEMATTRIBUTEMODEL_H
