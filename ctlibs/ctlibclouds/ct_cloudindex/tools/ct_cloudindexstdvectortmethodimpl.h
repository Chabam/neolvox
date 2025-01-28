#ifndef CT_CLOUDINDEXSTDVECTORTMETHODIMPL_H
#define CT_CLOUDINDEXSTDVECTORTMETHODIMPL_H

#include "ct_cloudindex/abstract/ct_abstractmodifiablecloudindext.h"

#include <vector>

template<typename T>
class CT_CloudIndexStdVectorTMethodImpl
{
public:
    typedef typename CT_AbstractModifiableCloudIndexT<T>::FindIfFunction     FindIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::RemoveIfFunction   RemoveIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::ShiftIfFunction    ShiftIfFunction;

    CT_CloudIndexStdVectorTMethodImpl(const CT_AbstractCloudIndex &ci, std::vector<T> &vector);

    void setSortType(CT_AbstractCloudIndex::SortType type);

    size_t size() const;
    size_t indexAt(const size_t &i) const;
    size_t operator[](const size_t &i) const;
    void indexAt(const size_t &i, size_t &index) const;
    const T &constIndexAt(const size_t &i) const;
    size_t first() const;
    size_t last() const;
    bool contains(const size_t &index) const;
    size_t indexOf(const size_t &index) const;
    size_t lowerBound(const size_t &value) const;
    size_t upperBound(const size_t &value) const;

    void addIndex(const size_t &newIndex);
    void removeIndex(const size_t &newIndex);
    void replaceIndex(const size_t &i, const T &newIndex, const bool &verifyRespectSort = true);
    void push_front(const size_t &newIndex);
    void fill();
    void clear();
    void erase(const size_t &beginIndex, const size_t &sizes);
    void resize(const size_t &newSize);
    void reserve(const size_t &newSize);

    void removeIfOrShiftIf(FindIfFunction findIf,
                                   RemoveIfFunction removeIf,
                                   ShiftIfFunction shiftIf,
                                   const size_t &shiftValue,
                                   const bool &negativeShift,
                                   void *context);

    void shiftAll(const size_t &offset, const bool &negativeOffset);

    typename std::vector<T>::iterator vectorFindIf(FindIfFunction findIf, void *context) const;

    void eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                          const size_t &offset, const bool &negativeOffset);

    void internalShiftAll(const size_t &offset, const bool &negativeOffset);
    void internalClear();

    void copy(std::vector<T> &destination);

private:
    std::vector<T>                  &_vector;
    const CT_AbstractCloudIndex     &m_ci;
};

#include "ct_cloudindex/tools/ct_cloudindexstdvectortmethodimpl.hpp"

#endif // CT_CLOUDINDEXSTDVECTORTMETHODIMPL_H
