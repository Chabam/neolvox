#include "ct_outabstractresultmodelgroup.h"

CT_OutAbstractResultModelGroup::CT_OutAbstractResultModelGroup(const QString& displayableName) : SuperClass(displayableName)
{
}

bool CT_OutAbstractResultModelGroup::visitChildrens(const CT_AbstractModel::ChildrenVisitor& visitor) const
{
    const DEF_CT_AbstractGroupModelOut* rootG = rootGroup();

    if(rootG == nullptr)
        return true;

    return visitor(rootG);
}

bool CT_OutAbstractResultModelGroup::isEmpty() const
{
    return (rootGroup() == nullptr);
}

