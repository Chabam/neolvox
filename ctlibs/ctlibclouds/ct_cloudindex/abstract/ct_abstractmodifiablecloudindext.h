#ifndef CT_ABSTRACTMODIFIABLECLOUDINDEXT_H
#define CT_ABSTRACTMODIFIABLECLOUDINDEXT_H

#include "ct_cloudindex/abstract/ct_abstractcloudindext.h"
#include "ct_cloudindex/tools/ct_modifiablecloudindexiteratort.h"
#include "ct_cloudindex/abstract/ct_abstractmodifiablecloudindex.h"

/**
 * A MODIFIABLE cloud of index (size_t) for T (point, face, etc...)
 */
template<typename T>
class CT_AbstractModifiableCloudIndexT : virtual public CT_AbstractCloudIndexT<T>, public CT_AbstractModifiableCloudIndex
{
public:
    CT_AbstractModifiableCloudIndexT() : CT_AbstractCloudIndexT<T>() {}

    /**
     * @brief removes all indexes between beginIndex and (beginIndex+sizes-1) included
     */
    virtual void erase(const size_t &beginIndex, const size_t &sizes) = 0;

    /**
     * @brief Resize the cloud index
     */
    virtual void resize(const size_t &newSize) = 0;

    /**
     * @brief Reserve the cloud index
     */
    virtual void reserve(const size_t &newSize) = 0;
};

#endif // CT_ABSTRACTMODIFIABLECLOUDINDEXT_H
