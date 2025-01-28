#ifndef CT_ABSTRACTCLOUDINDEXT_H
#define CT_ABSTRACTCLOUDINDEXT_H

#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"
#include "ct_cloudindex/tools/ct_cloudindexiteratort.h"

/**
 * A cloud of index (size_t) for T (point, face, etc...)
 */
template<typename T>
class CT_AbstractCloudIndexT : virtual public CT_AbstractCloudIndex
{
public:
    CT_AbstractCloudIndexT() : CT_AbstractCloudIndex() {}

protected:
    template<typename U> friend class CT_AbstractGlobalCloudManagerT;

    virtual void internalShiftAll(const size_t &offset, const bool &negativeOffset) = 0;
    virtual void internalClear() = 0;
};

#endif // CT_ABSTRACTCLOUDINDEXT_H
