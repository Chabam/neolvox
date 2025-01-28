#include "ct_inresultmodelnotneedinputresult.h"

CT_InResultModelNotNeedInputResult::CT_InResultModelNotNeedInputResult() : SuperClass(tr("Not input result"))
{
}

bool CT_InResultModelNotNeedInputResult::visitChildrens(const CT_AbstractModel::ChildrenVisitor& visitor) const
{
    Q_UNUSED(visitor)

    return true;
}

bool CT_InResultModelNotNeedInputResult::isEmpty() const
{
    return true;
}

CT_InAbstractModel* CT_InResultModelNotNeedInputResult::copy(bool withPossibilities) const
{
    Q_UNUSED(withPossibilities)

    return new CT_InResultModelNotNeedInputResult(*this);
}
