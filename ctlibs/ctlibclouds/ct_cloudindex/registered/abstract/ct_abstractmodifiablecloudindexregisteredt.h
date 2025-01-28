#ifndef CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTEREDT_H
#define CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTEREDT_H

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregistered.h"

#include "ct_cloudindex/abstract/ct_abstractmodifiablecloudindext.h"
/**
 * A MODIFIABLE cloud of index (size_t) for T (point, face, etc...) registered
 */
template<typename T>
class CT_AbstractModifiableCloudIndexRegisteredT : public CT_AbstractCloudIndexRegisteredT<T>, public CT_AbstractModifiableCloudIndexRegistered
{
public:
    ~CT_AbstractModifiableCloudIndexRegisteredT();

    inline void addIndex(const size_t &newIndex) { abstractModifiableCloudIndexT()->addIndex(newIndex); }

    inline const ct_index_type& operator[](const size_t &i) { return abstractModifiableCloudIndexT()->constIndexAt(i); }

    inline void push_front(const size_t &newIndex) { abstractModifiableCloudIndexT()->push_front(newIndex); }

    inline void fill() { abstractModifiableCloudIndexT()->fill(); }

    inline void clear() { abstractModifiableCloudIndexT()->clear(); }

    inline void erase(const size_t &beginIndex, const size_t &sizes) { abstractModifiableCloudIndexT()->erase(beginIndex, sizes); }

    inline void resize(const size_t &newSize) { abstractModifiableCloudIndexT()->resize(newSize); }

    inline void reserve(const size_t &newSize) { abstractModifiableCloudIndexT()->reserve(newSize); }

    typedef typename CT_AbstractModifiableCloudIndexT<T>::FindIfFunction FindIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::RemoveIfFunction RemoveIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::ShiftIfFunction ShiftIfFunction;

    inline void removeIfOrShiftIf(FindIfFunction findIf, RemoveIfFunction removeIf, ShiftIfFunction shiftIf, const size_t &shiftValue, const bool &negativeShift, void *context)
    {
        abstractModifiableCloudIndexT()->removeIfOrShiftIf(findIf, removeIf, shiftIf, shiftValue, negativeShift, context);
    }

    inline void shiftAll(const size_t &offset, const bool &negativeOffset) { abstractModifiableCloudIndexT()->shiftAll(offset, negativeOffset); }

    CT_AbstractModifiableCloudIndexT<T>* operator->() const;
    CT_AbstractModifiableCloudIndexT<T>* abstractModifiableCloudIndexT() const;
    CT_AbstractModifiableCloudIndex* abstractModifiableCloudIndex() const;
    CT_AbstractCloudIndex* abstractCloudIndex() const { return CT_AbstractCloudIndexRegisteredT<T>::abstractCloudIndex(); }

protected:
    CT_AbstractModifiableCloudIndexRegisteredT(CT_AbstractModifiableCloudIndexT<T> *ci);

private:
    CT_AbstractModifiableCloudIndexT<T> *m_modifiableIndex;
};

#include "ct_cloudindex/registered/abstract/ct_abstractmodifiablecloudindexregisteredt.hpp"

#endif // CT_ABSTRACTMODIFIABLECLOUDINDEXREGISTEREDT_H
