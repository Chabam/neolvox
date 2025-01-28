#ifndef CT_ABSTRACTCLOUDINDEXREGISTRATIONMANAGER_H
#define CT_ABSTRACTCLOUDINDEXREGISTRATIONMANAGER_H

#include "ctlibclouds_global.h"

/**
 * A utility class that register cloud index and sync indexes with
 * the global cloud when elements was deleted from them.
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCloudIndexRegistrationManager
{
public:
    CT_AbstractCloudIndexRegistrationManager();
    virtual ~CT_AbstractCloudIndexRegistrationManager();
};

#endif // CT_ABSTRACTCLOUDINDEXREGISTRATIONMANAGER_H
