#include "ct_abstractcloudregistered.h"

#include "ct_cloud/tools/abstract/ct_abstractcloudsynctoglobalcloudmanager.h"

CT_AbstractCloudRegistered::CT_AbstractCloudRegistered()
{
    m_syncManager = nullptr;
}

CT_AbstractCloudRegistered::~CT_AbstractCloudRegistered()
{
    if(m_syncManager != nullptr)
        m_syncManager->internalUnregisterCloud(this);
}

void CT_AbstractCloudRegistered::setSyncCloudManager(const CT_AbstractCloudSyncToGlobalCloudManager *manager)
{
    m_syncManager = (CT_AbstractCloudSyncToGlobalCloudManager*)manager;
}

CT_AbstractCloudSyncToGlobalCloudManager* CT_AbstractCloudRegistered::syncCloudManager() const
{
    return m_syncManager;
}
