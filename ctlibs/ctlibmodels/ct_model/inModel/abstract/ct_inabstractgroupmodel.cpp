#include "ct_inabstractgroupmodel.h"

#include "ct_model/outModel/abstract/ct_outabstractgroupmodel.h"

CT_InAbstractGroupModel::CT_InAbstractGroupModel(const QString& displayableName) : SuperClass(displayableName)
{
}

CT_InAbstractGroupModel::CT_InAbstractGroupModel(const CT_InAbstractGroupModel& other) : SuperClass(other)
{
}

bool CT_InAbstractGroupModel::canBeComparedWith(const CT_OutAbstractModel &model) const
{
    // ok if the out model represent a group
    return (dynamic_cast<const CT_OutAbstractGroupModel*>(&model) != nullptr);
}
