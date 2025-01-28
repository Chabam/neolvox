#include "ct_repositoryaccess.h"

CT_RepositoryAccess::CT_RepositoryAccess(const CT_Repository *r) : m_repository(r)
{
}

CT_GlobalPointCloudManager *CT_RepositoryAccess::globalPointCloudManager() const
{
    return m_repository->m_gpcManager;
}

CT_GlobalFaceCloudManager *CT_RepositoryAccess::globalFaceCloudManager() const
{
    return m_repository->m_gfcManager;
}

CT_GlobalEdgeCloudManager *CT_RepositoryAccess::globalEdgeCloudManager() const
{
    return m_repository->m_gecManager;
}

CT_PointCloudIndexRegistrationManager *CT_RepositoryAccess::globalPointCloudIndexRegistrationManager() const
{
    return m_repository->m_pcirManager;
}

CT_FaceCloudIndexRegistrationManager *CT_RepositoryAccess::globalFaceCloudIndexRegistrationManager() const
{
    return m_repository->m_fcirManager;
}

CT_EdgeCloudIndexRegistrationManager *CT_RepositoryAccess::globalEdgeCloudIndexRegistrationManager() const
{
    return m_repository->m_ecirManager;
}

CT_SyncPointCloudManager *CT_RepositoryAccess::globalPointCloudSynchronizationManager() const
{
    return m_repository->m_syncPointCloudManager;
}

CT_SyncFaceCloudManager *CT_RepositoryAccess::globalFaceCloudSynchronizationManager() const
{
    return m_repository->m_syncFaceCloudManager;
}

CT_SyncEdgeCloudManager *CT_RepositoryAccess::globalEdgeCloudSynchronizationManager() const
{
    return m_repository->m_syncEdgeCloudManager;
}
