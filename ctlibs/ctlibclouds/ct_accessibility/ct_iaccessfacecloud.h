#ifndef CT_IACCESSFACECLOUD_H
#define CT_IACCESSFACECLOUD_H

#include "ct_defines.h"

class CT_IAccessFaceCloud
{
public:
    virtual ~CT_IAccessFaceCloud() {}

    /**
     * @brief Returns the face cloud index
     */
    virtual const CT_AbstractFaceCloudIndex* faceCloudIndex() const = 0;

    /**
     * @brief Returns the face cloud index registered
     */
    virtual CT_FCIR faceCloudIndexRegistered() const = 0;
};

#endif // CT_IACCESSFACECLOUD_H
