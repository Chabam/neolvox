#ifndef CT_CLOUDINDEXSTDMAPT_H
#define CT_CLOUDINDEXSTDMAPT_H

#include "ct_cloudindex/abstract/ct_abstractmodifiablecloudindext.h"

/**
 * A cloud of index = key (size_t) for T (point, face, etc...) with a value of type ValueT
 * that can be modified.
 */
template<typename T, typename ValueT>
class CT_CloudIndexStdMapT : public CT_AbstractModifiableCloudIndexT<T>
{
public:

    typedef typename CT_AbstractModifiableCloudIndexT<T>::FindIfFunction     FindIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::RemoveIfFunction   RemoveIfFunction;
    typedef typename CT_AbstractModifiableCloudIndexT<T>::ShiftIfFunction    ShiftIfFunction;

    CT_CloudIndexStdMapT();

    // CT_AbstractCloudIndex
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

    // CT_AbstractModifiableCloudIndex
    void addIndex(const size_t &newIndex);
    void removeIndex(const size_t &index);
    void replaceIndex(const size_t &i, const ct_index_type &newIndex, const bool &verifyRespectSort = true);

    /**
     * @brief Use it to replace an index if you want or to get a reference to it
     * @warning CAUTION ! If you modify the index and sortType() != NotSorted the cloud will be corrupted !
     */
    ct_index_type& operator[](const size_t &i);

    /**
     * @brief insert index and value in the map. If the index already exist, the value is replaced by the new.
     */
    void insertIndexAndValue(const size_t &index, const ValueT & value);

    /**
     * @brief return the value for the index of point, face or edge in the map
     */
    const ValueT& valueAtGlobalIndex(const size_t &index, const ValueT & defaultValue = ValueT()) const;

    /**
     * @brief return the value for the index 'index' in the map
     */
    const ValueT& valueAt(const size_t &index, const ValueT & defaultValue = ValueT()) const;

    typename std::map<ct_index_type,ValueT >::const_iterator cbegin() const;

    typename std::map<ct_index_type,ValueT >::const_iterator cend() const;

    typename std::map<ct_index_type,ValueT >::const_iterator findAtGlobalIndex(const size_t& index) const;

    typename std::map<ct_index_type,ValueT >::const_iterator findAtLocalIndex(const size_t& index) const;

    /**
     * @warning Don't use this method. show the definition of this method in the superclass
     *
     * PREFERS addIndex method
     */
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
    void shiftAll(const size_t& offset, const bool &negativeOffset);
    void eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                  const size_t &offset, const bool &negativeOffset);


    CT_SharedPointer< std::vector<int> > toStdVectorInt() const;

    CT_AbstractCloud* copy() const;

private:
    QSharedPointer< std::map<ct_index_type,ValueT > >    m_collection;

    template<typename S>
    typename std::map<S,ValueT >::iterator mapFindIf(FindIfFunction findIf, void *context) const;

protected:

    template<typename U> friend class CT_AbstractGlobalCloudManagerT;

    std::map< ct_index_type, ValueT >* internalData() const;
    void internalShiftAll(const size_t &offset, const bool &negativeOffset);
    void internalClear();
};

#include "ct_cloudindex/ct_cloudindexstdmapt.hpp"

#endif // CT_CLOUDINDEXSTDMAPT_H
