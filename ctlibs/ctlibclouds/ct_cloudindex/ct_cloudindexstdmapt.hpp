#include "ct_cloudindex/ct_cloudindexstdmapt.h"

#include <functional>

template<typename T, typename ValueT>
CT_CloudIndexStdMapT<T, ValueT>::CT_CloudIndexStdMapT() : CT_AbstractModifiableCloudIndexT<T>()
{
    this->internalSetSortType(CT_AbstractCloudIndex::SortedInAscendingOrder);
    this->internalSetUnregisteredWhenIsEmpty(false);

    m_collection = QSharedPointer< std::map<ct_index_type,ValueT > >(new std::map<ct_index_type,ValueT >());
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::size() const
{
    return m_collection->size();
}


template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::memoryUsed() const
{
    return size() * (sizeof(T) + sizeof(ValueT));
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::indexAt(const size_t &i) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, i);

    return it->first;
}

template<typename T, typename ValueT>
const ct_index_type& CT_CloudIndexStdMapT<T, ValueT>::constIndexAt(const size_t &i) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, i);

    return it->first;
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::operator[](const size_t &i) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, i);

    return it->first;
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::indexAt(const size_t &i, size_t &index) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, i);

    index = it->first;
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::first() const
{
    return m_collection->begin()->first;
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::last() const
{
    return m_collection->rbegin()->first;
}

template<typename T, typename ValueT>
bool CT_CloudIndexStdMapT<T, ValueT>::contains(const size_t &index) const
{
    return (m_collection->find(ct_index_type(index)) != m_collection->end());
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::indexOf(const size_t &index) const
{
    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    typename std::map<ct_index_type,ValueT >::iterator l = m_collection->end();

    f = m_collection->lower_bound(ct_index_type(index));

    if(f!=l && !(static_cast<int>(index) < f->first))
        return f->first;

    return size();
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::lowerBound(const size_t &value) const
{
    typename std::map<ct_index_type,ValueT >::iterator it;
    typename std::map<ct_index_type,ValueT >::iterator itEnd = m_collection->end();

    it = m_collection->lower_bound(ct_index_type(value));

    return size() - std::distance(it, itEnd);
}

template<typename T, typename ValueT>
size_t CT_CloudIndexStdMapT<T, ValueT>::upperBound(const size_t &value) const
{
    typename std::map<ct_index_type,ValueT >::iterator it;
    typename std::map<ct_index_type,ValueT >::iterator itEnd = m_collection->end();

    it = m_collection->upper_bound(ct_index_type(value));

    return size() - std::distance(it, itEnd);
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::addIndex(const size_t &newIndex)
{
    ValueT val = ValueT();
    insertIndexAndValue(newIndex, val);
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::removeIndex(const size_t &index)
{
    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    typename std::map<ct_index_type,ValueT >::iterator l = m_collection->end();

    f = m_collection->lower_bound(ct_index_type(index));

    if(f!=l && !(static_cast<int>(index) < f->first))
        m_collection->erase(f);
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::replaceIndex(const size_t &i, const ct_index_type &newIndex, const bool &verifyRespectSort)
{
    Q_UNUSED(verifyRespectSort)

    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    std::advance(f, i);

    ValueT val = f->second;
    m_collection->erase(f);

    insertIndexAndValue(newIndex, val);
}

template<typename T, typename ValueT>
ct_index_type& CT_CloudIndexStdMapT<T, ValueT>::operator[](const size_t &i)
{
    Q_UNUSED(i)

    qFatal("CT_CloudIndexStdMapT<T, ValueT>::operator[] can not be used, use CT_CloudIndexStdMapT<T, ValueT>::replaceIndex(...) !");
    static ct_index_type tmp;
    return tmp;
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::insertIndexAndValue(const size_t &index, const ValueT & value)
{
    std::pair< typename std::map<ct_index_type,ValueT >::iterator, bool > ret;
    ret = m_collection->insert( std::pair<ct_index_type,ValueT >(ct_index_type(index),value) );

    if (ret.second == false)
        ret.first->second = value;
}

template<typename T, typename ValueT>
const ValueT& CT_CloudIndexStdMapT<T, ValueT>::valueAtGlobalIndex(const size_t &index, const ValueT & defaultValue) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->find(ct_index_type(index));

    if(it != m_collection->end())
        return it->second;

    return defaultValue;
}

template<typename T, typename ValueT>
const ValueT& CT_CloudIndexStdMapT<T, ValueT>::valueAt(const size_t &index, const ValueT & defaultValue) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, index);

    if(it != m_collection->end())
        return it->second;

    return defaultValue;
}

template<typename T, typename ValueT>
typename std::map<ct_index_type,ValueT >::const_iterator CT_CloudIndexStdMapT<T, ValueT>::cbegin() const
{
    return m_collection->begin();
}

template<typename T, typename ValueT>
typename std::map<ct_index_type,ValueT >::const_iterator CT_CloudIndexStdMapT<T, ValueT>::cend() const
{
    return m_collection->end();
}

template<typename T, typename ValueT>
typename std::map<ct_index_type,ValueT >::const_iterator CT_CloudIndexStdMapT<T, ValueT>::findAtGlobalIndex(const size_t& index) const
{
    return m_collection->find(ct_index_type(index));
}

template<typename T, typename ValueT>
typename std::map<ct_index_type,ValueT >::const_iterator CT_CloudIndexStdMapT<T, ValueT>::findAtLocalIndex(const size_t& index) const
{
    typename std::map<ct_index_type,ValueT >::const_iterator it = m_collection->begin();
    std::advance(it, index);

    return it;
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::push_front(const size_t &newIndex)
{
    m_collection->insert( std::pair<ct_index_type, ValueT >(ct_index_type(newIndex), ValueT()) );
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::fill()
{
    qFatal("CT_CloudIndexStdMapT<T, ValueT>::fill can not be used");
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::clear()
{
    internalClear();
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::erase(const size_t &beginIndex, const size_t &sizes)
{
    typename std::map<ct_index_type,ValueT >::iterator b = m_collection->begin();
    std::advance(b, beginIndex);

    typename std::map<ct_index_type,ValueT >::iterator e = m_collection->begin();
    std::advance(e, beginIndex+sizes);

    m_collection->erase(b, e);
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::resize(const size_t &newSize)
{
    Q_UNUSED(newSize)
    qFatal("CT_CloudIndexStdMapT<T, ValueT>::resize can not be used");
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::reserve(const size_t &newSize)
{
    Q_UNUSED(newSize)
    qFatal("CT_CloudIndexStdMapT<T, ValueT>::reserve can not be used");
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::removeIfOrShiftIf(typename CT_CloudIndexStdMapT<T, ValueT>::FindIfFunction findIf,
                                                typename CT_CloudIndexStdMapT<T, ValueT>::RemoveIfFunction removeIf,
                                                typename CT_CloudIndexStdMapT<T, ValueT>::ShiftIfFunction shiftIf,
                                                const size_t &shiftValue,
                                                const bool &negativeShift,
                                                void *context)
{
    Q_UNUSED(findIf)
    Q_UNUSED(removeIf)
    Q_UNUSED(shiftIf)
    Q_UNUSED(shiftValue)
    Q_UNUSED(negativeShift)
    Q_UNUSED(context)
    // used only for not sorted cloud index
    qFatal("CT_CloudIndexStdMapT<T, ValueT>::removeIfOrShiftIf can not be used");
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::shiftAll(const size_t &offset, const bool &negativeOffset)
{
    this->internalShiftAll(offset, negativeOffset);
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                                               const size_t &offset, const bool &negativeOffset)
{
    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    typename std::map<ct_index_type,ValueT >::iterator l = m_collection->end();
    std::advance(f, eraseBeginPos+eraseSize);

    std::map<ct_index_type,ValueT > tmp;

    if(negativeOffset)
    {
        while(f != l)
        {
            tmp[ct_index_type(f->first - offset)] = f->second;
            ++f;
        }
    }
    else
    {
        while(f != l)
        {
            tmp[ct_index_type(f->first + offset)] = f->second;
            ++f;
        }
    }

    erase(eraseBeginPos, size()-eraseBeginPos);
    m_collection->insert(tmp.begin(), tmp.end());
}

template<typename T, typename ValueT>
CT_SharedPointer< std::vector<int> > CT_CloudIndexStdMapT<T, ValueT>::toStdVectorInt() const
{
    CT_SharedPointer< std::vector<int> > indices(new std::vector<int>(size()));

    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    typename std::map<ct_index_type,ValueT >::iterator l = m_collection->end();

    size_t i = 0;

    while(f != l)
    {
        (*indices.get())[i] = f->first;
        ++f;
        ++i;
    }

    return indices;
}

template<typename T, typename ValueT>
CT_AbstractCloud* CT_CloudIndexStdMapT<T, ValueT>::copy() const
{
    CT_CloudIndexStdMapT<T, ValueT> *index = new CT_CloudIndexStdMapT<T, ValueT>();

    index->m_collection->insert(m_collection->begin(), m_collection->end());

    return index;
}

template<typename T, typename ValueT>
template<typename S>
typename std::map<S, ValueT>::iterator CT_CloudIndexStdMapT<T, ValueT>::mapFindIf(typename CT_CloudIndexStdMapT<T, ValueT>::FindIfFunction findIf, void *context) const
{
    typename std::map<S, ValueT>::iterator f = m_collection->begin();
    typename std::map<S, ValueT>::iterator l = m_collection->end();

    size_t tmp;

    while (f!=l)
    {
        tmp = f->first;

        if ((*findIf)(context, tmp))
            return f;

        ++f;
    }

    return l;
}

template<typename T, typename ValueT>
std::map< ct_index_type, ValueT >* CT_CloudIndexStdMapT<T, ValueT>::internalData() const
{
    return m_collection.data();
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::internalShiftAll(const size_t &offset, const bool &negativeOffset)
{
    typename std::map<ct_index_type,ValueT >::iterator f = m_collection->begin();
    typename std::map<ct_index_type,ValueT >::iterator l = m_collection->end();

    std::map<ct_index_type,ValueT > tmp;

    if(negativeOffset)
    {
        while(f != l)
        {
            tmp[ct_index_type(f->first - offset)] = f->second;
            ++f;
        }
    }
    else
    {
        while(f != l)
        {
            tmp[ct_index_type(f->first + offset)] = f->second;
            ++f;
        }
    }

    internalClear();
    m_collection->insert(tmp.begin(), tmp.end());
}

template<typename T, typename ValueT>
void CT_CloudIndexStdMapT<T, ValueT>::internalClear()
{
    m_collection->clear();
}
