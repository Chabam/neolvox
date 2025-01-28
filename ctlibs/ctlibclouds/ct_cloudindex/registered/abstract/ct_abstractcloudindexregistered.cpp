#include "ct_abstractcloudindexregistered.h"

#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"

CT_AbstractCloudIndexRegistered::CT_AbstractCloudIndexRegistered()
{
    m_autoDelete = true;
}

CT_AbstractCloudIndexRegistered::~CT_AbstractCloudIndexRegistered()
{
}

void CT_AbstractCloudIndexRegistered::setAutoDeleteCloudIndex(bool autoDelete)
{
    m_autoDelete = autoDelete;
}

bool CT_AbstractCloudIndexRegistered::mustAutoDeleteCloudIndex() const
{
    return m_autoDelete;
}
