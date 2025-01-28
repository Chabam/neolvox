#ifndef CT_IACCESSEDGECLOUD_H
#define CT_IACCESSEDGECLOUD_H

#include "ct_defines.h"

class CT_IAccessEdgeCloud
{
public:
    virtual ~CT_IAccessEdgeCloud() {}

    /**
     * @brief Returns the edge cloud index
     */
    virtual const CT_AbstractEdgeCloudIndex* edgeCloudIndex() const = 0;

    /**
     * @brief Returns the edge cloud index registered
     */
    virtual CT_ECIR edgeCloudIndexRegistered() const = 0;
};

#endif // CT_IACCESSEDGECLOUD_H
