#include "ct_cloud/ct_standardcloudstdvectort.h"

template<typename T>
CT_StandardCloudStdVectorT<T>::CT_StandardCloudStdVectorT(size_t size) : CT_AbstractCloudT<T>()
{
    if(size > 0)
        m_collection.resize(size);
}

template<typename T>
typename CT_StandardCloudStdVectorT<T>::Iterator CT_StandardCloudStdVectorT<T>::begin()
{
    return m_collection.begin();
}

template<typename T>
typename CT_StandardCloudStdVectorT<T>::Const_Iterator CT_StandardCloudStdVectorT<T>::constBegin() const
{
    return m_collection.begin();
}

template<typename T>
typename CT_StandardCloudStdVectorT<T>::Iterator CT_StandardCloudStdVectorT<T>::end()
{
    return m_collection.end();
}

template<typename T>
typename CT_StandardCloudStdVectorT<T>::Const_Iterator CT_StandardCloudStdVectorT<T>::constEnd() const
{
    return m_collection.end();
}

template<typename T>
size_t CT_StandardCloudStdVectorT<T>::size() const
{
    return m_collection.size();
}

template<typename T>
size_t CT_StandardCloudStdVectorT<T>::memoryUsed() const
{
    return size() * sizeof(T);
}

template<typename T>
T& CT_StandardCloudStdVectorT<T>::operator[](const size_t &index)
{
    return pTAt(index);
}

template<typename T>
const T& CT_StandardCloudStdVectorT<T>::operator[](const size_t &index) const
{
    return pTAt(index);
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::erase(const size_t &beginIndex, const size_t &sizes)
{
    if(sizes == 0)
        return;

    const size_t endIndex = std::min(beginIndex + sizes, size());
    const size_t cpySize = size() - endIndex;

    if(cpySize > 0)
    {
        T *data = m_collection.data();

        T *dst = data+beginIndex;
        T *src = data+endIndex;

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
        memcpy(dst, src, sizeof(T)*cpySize);
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
    }

    resize(size() - std::min(sizes, size()));
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::resize(const size_t &newSize)
{
    m_collection.resize(newSize, T());
    m_collection.shrink_to_fit();
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::reserve(const size_t &newSize)
{
    m_collection.reserve(newSize);
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::internalCopyData(const size_t &destIndex, const size_t &srcIndex, const size_t &size)
{
    T *data = &pTAt(0);

    T *dst = data+destIndex;
    T *src = data+srcIndex;

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(dst, src, sizeof(T)*size);
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::copyDataFromTo(T* src, const size_t &destIndex, const size_t &size)
{
    T *data = &pTAt(0);
    T *dst = data+destIndex;

#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
    memcpy(dst, src, sizeof(T)*size);
#if defined(__GNUC__) && !defined(__INTEL_COMPILER) && (__GNUC__ >= 8)
#pragma GCC diagnostic pop
#endif
}

template<typename T>
CT_AbstractCloud* CT_StandardCloudStdVectorT<T>::copy() const
{
    size_t s = size();

    CT_StandardCloudStdVectorT<T> *cloud = new CT_StandardCloudStdVectorT<T>(s);

    for(size_t i=0; i<s; ++i)
        (*cloud)[i] = m_collection[i];

    return cloud;
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::addT(const T &val)
{
    m_collection.push_back(val);
}

template<typename T>
T& CT_StandardCloudStdVectorT<T>::addT()
{
    T tmp;
    addT(tmp);
    return *(m_collection.end()-1);
}

template<typename T>
void CT_StandardCloudStdVectorT<T>::replaceT(const size_t &index, T &newVal)
{
    m_collection[index] = newVal;
}
