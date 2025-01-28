#ifndef CT_CLOUDINDEXLESSMEMORYT_H
#define CT_CLOUDINDEXLESSMEMORYT_H

#include "ct_cloudindex/abstract/ct_abstractcloudindext.h"

/**
 * A cloud of index (size_t) for T (point, face, etc...) that cannot be modifiabled but use just
 * two size_t (begin and size) in memory.
 */
template<typename T>
class CT_CloudIndexLessMemoryT : virtual public CT_AbstractCloudIndexT<T>
{
public:
    CT_CloudIndexLessMemoryT(const size_t &begin, const size_t &size);

    // CT_AbstractCloudIndex
    bool doIndexesFollowEachOther() const { return true; }
    size_t size() const;
    size_t memoryUsed() const;
    size_t indexAt(const size_t &i) const;
    const ct_index_type& constIndexAt(const size_t &i) const;
    size_t operator[](const size_t &i) const;
    void indexAt(const size_t &i, size_t &index) const;
    size_t first() const;
    size_t last() const;
    bool contains(const size_t &index) const;
    size_t indexOf(const size_t &index) const;
    size_t lowerBound(const size_t &value) const;
    size_t upperBound(const size_t &value) const;

    void shiftAll(const size_t &offset, const bool &negativeOffset);
    bool cut(const size_t &begin, const size_t &end);
    void clear();

    CT_SharedPointer< std::vector<int> > toStdVectorInt() const;

    CT_AbstractCloud* copy() const;

private:
    template<typename A, class CloudIndexLessMemory> friend class CT_CloudIndexRegistrationManagerT;
    template<typename B, typename CLOUD> friend class CT_GlobalCloudManagerT;

    size_t                                      m_begin;
    size_t                                      m_size;
    mutable ct_index_type                       m_temporary;

protected:
    void erase(const size_t &beginIndex, const size_t &sizes);
    void resize(const size_t &newSize);
    void reserve(const size_t &newSize);
    void internalShiftAll(const size_t &offset, const bool &negativeOffset);
    void internalClear();
};

#include "ct_cloudindex/ct_cloudindexlessmemoryt.hpp"

#endif // CT_CLOUDINDEXLESSMEMORYT_H
