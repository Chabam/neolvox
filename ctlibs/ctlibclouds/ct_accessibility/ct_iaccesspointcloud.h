#ifndef CT_IACCESSPOINTCLOUD_H
#define CT_IACCESSPOINTCLOUD_H

#include "ct_defines.h"

class CT_IAccessPointCloud
{
public:
    virtual ~CT_IAccessPointCloud() {}

    /**
     * @brief Returns the point cloud index
     */
    virtual const CT_AbstractPointCloudIndex* pointCloudIndex() const = 0;

    /**
     * @brief Returns the point cloud index registered
     */
    virtual CT_PCIR pointCloudIndexRegistered() const = 0;
};

#endif // CT_IACCESSPOINTCLOUD_H
