#ifndef CT_STANDARDMODIFIABLECLOUDINDEXREGISTEREDT_H
#define CT_STANDARDMODIFIABLECLOUDINDEXREGISTEREDT_H

#include "ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregisteredt.h"

/**
 * A standard MODIFIABLE cloud of index (size_t) for T (point, face, etc...) registered
 */
template<typename T>
class CT_StandardModifiableCloudIndexRegisteredT : public CT_AbstractModifiableCloudIndexRegisteredT<T>
{
public:
    CT_AbstractCloudIndex* abstractCloudIndex() const { return CT_AbstractCloudIndexRegisteredT<T>::abstractCloudIndex(); }

protected:
    template<typename U> friend class CT_AbstractCloudIndexRegistrationManagerT;

    CT_StandardModifiableCloudIndexRegisteredT(CT_AbstractModifiableCloudIndexT<T> *ci);
};

#include "ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.hpp"

#endif // CT_STANDARDMODIFIABLECLOUDINDEXREGISTEREDT_H
