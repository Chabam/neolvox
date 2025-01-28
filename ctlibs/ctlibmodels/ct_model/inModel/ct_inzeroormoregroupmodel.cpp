#include "ct_inzeroormoregroupmodel.h"

#include "ct_model/outModel/abstract/ct_outabstractgroupmodel.h"

CT_InZeroOrMoreGroupModel::CT_InZeroOrMoreGroupModel(const QString& groupType,
                                                     const QString& groupNameFromType) : SuperClass(groupType,
                                                                                                    groupNameFromType,
                                                                                                    tr("*"))
{
    setShortDescription(tr("None or one group"));
    setMinimumNumberOfPossibilityToSelect(0);
    setMaximumNumberOfPossibilityThatCanBeSelected(0);
}

CT_InZeroOrMoreGroupModel::CT_InZeroOrMoreGroupModel(const CT_InZeroOrMoreGroupModel& other,
                                                     const bool& withPossibilities) : SuperClass(other, withPossibilities)
{
}

CT_InAbstractModel* CT_InZeroOrMoreGroupModel::copy(bool withPossibilities) const
{
    return new CT_InZeroOrMoreGroupModel(*this, withPossibilities);
}

bool CT_InZeroOrMoreGroupModel::mustCompareThisWithChildrenOfOutModel() const
{
    // we accept to compare this model with children of a model
    return true;
}

bool CT_InZeroOrMoreGroupModel::mustCompareOutModelWithChildrenOfThisModel() const
{
    // we accept to compare children of this model with the current output model
    return true;
}

bool CT_InZeroOrMoreGroupModel::mustAllObligatoryChildrensFoundAtLeastOnePossibilityInTheSameParent() const
{
    return false;
}

void CT_InZeroOrMoreGroupModel::groupAdded(const DEF_CT_AbstractGroupModelIn* group)
{
    Q_UNUSED(group)

    setMaximumNumberOfPossibilityThatCanBeSelected(-1);
}

void CT_InZeroOrMoreGroupModel::itemAdded(const CT_InAbstractSingularItemModel* item)
{
    Q_UNUSED(item)

    groupAdded(nullptr);
}

void CT_InZeroOrMoreGroupModel::groupRemoved(const DEF_CT_AbstractGroupModelIn* group)
{
    Q_UNUSED(group)

    if(isEmpty())
        setMaximumNumberOfPossibilityThatCanBeSelected(0);
}

void CT_InZeroOrMoreGroupModel::itemRemoved(const CT_InAbstractSingularItemModel* item)
{
    Q_UNUSED(item)

    groupRemoved(nullptr);
}
