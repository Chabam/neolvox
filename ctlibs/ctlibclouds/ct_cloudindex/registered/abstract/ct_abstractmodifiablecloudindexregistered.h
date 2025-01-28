#ifndef CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTERED_H
#define CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTERED_H

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregistered.h"

class CT_AbstractModifiableCloudIndex;

class CTLIBCLOUDS_EXPORT CT_AbstractModifiableCloudIndexRegistered : virtual public CT_AbstractCloudIndexRegistered
{
public:
    CT_AbstractModifiableCloudIndexRegistered() : CT_AbstractCloudIndexRegistered() {}

    /**
     * @brief Returns the cloud index of this registered object
     */
    virtual CT_AbstractModifiableCloudIndex* abstractModifiableCloudIndex() const = 0;
};

#endif // CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTERED_H
