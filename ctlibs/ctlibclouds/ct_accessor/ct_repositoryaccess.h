#ifndef CT_REPOSITORYACCESS_H
#define CT_REPOSITORYACCESS_H

#include "ct_global/ct_repository.h"

class CTLIBCLOUDS_EXPORT CT_RepositoryAccess
{
public:
    CT_RepositoryAccess(const CT_Repository *r);

    CT_GlobalPointCloudManager* globalPointCloudManager() const;
    CT_GlobalFaceCloudManager* globalFaceCloudManager() const;
    CT_GlobalEdgeCloudManager* globalEdgeCloudManager() const;

    CT_PointCloudIndexRegistrationManager* globalPointCloudIndexRegistrationManager() const;
    CT_FaceCloudIndexRegistrationManager* globalFaceCloudIndexRegistrationManager() const;
    CT_EdgeCloudIndexRegistrationManager* globalEdgeCloudIndexRegistrationManager() const;

    CT_SyncPointCloudManager* globalPointCloudSynchronizationManager() const;
    CT_SyncFaceCloudManager* globalFaceCloudSynchronizationManager() const;
    CT_SyncEdgeCloudManager* globalEdgeCloudSynchronizationManager() const;

private:
    const CT_Repository *m_repository;
};

#endif // CT_REPOSITORYACCESS_H
