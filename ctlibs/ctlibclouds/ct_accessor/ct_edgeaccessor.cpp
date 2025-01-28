#include "ct_edgeaccessor.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_edgecloud/abstract/ct_abstractedgecloud.h"

CT_EdgeAccessor::CT_EdgeAccessor()
{
    m_eCloud = PS_REPOSITORY->globalEdgeCloud();
}

const CT_Edge& CT_EdgeAccessor::constEdgeAt(const size_t &globalIndex) const
{
    return m_eCloud->constTAt(globalIndex);
}

CT_Edge& CT_EdgeAccessor::edgeAt(const size_t &globalIndex)
{
    return m_eCloud->tAt(globalIndex);
}

size_t CT_EdgeAccessor::size() const
{
    return m_eCloud->size();
}
