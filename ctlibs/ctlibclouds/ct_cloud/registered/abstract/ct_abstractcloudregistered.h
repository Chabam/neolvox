#ifndef CT_ABSTRACTCLOUDREGISTERED_H
#define CT_ABSTRACTCLOUDREGISTERED_H

#include "ct_cloud/abstract/ct_abstractcloud.h"

class CT_AbstractCloudSyncToGlobalCloudManager;

/**
 * A cloud registered
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCloudRegistered
{
public:
    CT_AbstractCloudRegistered();
    virtual ~CT_AbstractCloudRegistered();

    /**
     * @brief Returns the abstract cloud
     */
    virtual CT_AbstractCloud* abstractCloud() const = 0;

    /**
     * @brief Returns the manager of synchronization for this cloud
     */
    CT_AbstractCloudSyncToGlobalCloudManager* syncCloudManager() const;

protected:

    friend class CT_AbstractCloudSyncToGlobalCloudManager;

    /**
     * @brief Called from CT_AbstractCloudSyncToGlobalCloudManager to set it
     */
    void setSyncCloudManager(const CT_AbstractCloudSyncToGlobalCloudManager *manager);

private:

    CT_AbstractCloudSyncToGlobalCloudManager *m_syncManager;
};

#endif // CT_ABSTRACTCLOUDREGISTERED_H
