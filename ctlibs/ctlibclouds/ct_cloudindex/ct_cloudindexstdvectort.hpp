#include "ct_cloudindex/ct_cloudindexstdvectort.h"

#include <functional>
#include <QDebug>

template<typename T>
CT_CloudIndexStdVectorT<T>::CT_CloudIndexStdVectorT(const size_t &size) : CT_AbstractModifiableCloudIndexT<T>()
{
    this->internalSetSortType(CT_AbstractCloudIndex::SortedInAscendingOrder);

    _vector.resize(size);
    m_impl = new CT_CloudIndexStdVectorTMethodImpl<ct_index_type>(*this, _vector);
}

template<typename T>
CT_CloudIndexStdVectorT<T>::~CT_CloudIndexStdVectorT()
{
    delete m_impl;
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::setSortType(CT_AbstractCloudIndex::SortType type)
{
    m_impl->setSortType(type);
    this->internalSetSortType(type);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::size() const
{
    return m_impl->size();
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::memoryUsed() const
{
    return size() * sizeof(ct_index_type);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::indexAt(const size_t &i) const
{
    return m_impl->indexAt(i);
}

template<typename T>
const ct_index_type& CT_CloudIndexStdVectorT<T>::constIndexAt(const size_t &i) const
{
    return m_impl->constIndexAt(i);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::operator[](const size_t &i) const
{
    return m_impl->indexAt(i);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::indexAt(const size_t &i, size_t &index) const
{
    return m_impl->indexAt(i, index);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::first() const
{
    return m_impl->first();
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::last() const
{
    return m_impl->last();
}

template<typename T>
bool CT_CloudIndexStdVectorT<T>::contains(const size_t &index) const
{
    return m_impl->contains(index);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::indexOf(const size_t &index) const
{
    return m_impl->indexOf(index);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::lowerBound(const size_t &value) const
{
    return m_impl->lowerBound(value);
}

template<typename T>
size_t CT_CloudIndexStdVectorT<T>::upperBound(const size_t &value) const
{
    return m_impl->upperBound(value);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::addIndex(const size_t &newIndex)
{
    m_impl->addIndex(newIndex);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::removeIndex(const size_t &index)
{
    m_impl->removeIndex(index);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::replaceIndex(const size_t &i, const ct_index_type &newIndex, const bool &verifyRespectSort)
{
    m_impl->replaceIndex(i, newIndex, verifyRespectSort);
}

template<typename T>
ct_index_type& CT_CloudIndexStdVectorT<T>::operator[](const size_t &i)
{
    return _vector[i];
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::push_front(const size_t &newIndex)
{
    m_impl->push_front(newIndex);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::fill()
{
    m_impl->fill();
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::clear()
{
    m_impl->clear();
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::erase(const size_t &beginIndex, const size_t &sizes)
{
    m_impl->erase(beginIndex, sizes);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::resize(const size_t &newSize)
{
    m_impl->resize(newSize);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::reserve(const size_t &newSize)
{
    m_impl->reserve(newSize);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::removeIfOrShiftIf(typename CT_CloudIndexStdVectorT<T>::FindIfFunction findIf,
                                                typename CT_CloudIndexStdVectorT<T>::RemoveIfFunction removeIf,
                                                typename CT_CloudIndexStdVectorT<T>::ShiftIfFunction shiftIf,
                                                const size_t &shiftValue,
                                                const bool &negativeShift,
                                                void *context)
{

    m_impl->removeIfOrShiftIf(findIf,
                              removeIf,
                              shiftIf,
                              shiftValue,
                              negativeShift,
                              context);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::shiftAll(const size_t &offset, const bool &negativeOffset)
{
    m_impl->shiftAll(offset, negativeOffset);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                                          const size_t &offset, const bool &negativeOffset)
{
    m_impl->eraseBetweenAndShiftRest(eraseBeginPos, eraseSize, offset, negativeOffset);
}

template<typename T>
CT_SharedPointer< std::vector<int> > CT_CloudIndexStdVectorT<T>::toStdVectorInt() const
{
    // c++11
    static_assert(sizeof(int) == sizeof(ct_index_type), "CT_CloudIndexStdVectorT<T>::toStdVectorInt can not be used like this !");

    // create a shared pointer that will not delete this vector (second parameter = false)
    return CT_SharedPointer< std::vector<int> >(&const_cast<std::vector<int>&>(_vector), false);
}

template<typename T>
CT_AbstractCloud* CT_CloudIndexStdVectorT<T>::copy() const
{
    CT_CloudIndexStdVectorT<T> *index = new CT_CloudIndexStdVectorT<T>(size());
    index->setSortType(this->sortType());

    m_impl->copy(*index->internalData());

    return index;
}

template<typename T>
typename std::vector<ct_index_type>::iterator CT_CloudIndexStdVectorT<T>::vectorFindIf(typename CT_CloudIndexStdVectorT<T>::FindIfFunction findIf, void *context) const
{
    typename std::vector<ct_index_type>::iterator first = _vector.begin();
    typename std::vector<ct_index_type>::iterator last = _vector.end();

    ct_index_type tmp;

    while (first!=last)
    {
        tmp = *first;

        if ((*findIf)(context, tmp))
            return first;

        ++first;
    }

    return last;
}

template<typename T>
std::vector< ct_index_type >* CT_CloudIndexStdVectorT<T>::internalData() const
{
    return &const_cast< std::vector<ct_index_type>& >(_vector);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::internalShiftAll(const size_t &offset, const bool &negativeOffset)
{
    m_impl->internalShiftAll(offset, negativeOffset);
}

template<typename T>
void CT_CloudIndexStdVectorT<T>::internalClear()
{
    m_impl->internalClear();
}
