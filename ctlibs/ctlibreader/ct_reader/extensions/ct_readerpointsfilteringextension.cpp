#include "ct_readerpointsfilteringextension.h"

CT_ReaderPointsFilteringExtension::CT_ReaderPointsFilteringExtension()
{
    m_pointsFilter = nullptr;
}

void CT_ReaderPointsFilteringExtension::setPointsFilter(CT_ReaderPointsFilteringExtension::filterPointsFunction f)
{
    m_pointsFilter = f;
}

CT_ReaderPointsFilteringExtension::filterPointsFunction CT_ReaderPointsFilteringExtension::pointsFilter() const
{
    return m_pointsFilter;
}

bool CT_ReaderPointsFilteringExtension::isPointFiltered(const CT_Point& pt) const
{
    if(m_pointsFilter == nullptr)
        return false;

    return m_pointsFilter(pt);
}

