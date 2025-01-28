#include "ct_cloudindex/tools/ct_cloudindexstdvectortmethodimpl.h"

template<typename T>
CT_CloudIndexStdVectorTMethodImpl<T>::CT_CloudIndexStdVectorTMethodImpl(const CT_AbstractCloudIndex &ci, std::vector<T> &vector) : _vector(vector), m_ci(ci)
{
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::setSortType(CT_AbstractCloudIndex::SortType type)
{
    if(m_ci.sortType() != type)
    {
        if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
            std::sort(_vector.begin(), _vector.end());
    }
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::size() const
{
    return _vector.size();
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::indexAt(const size_t &i) const
{
    return _vector[i];
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::operator[](const size_t &i) const
{
    return _vector[i];
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::indexAt(const size_t &i, size_t &index) const
{
    index = _vector[i];
}

template<typename T>
const T& CT_CloudIndexStdVectorTMethodImpl<T>::constIndexAt(const size_t &i) const
{
    return _vector[i];
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::first() const
{
    return _vector.front();
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::last() const
{
    return _vector.back();
}

template<typename T>
bool CT_CloudIndexStdVectorTMethodImpl<T>::contains(const size_t &index) const
{
    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        return std::binary_search(_vector.begin(), _vector.end(), index);

    return (std::find(_vector.begin(), _vector.end(), index) != _vector.end());
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::indexOf(const size_t &index) const
{
    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
    {
        typename std::vector<T>::iterator first = _vector.begin();
        typename std::vector<T>::iterator last = _vector.end();

        first = std::lower_bound(first, last, index);

        if(first!=last && !(static_cast<int>(index)<(*first)))
            return std::distance(_vector.begin(), first);
    }
    else
    {
        typename std::vector<T>::const_iterator it = std::find(_vector.begin(), _vector.end(), index);

        if(it != _vector.end())
            return std::distance(_vector.cbegin(), it);
    }

    return size();
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::lowerBound(const size_t &value) const
{
    typename std::vector<T>::iterator it;
    typename std::vector<T>::iterator itEnd = _vector.end();

    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        it = std::lower_bound(_vector.begin(), _vector.end(), value);
    else
        it = std::find_if(_vector.begin(), _vector.end(), std::bind(std::greater_equal<T>(), std::placeholders::_1, T(value)));

    return size() - (itEnd-it);
}

template<typename T>
size_t CT_CloudIndexStdVectorTMethodImpl<T>::upperBound(const size_t &value) const
{
    typename std::vector<T>::iterator it;
    typename std::vector<T>::iterator itEnd = _vector.end();

    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        it = std::upper_bound(_vector.begin(), _vector.end(), value);
    else
        it = std::find_if(_vector.begin(), _vector.end(), std::bind(std::greater<T>(), std::placeholders::_1, T(value)));

    return size() - (itEnd-it);
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::addIndex(const size_t &newIndex)
{
    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
        _vector.insert(std::lower_bound(_vector.begin(), _vector.end(), T(newIndex)), T(newIndex));
    else
        _vector.push_back(T(newIndex));
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::removeIndex(const size_t &index)
{
    if(m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder)
    {
        typename std::vector<T>::iterator first = _vector.begin();
        typename std::vector<T>::iterator last = _vector.end();

        first = std::lower_bound(first, last, index);

        if(first!=last && !(static_cast<int>(index)<(*first)))
            _vector.erase(first);
    }
    else
    {
        _vector.erase(std::find(_vector.begin(), _vector.end(), index));
    }
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::replaceIndex(const size_t &i, const T &newIndex, const bool &verifyRespectSort)
{
    _vector[i] = newIndex;

    if(verifyRespectSort
            && (m_ci.sortType() == CT_AbstractCloudIndex::SortedInAscendingOrder))
    {
        bool ok = false;

        if(i>0)
        {
            if(_vector.at(i-1) <= newIndex)
                ok = true;
        }

        if(ok && i<(size()-1))
        {
            if(_vector.at(i+1) >= newIndex)
                ok = true;
            else
                ok = false;
        }

        if(!ok)
            this->setSortType(CT_AbstractCloudIndex::NotSorted);
    }
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::push_front(const size_t &newIndex)
{
    CT_AbstractCloudIndex::SortType t = m_ci.sortType();

    if(t != CT_AbstractCloudIndex::NotSorted)
    {
        if(size() > 0)
        {
            if(newIndex > first())
                t = CT_AbstractCloudIndex::NotSorted;
        }
    }

    _vector.insert(_vector.begin(), T(newIndex));
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::fill()
{
    size_t s = size();

    for(size_t i=0; i<s; ++i)
        _vector[i] = T(i);
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::clear()
{
    internalClear();
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::erase(const size_t &beginIndex, const size_t &sizes)
{
    if(sizes == 0)
        return;

    const size_t endIndex = std::min(beginIndex + sizes, size());
    const size_t cpySize = size() - endIndex;

    if(cpySize > 0)
    {
        T *data = _vector.data();

        T *dst = data+beginIndex;
        T *src = data+endIndex;

        memcpy(dst, src, sizeof(T)*cpySize);
    }

    resize(size() - std::min(sizes, size()));
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::resize(const size_t &newSize)
{
    _vector.resize(newSize, 0);
    _vector.shrink_to_fit();
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::reserve(const size_t &newSize)
{
    _vector.reserve(newSize);
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::removeIfOrShiftIf(typename CT_CloudIndexStdVectorTMethodImpl<T>::FindIfFunction findIf,
                                                             typename CT_CloudIndexStdVectorTMethodImpl<T>::RemoveIfFunction removeIf,
                                                             typename CT_CloudIndexStdVectorTMethodImpl<T>::ShiftIfFunction shiftIf,
                                                             const size_t &shiftValue,
                                                             const bool &negativeShift,
                                                             void *context)
{
    typename std::vector<T>::iterator first = vectorFindIf(findIf, context);
    typename std::vector<T>::iterator last = _vector.end();

    if(first != last)
    {
        typename std::vector<T>::iterator it = first;

        T nI;

        while(it != last)
        {
            nI = *it;

            if((*shiftIf)(context, nI))
            {
                if(negativeShift)
                    *first = T(nI-shiftValue);
                else
                    *first = T(nI+shiftValue);

                ++first;
            }
            else if(!(*removeIf)(context, nI))
            {
                *first = nI;
                ++first;
            }

            ++it;
        }

        _vector.erase(first, last);
    }
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::shiftAll(const size_t &offset, const bool &negativeOffset)
{
    this->internalShiftAll(offset, negativeOffset);
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::eraseBetweenAndShiftRest(const size_t &eraseBeginPos, const size_t &eraseSize,
                                                          const size_t &offset, const bool &negativeOffset)
{
    const size_t endOfCollection = size();
    size_t currentIndexOfValueToReplace = eraseBeginPos;
    size_t currentIndexOfValueToShift = eraseBeginPos + eraseSize;

    // we copy the shift part to the erase part and shift the index in the same time
    if(negativeOffset)
    {
        while(currentIndexOfValueToShift < endOfCollection)
        {
            _vector.at(currentIndexOfValueToReplace) = _vector.at(currentIndexOfValueToShift) - T(offset);

            ++currentIndexOfValueToShift;
            ++currentIndexOfValueToReplace;
        }
    }
    else
    {
        while(currentIndexOfValueToShift < endOfCollection)
        {
            _vector.at(currentIndexOfValueToReplace) = _vector.at(currentIndexOfValueToShift) + T(offset);

            ++currentIndexOfValueToShift;
            ++currentIndexOfValueToReplace;
        }
    }

    // we erase the end of the cloud
    if(currentIndexOfValueToReplace < endOfCollection)
        erase(currentIndexOfValueToReplace, endOfCollection-currentIndexOfValueToReplace);
}

template<typename T>
typename std::vector<T>::iterator CT_CloudIndexStdVectorTMethodImpl<T>::vectorFindIf(typename CT_CloudIndexStdVectorTMethodImpl<T>::FindIfFunction findIf,
                                                                                     void *context) const
{
    typename std::vector<T>::iterator first = _vector.begin();
    typename std::vector<T>::iterator last = _vector.end();

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
void CT_CloudIndexStdVectorTMethodImpl<T>::internalShiftAll(const size_t &offset, const bool &negativeOffset)
{
    typename std::vector<T>::iterator first = _vector.begin();
    typename std::vector<T>::iterator last = _vector.end();

    if(negativeOffset)
    {
        while(first != last)
        {
            *first -= T(offset);
            ++first;
        }
    }
    else
    {
        while(first != last)
        {
            *first += T(offset);
            ++first;
        }
    }
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::internalClear()
{
    _vector.clear();
}

template<typename T>
void CT_CloudIndexStdVectorTMethodImpl<T>::copy(std::vector<T> &destination)
{
    typename std::vector<T>::const_iterator it = _vector.begin();
    typename std::vector<T>::const_iterator end = _vector.begin();
    typename std::vector<T>::iterator itDest = destination.begin();

    while(it != end) {
        (*itDest) = (*it);
        ++it;
        ++itDest;
    }
}
