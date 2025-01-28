#ifndef CT_STDCLOUDREGISTEREDT_H
#define CT_STDCLOUDREGISTEREDT_H

#include "ct_cloud/registered/abstract/ct_abstractcloudregistered.h"

/**
 * A cloud registered of T (T must be the type of the cloud, per example CT_StandardCloudStdVectorT<GLubyte> ). It's size is sync with a global cloud.
 */
template<typename T >
class CT_StdCloudRegisteredT : public CT_AbstractCloudRegistered
{
public:
    ~CT_StdCloudRegisteredT();

    /**
     * @brief Returns the abstract cloud
     */
    CT_AbstractCloud* abstractCloud() const;

    /**
     * @brief Returns the cloud
     */
    T* cloudT() const;

protected:

    friend class CT_CloudSyncToGlobalCloudManager;

    CT_StdCloudRegisteredT(T *cloud);

private:
    T                   *m_cloud;
};

#include "ct_cloud/registered/ct_stdcloudregisteredt.hpp"

#endif // CT_STDCLOUDREGISTEREDT_H
