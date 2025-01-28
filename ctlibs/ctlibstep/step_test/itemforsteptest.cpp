#include "itemforsteptest.h"

ItemForStepTest::ItemForStepTest() : SuperClass()
{

}

CT_AbstractItemDrawable*ItemForStepTest::copy(const CT_OutAbstractItemModel* newModelToUse, const CT_AbstractResult* newResult) const
{
    ItemForStepTest* copy = new ItemForStepTest(*this);
    copy->setModel(newModelToUse);
    copy->setResult(newResult);

    return copy;
}
