#include "ct_outstdgroupmodel.h"

template<class StdGroup>
CT_OutStdGroupModel<StdGroup>::CT_OutStdGroupModel(const QString& displayableName,
                                                   const QString& shortDescription,
                                                   const QString& detailledDescription,
                                                   StdGroup* prototype) : SuperClass(displayableName)
{
    setPrototype((prototype == nullptr) ? new StdGroup() : prototype);
    setShortDescription(shortDescription);
    setDetailledDescription(detailledDescription);
}

template<class StdGroup>
CT_OutStdGroupModel<StdGroup>::CT_OutStdGroupModel(const CT_OutStdGroupModel& other) : SuperClass(other)
{
    const SuperClass::GroupCollection& grps = other.groups();

    for(const SuperClass::GroupType* group : grps) {
        this->addGroup(static_cast<SuperClass::GroupType*>(group->copy()));
    }

    const SuperClass::ItemCollection& its = other.items();

    for(const SuperClass::ItemType* item : its) {
        this->addItem(static_cast<SuperClass::ItemType*>(item->copy()));
    }
}

template<class StdGroup>
CT_OutAbstractModel* CT_OutStdGroupModel<StdGroup>::copy() const
{
    return new CT_OutStdGroupModel(*this);
}

template<class StdGroup>
typename CT_OutStdGroupModel<StdGroup>::GroupType* CT_OutStdGroupModel<StdGroup>::findGroup(const int& uniqueIndex) const
{
    const SuperClass::GroupCollection& grps = groups();

    for(SuperClass::GroupType* group : grps) {
        if(group->uniqueIndex() == uniqueIndex)
            return group;
    }

    return nullptr;
}

template<class StdGroup>
typename CT_OutStdGroupModel<StdGroup>::ItemType* CT_OutStdGroupModel<StdGroup>::findItem(const int& uniqueIndex) const
{
    const SuperClass::ItemCollection& its = items();

    for(SuperClass::ItemType* item : its) {
        if(item->uniqueIndex() == uniqueIndex)
            return item;
    }

    return nullptr;
}
