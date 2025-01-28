#include "ct_abstractcloudindex.h"

CT_AbstractCloudIndex::CT_AbstractCloudIndex()
{
    internalSetSortType(NotSorted);
    internalSetUnregisteredWhenIsEmpty(true);
}

CT_AbstractCloudIndex::SortType CT_AbstractCloudIndex::sortType() const
{
    return m_sortType;
}

bool CT_AbstractCloudIndex::mustBeUnregisteredWhenIsEmpty() const
{
    return m_unregisterWhenEmpty;
}

void CT_AbstractCloudIndex::internalSetSortType(CT_AbstractCloudIndex::SortType st)
{
    m_sortType = st;
}

void CT_AbstractCloudIndex::internalSetUnregisteredWhenIsEmpty(bool enable)
{
    m_unregisterWhenEmpty = enable;
}
