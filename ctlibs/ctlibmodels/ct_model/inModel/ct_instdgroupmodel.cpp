#include "ct_model/inModel/ct_instdgroupmodel.h"

#include "ct_model/outModel/abstract/ct_outabstractgroupmodel.h"

CT_InStdGroupModel::CT_InStdGroupModel(const QString& groupType,
                                       const QString& groupNameFromType,
                                       const QString& displayableName,
                                       const QString& shortDescription,
                                       const QString& detailledDescription,
                                       quint8 minimumNumberOfPossibilityToSelect,
                                       int maximumNumberOfPossibilityThatCanBeSelected) : SuperClass(displayableName)
{
    setItemType(groupType);
    setItemNameFromType(groupNameFromType);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
    setMinimumNumberOfPossibilityToSelect(minimumNumberOfPossibilityToSelect);
    setMaximumNumberOfPossibilityThatCanBeSelected(maximumNumberOfPossibilityThatCanBeSelected);
}

CT_InStdGroupModel::CT_InStdGroupModel(const CT_InStdGroupModel& other, const bool& withPossibilities) : SuperClass(other)
{
    const SuperClass::GroupCollection& grps = other.groups();

    for(const SuperClass::GroupType* group : grps) {
        this->addGroup(static_cast<SuperClass::GroupType*>(group->copy(withPossibilities)));
    }

    const SuperClass::ItemCollection& its = other.items();

    for(const SuperClass::ItemType* item : its) {
        this->addItem(static_cast<SuperClass::ItemType*>(item->copy(withPossibilities)));
    }

    if(withPossibilities)
        staticCopyPossibilitiesToModel(&other, this);
}

CT_InAbstractModel* CT_InStdGroupModel::copy(bool withPossibilities) const
{
    return new CT_InStdGroupModel(*this, withPossibilities);
}
