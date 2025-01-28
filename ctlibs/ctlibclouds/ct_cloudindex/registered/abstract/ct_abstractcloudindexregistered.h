#ifndef CT_ABSTRACTCLOUDINDEXREGISTERED_H
#define CT_ABSTRACTCLOUDINDEXREGISTERED_H

#include "ctlibclouds_global.h"

class CT_AbstractCloudIndex;

/**
 * A cloud of index registered
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCloudIndexRegistered
{
public:
    typedef void (*DeleteHandler)(CT_AbstractCloudIndex *ci);

    CT_AbstractCloudIndexRegistered();
    virtual ~CT_AbstractCloudIndexRegistered();

    /**
     * @brief Return the cloud index
     */
    virtual CT_AbstractCloudIndex* abstractCloudIndex() const = 0;

    void setAutoDeleteCloudIndex(bool autoDelete);
    bool mustAutoDeleteCloudIndex() const;

protected:
    bool   m_autoDelete;
};

#endif // CT_ABSTRACTCLOUDINDEXREGISTERED_H
