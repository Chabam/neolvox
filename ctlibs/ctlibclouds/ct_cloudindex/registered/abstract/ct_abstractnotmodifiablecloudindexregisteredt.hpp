#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"

#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"

template<typename T>
CT_AbstractNotModifiableCloudIndexRegisteredT<T>::~CT_AbstractNotModifiableCloudIndexRegisteredT()
{
    if(this->cloudIndexRegistrationManager() != nullptr)
        this->cloudIndexRegistrationManager()->unregisterIndex(this);
}
