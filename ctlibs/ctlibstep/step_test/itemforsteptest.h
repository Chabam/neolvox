#ifndef ITEMFORSTEPTEST_H
#define ITEMFORSTEPTEST_H

#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

class ItemForStepTest : public CT_AbstractSingularItemDrawable
{
    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    ItemForStepTest();

    CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel *newModelToUse, const CT_AbstractResult *newResult) const override;
};

#endif // ITEMFORSTEPTEST_H
