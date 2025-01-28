#include "ct_cloudindex/ct_cloudindexstdlistt.h"

#include <functional>

template<typename T>
CT_CloudIndexStdListT<T>::CT_CloudIndexStdListT(const size_t &size) : CT_AbstractModifiableCloudIndexT<T>()
{
    this->internalSetSortType(CT_AbstractCloudIndex::SortedInAscendingOrder);
    this->internalSetUnregisteredWhenIsEmpty(false);

    m_collection = QSharedPointer< std::list<ct_index_type> >(new std::list<ct_index_type>(size));
}

template<typename T>
void CT_CloudIndexStdListT<T>::setSortType(CT_AbstractCloudIndex::SortType type)
{
    if(this->sortType() != type)
    {
        this->internalSetSortType(type);

        if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
            m_collection->sort();
    }
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::size() const
{
    return m_collection->size();
}


template<typename T>
size_t CT_CloudIndexStdListT<T>::memoryUsed() const
{
    return size() * sizeof(T);
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::indexAt(const size_t &i) const
{
    typename std::list<ct_index_type>::const_iterator it = m_collection->begin();
    std::advance(it, i);

    return *it;
}

template<typename T>
const ct_index_type& CT_CloudIndexStdListT<T>::constIndexAt(const size_t &i) const
{
    typename std::list<ct_index_type>::iterator it = m_collection->begin();
    std::advance(it, i);

    return *it;
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::operator[](const size_t &i) const
{
    typename std::list<ct_index_type>::const_iterator it = m_collection->begin();
    std::advance(it, i);

    return *it;
}

template<typename T>
void CT_CloudIndexStdListT<T>::indexAt(const size_t &i, size_t &index) const
{
    typename std::list<ct_index_type>::const_iterator it = m_collection->begin();
    std::advance(it, i);

    index = *it;
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::first() const
{
    return m_collection->front();
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::last() const
{
    return m_collection->back();
}

template<typename T>
bool CT_CloudIndexStdListT<T>::contains(const size_t &index) const
{
    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        return std::binary_search(m_collection->begin(), m_collection->end(), index);

    return (std::find(m_collection->begin(), m_collection->end(), index) != m_collection->end());
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::indexOf(const size_t &index) const
{
    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
    {
        typename std::list<ct_index_type>::iterator first = m_collection->begin();
        typename std::list<ct_index_type>::iterator last = m_collection->end();

        first = std::lower_bound(first, last, index);

        if(first!=last && !(static_cast<int>(index)<(*first)))
           return *first;
    }
    else
    {
        typename std::list<ct_index_type>::const_iterator it = std::find(m_collection->begin(), m_collection->end(), index);

        if(it != m_collection->end())
            return *it;
    }

    return size();
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::lowerBound(const size_t &value) const
{
    typename std::list<ct_index_type>::iterator it;
    typename std::list<ct_index_type>::iterator itEnd = m_collection->end();

    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        it = std::lower_bound(m_collection->begin(), m_collection->end(), value);
    else
        it = std::find_if(m_collection->begin(), m_collection->end(), std::bind(std::greater_equal<ct_index_type>(), std::placeholders::_1, ct_index_type(value)));

    return size() - std::distance(it, itEnd);
}

template<typename T>
size_t CT_CloudIndexStdListT<T>::upperBound(const size_t &value) const
{
    typename std::list<ct_index_type>::iterator it;
    typename std::list<ct_index_type>::iterator itEnd = m_collection->end();

    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        it = std::upper_bound(m_collection->begin(), m_collection->end(), value);
    else
        it = std::find_if(m_collection->begin(), m_collection->end(), std::bind(std::greater<ct_index_type>(), std::placeholders::_1, ct_index_type(value)));

    return size() - std::distance(it, itEnd);
}

template<typename T>
void CT_CloudIndexStdListT<T>::addIndex(const size_t &newIndex)
{
    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        m_collection->insert(std::lower_bound(m_collection->begin(), m_collection->end(), ct_index_type(newIndex)), ct_index_type(newIndex));
    else
        m_collection->push_back(ct_index_type(newIndex));
}

template<typename T>
void CT_CloudIndexStdListT<T>::removeIndex(const size_t &index)
{
    if(this->sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
    {
        typename std::list<ct_index_type>::iterator first = m_collection->begin();
        typename std::list<ct_index_type>::iterator last = m_collection->end();

        first = std::lower_bound(first, last, index);

        if(first!=last && !(static_cast<int>(index)<(*first)))
            m_collection->erase(first);
    }
    else
    {
        m_collection->erase(std::find(m_collection->begin(), m_collection->end(), index));
    }
}

template<typename T>
void CT_CloudIndexStdListT<T>::replaceIndex(const size_t &i, const ct_index_type &newIndex, const bool &verifyRespectSort)
{
    typename std::list<ct_index_type>::iterator it = m_collection->begin();
    std::advance(it, i);

    (*it) = newIndex;

    if(verifyRespectSort
            && (this->sortType() == CT_CloudIndexStdListT<T>::SortedInAscendingOrder))
    {
        bool ok = false;

        if(i>0)
        {
            typename std::list<ct_index_type>::const_iterator itP = it;
            --itP;

            if((*itP) <= newIndex)
                ok = true;
        }

        if(ok)
        {
            typename std::list<ct_index_type>::const_iterator itN = it;
            ++itN;

            if(itN != m_collection->end())
            {
                if((*itN) >= newIndex)
                    ok = true;
                else
                    ok = false;
            }
        }

        if(!ok)
            this->setSortType(CT_CloudIndexStdListT<T>::NotSorted);
    }
}

template<typename T>
ct_index_type& CT_CloudIndexStdListT<T>::operator[](const size_t &i)
{
    typename std::list<ct_index_type>::iterator it = m_collection->begin();
    std::advance(it, i);

    return *it;
}

template<typename T>
void CT_CloudIndexStdListT<T>::push_front(const size_t &newIndex)
{
    CT_AbstractCloudIndex::SortType t = this->sortType();

    if(t != CT_AbstractCloudIndex::NotSorted)
    {
        if(size() > 0)
        {
            if(newIndex > first())
                t = CT_AbstractCloudIndex::NotSorted;
        }
    }

    m_collection->insert(m_collection->begin(), ct_index_type(newIndex));
}

template<typename T>
void CT_CloudIndexStdListT<T>::fill()
{
    typename std::list<ct_index_type>::iterator it = m_collection->begin();
    typename std::list<ct_index_type>::iterator end = m_collection->end();

    ct_index_type i = 0;

    while(it != end)
    {
        (*it) = i;
        ++it;
        ++i;
    }
}

template<typename T>
void CT_CloudIndexStdListT<T>::clear()
{
    internalClear();
}

template<typename T>
void CT_CloudIndexStdListT<T>::erase(const size_t &beginIndex, const size_t &sizes)
{
    typename std::list<ct_index_type>::iterator b = m_collection->begin();
    std::advance(b, beginIndex);

    typename std::list<ct_index_type>::iterator e = m_collection->begin();
    std::advance(e, beginIndex+sizes);

    m_collection->erase(b, e);
}

template<typename T>
void CT_CloudIndexStdListT<T>::resize(const size_t &newSize)
{
    m_collection->resize(newSize, 0);
}

template<typename T>
void CT_CloudIndexStdListT<T>::reserve(const size_t &newSize)
{
    Q_UNUSED(newSize)
    qFatal("CT_CloudIndexStdListT<T>::reserve can not be used");
}

template<typename T>
void CT_CloudIndexStdListT<T>::removeIfOrShiftIf(typename CT_CloudIndexStdListT<T>::FindIfFunction findIf,
                                                typename CT_CloudIndexStdListT<T>::RemoveIfFunction removeIf,
                                                typename CT_CloudIndexStdListT<T>::ShiftIfFunction shiftIf,
                                                const size_t &shiftValue,
                                                const bool &negativeShift,
                                                void *context)
{
    typename std::list<ct_index_type>::iterator first = listFindIf<ct_index_type>(findIf, context);
    typename std::list<ct_index_type>::iterator last = m_collection->end();

    if(first != last)
    {
        typename std::list<ct_index_type>::iterator it = first;

        ct_index_type nI;

        while(it != last)
        {
            nI = *it;

            if((*shiftIf)(context, nI))
            {
                if(negativeShift)
                    *first = ct_index_type(nI-shiftValue);
                else
                    *first = ct_index_type(nI+shiftValue);

                ++first;
            }
            else if(!(*removeIf)(context, nI))
            {
                *first = nI;
                ++first;
            }

            ++it;
        }

        m_collection->erase(first, last);
    }
}

template<typename T>
void CT_CloudIndexStdListT<T>::shiftAll(const size_t &offset, const bool &negativeOffset)
{
    this->internalShiftAll(offset, negativeOffset);
}

template<typename T>
void CT_CloudIndexStdListT<T>::eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                                        const size_t &offset, const bool &negativeOffset)
{
    erase(eraseBeginPos, eraseSize);

    typename std::list<ct_index_type>::iterator b = m_collection->begin();
    typename std::list<ct_index_type>::iterator e = m_collection->end();
    std::advance(b, eraseBeginPos);

    if(negativeOffset)
    {
        while(b != e)
        {
            (*b) = ct_index_type((*b) - offset);
            ++b;
        }
    }
    else
    {
        while(b != e)
        {
            (*b) = ct_index_type((*b) + offset);
            ++b;
        }
    }
}

template<typename T>
CT_SharedPointer< std::vector<int> > CT_CloudIndexStdListT<T>::toStdVectorInt() const
{
    CT_SharedPointer< std::vector<int> > indices(new std::vector<int>(size()));

    typename std::list<ct_index_type>::iterator f = m_collection->begin();
    typename std::list<ct_index_type>::iterator l = m_collection->end();

    size_t i = 0;

    while(f != l)
    {
        (*indices.get())[i] = *f;
        ++f;
        ++i;
    }

    return indices;
}

template<typename T>
CT_AbstractCloud* CT_CloudIndexStdListT<T>::copy() const
{
    CT_CloudIndexStdListT<T> *index = new CT_CloudIndexStdListT<T>(size());

    std::copy(m_collection->begin(), m_collection->end(), index->m_collection->begin());

    index->setSortType(this->sortType());

    return index;
}

template<typename T>
template<typename S>
typename std::list<S>::iterator CT_CloudIndexStdListT<T>::listFindIf(typename CT_CloudIndexStdListT<T>::FindIfFunction findIf, void *context) const
{
    typename std::list<S>::iterator first = m_collection->begin();
    typename std::list<S>::iterator last = m_collection->end();

    size_t tmp;

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
std::list< ct_index_type >* CT_CloudIndexStdListT<T>::internalData() const
{
    return m_collection.data();
}

template<typename T>
void CT_CloudIndexStdListT<T>::internalShiftAll(const size_t &offset, const bool &negativeOffset)
{
    typename std::list<ct_index_type>::iterator first = m_collection->begin();
    typename std::list<ct_index_type>::iterator last = m_collection->end();

    if(negativeOffset)
    {
        while(first != last)
        {
            *first -= ct_index_type(offset);
            ++first;
        }
    }
    else
    {
        while(first != last)
        {
            *first += ct_index_type(offset);
            ++first;
        }
    }
}

template<typename T>
void CT_CloudIndexStdListT<T>::internalClear()
{
    m_collection->clear();
}
