#ifndef CT_STANDARDNOTMODIFIABLECLOUDINDEXREGISTEREDT_H
#define CT_STANDARDNOTMODIFIABLECLOUDINDEXREGISTEREDT_H

#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"

/**
 * A standard NOT MODIFIABLE cloud of index (size_t) for T (point, face, etc...) registered
 */
template<typename T>
class CT_StandardNotModifiableCloudIndexRegisteredT : public CT_AbstractNotModifiableCloudIndexRegisteredT<T>
{
public:
    template<typename U> friend class CT_AbstractCloudIndexRegistrationManagerT;
    template<typename U> friend class CT_AbstractGlobalCloudManagerT;

    CT_StandardNotModifiableCloudIndexRegisteredT(CT_AbstractCloudIndexT<T> *ci);
};

#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.hpp"

#endif // CT_STANDARDNOTMODIFIABLECLOUDINDEXREGISTEREDT_H
