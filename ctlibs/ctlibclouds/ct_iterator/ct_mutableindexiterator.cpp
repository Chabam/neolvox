#include "ct_mutableindexiterator.h"

#include "ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregistered.h"

CT_MutableIndexIterator::CT_MutableIndexIterator(const CT_AbstractModifiableCloudIndex *mci)
{
    m_index = (CT_AbstractModifiableCloudIndex*)mci;
    m_startPos = 0;
    m_endPos = (m_index == nullptr) ? 0 : m_index->size();

    toFront();
}

CT_MutableIndexIterator::CT_MutableIndexIterator(const CT_MCIR mcir)
{
    if(mcir.data() == nullptr)
        m_index = nullptr;
    else
        m_index = mcir->abstractModifiableCloudIndex();

    m_startPos = 0;
    m_endPos = (m_index == nullptr) ? 0 : m_index->size();

    toFront();
}
