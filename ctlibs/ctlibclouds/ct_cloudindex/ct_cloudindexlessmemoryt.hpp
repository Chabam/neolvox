#include "ct_cloudindex/ct_cloudindexlessmemoryt.h"

template<typename T>
CT_CloudIndexLessMemoryT<T>::CT_CloudIndexLessMemoryT(const size_t &begin, const size_t &size) : CT_AbstractCloudIndexT<T>()
{
    internalClear();
    this->internalSetSortType(CT_AbstractCloudIndex::SortedInAscendingOrder);

    m_begin = begin;
    m_size = size;
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::size() const
{
    return m_size;
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::memoryUsed() const
{
    return 2*sizeof(size_t);
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::indexAt(const size_t &i) const
{
    return i+m_begin;
}

template<typename T>
const ct_index_type& CT_CloudIndexLessMemoryT<T>::constIndexAt(const size_t &i) const
{
    m_temporary = ct_index_type(i+m_begin);
    return m_temporary;
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::operator[](const size_t &i) const
{
    return i+m_begin;
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::indexAt(const size_t &i, size_t &index) const
{
    index = i+m_begin;
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::first() const
{
    return m_begin;
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::last() const
{
    return m_begin + (m_size-1);
}

template<typename T>
bool CT_CloudIndexLessMemoryT<T>::contains(const size_t &index) const
{
    if(size() == 0)
        return false;

    return ((index >= m_begin) && (index <= last()));
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::indexOf(const size_t &index) const
{
    if((index >= m_begin) && (index <= last()))
        return index-m_begin;

    return size();
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::lowerBound(const size_t &value) const
{
    if((value >= m_begin) && (value <= last()))
        return value-m_begin;

    return std::numeric_limits<size_t>::max();
}

template<typename T>
size_t CT_CloudIndexLessMemoryT<T>::upperBound(const size_t &value) const
{
    if((value >= m_begin) && (value <= last()))
        return (value-m_begin)+1;

    return std::numeric_limits<size_t>::max();
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::shiftAll(const size_t &offset, const bool &negativeOffset)
{
   internalShiftAll(offset, negativeOffset);
}

template<typename T>
bool CT_CloudIndexLessMemoryT<T>::cut(const size_t &begin, const size_t &end)
{
    if((begin >= first())
            && (end <= last()))
    {
        m_size = m_size - ((end - begin)+1);

        return true;
    }

    return false;
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::clear()
{
    internalClear();
}

template<typename T>
CT_SharedPointer< std::vector<int> > CT_CloudIndexLessMemoryT<T>::toStdVectorInt() const
{
    CT_SharedPointer< std::vector<int> > indices(new std::vector<int>(m_size));

    size_t a = m_begin;

    for(size_t i=0; i<m_size; ++i)
    {
        (*indices.get())[i] = int(a);
        ++a;
    }

    return indices;
}

template<typename T>
CT_AbstractCloud* CT_CloudIndexLessMemoryT<T>::copy() const
{
    return new CT_CloudIndexLessMemoryT<T>(m_begin, m_size);
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::erase(const size_t &beginIndex, const size_t &sizes)
{
    Q_UNUSED(beginIndex)
    Q_UNUSED(sizes)

    qFatal("CT_CloudIndexLessMemoryT<T>::erase can not be used");
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::resize(const size_t &newSize)
{
    m_size = newSize;
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::reserve(const size_t &newSize)
{
    Q_UNUSED(newSize)
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::internalShiftAll(const size_t &offset, const bool &negativeOffset)
{
    if(negativeOffset)
        m_begin -= offset;
    else
        m_begin += offset;
}

template<typename T>
void CT_CloudIndexLessMemoryT<T>::internalClear()
{
    m_begin = 0;
    m_size = 0;
}
