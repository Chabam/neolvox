#include "ct_instditemattributemodel.h"

CT_InStdItemAttributeModel::CT_InStdItemAttributeModel(int valueType,
                                                       const QString& categoryType,
                                                       const QString& displayableName,
                                                       const QString& shortDescription,
                                                       const QString& detailledDescription,
                                                       quint8 minimumNumberOfPossibilityToSelect,
                                                       int maximumNumberOfPossibilityThatCanBeSelected) : CT_InStdItemAttributeModel(valueType,
                                                                                                                                     QList<QString>() << categoryType,
                                                                                                                                     displayableName,
                                                                                                                                     shortDescription,
                                                                                                                                     detailledDescription,
                                                                                                                                     minimumNumberOfPossibilityToSelect,
                                                                                                                                     maximumNumberOfPossibilityThatCanBeSelected)
{
}

CT_InStdItemAttributeModel::CT_InStdItemAttributeModel(int valueType,
                                                       const QList<QString>& categoriesType,
                                                       const QString& displayableName,
                                                       const QString& shortDescription,
                                                       const QString& detailledDescription,
                                                       quint8 minimumNumberOfPossibilityToSelect,
                                                       int maximumNumberOfPossibilityThatCanBeSelected) : SuperClass(displayableName)
{
    setValueType(valueType);
    setCategoriesType(categoriesType);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
    setMinimumNumberOfPossibilityToSelect(minimumNumberOfPossibilityToSelect);
    setMaximumNumberOfPossibilityThatCanBeSelected(maximumNumberOfPossibilityThatCanBeSelected);
}

CT_InStdItemAttributeModel::CT_InStdItemAttributeModel(const CT_InStdItemAttributeModel& other, const bool& withPossibilities) : SuperClass(other)
{
    if(withPossibilities)
        staticCopyPossibilitiesToModel(&other, this);
}

CT_InAbstractModel* CT_InStdItemAttributeModel::copy(bool withPossibilities) const
{
    return new CT_InStdItemAttributeModel(*this, withPossibilities);
}
