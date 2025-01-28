#include "ct_instdsingularitemmodel.h"

#include "ct_model/outModel/abstract/ct_outabstractsingularitemmodel.h"

CT_InStdSingularItemModel::CT_InStdSingularItemModel(const QString& itemType,
                                                     const QString& itemNameFromType,
                                                     const QString& displayableName,
                                                     const QString& shortDescription,
                                                     const QString& detailledDescription,
                                                     quint8 minimumNumberOfPossibilityToSelect,
                                                     int maximumNumberOfPossibilityThatCanBeSelected) : SuperClass(displayableName)
{
    setItemType(itemType);
    setItemNameFromType(itemNameFromType);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
    setMinimumNumberOfPossibilityToSelect(minimumNumberOfPossibilityToSelect);
    setMaximumNumberOfPossibilityThatCanBeSelected(maximumNumberOfPossibilityThatCanBeSelected);
}

CT_InStdSingularItemModel::CT_InStdSingularItemModel(const CT_InStdSingularItemModel& other,
                                                     const bool& withPossibilities) : SuperClass(other)
{
    const auto visitor = [this, &withPossibilities](const CT_InAbstractItemAttributeModel* att) -> bool {
        this->addItemAttribute(static_cast<CT_InAbstractItemAttributeModel*>(att->copy(withPossibilities)));
        return true;
    };

    other.visitAttributes(visitor);

    if(withPossibilities)
        staticCopyPossibilitiesToModel(&other, this);
}

CT_InAbstractModel* CT_InStdSingularItemModel::copy(bool withPossibilities) const
{
    return new CT_InStdSingularItemModel(*this, withPossibilities);
}

bool CT_InStdSingularItemModel::canBeComparedWith(const CT_OutAbstractModel& model) const
{
    // ok if the out model represent a singular item
    return (dynamic_cast<const CT_OutAbstractSingularItemModel*>(&model) != nullptr);
}
