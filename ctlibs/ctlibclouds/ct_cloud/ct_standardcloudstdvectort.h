#ifndef CT_STANDARDCLOUDSTDVECTORT_H
#define CT_STANDARDCLOUDSTDVECTORT_H

#include "ct_cloud/abstract/ct_abstractcloudt.h"
#include <vector>

/**
 * A cloud of T (point, face, etc..) that use a std::vector.
 */
template<typename T>
class CT_StandardCloudStdVectorT : virtual public CT_AbstractCloudT<T>
{
public:
    typedef typename std::vector<T>::iterator        Iterator;
    typedef typename std::vector<T>::const_iterator  Const_Iterator;

    CT_StandardCloudStdVectorT(size_t size = 0);

    Iterator begin();
    Const_Iterator constBegin() const;

    Iterator end();
    Const_Iterator constEnd() const;

    size_t size() const;

    size_t memoryUsed() const;

    T& operator[](const size_t &index);
    const T& operator[](const size_t &index) const;

    void addT(const T &val);
    T& addT();

    void replaceT(const size_t &index, T &newVal);

    /**
     * @brief Copy data of other source
     * @warning No verification is defined for the validity of size and destIndex
     */
    void copyDataFromTo(T* src, const size_t &destIndex, const size_t &size);

    virtual CT_AbstractCloud* copy() const;

protected:

    std::vector<T>   m_collection;

    template<typename A, typename CLOUD> friend class CT_GlobalCloudManagerT;
    template<typename X> friend class CT_UndefinedPointCloudSizeDenseAttributeSetter;

    /**
     * @brief delete all elements of the cloud between beginIndex and (beginIndex+size-1) included.
     */
    void erase(const size_t &beginIndex, const size_t &sizes);

    /**
     * @brief resize the collection
     */
    void resize(const size_t &newSize);

    /**
     * @brief resize the collection
     */
    void reserve(const size_t &newSize);

    /**
     * @brief copy data
     */
    void internalCopyData(const size_t &destIndex, const size_t &srcIndex, const size_t &size);

    inline const T& pTAt(const size_t &index) const
    {
        return m_collection[index];
    }

    inline T& pTAt(const size_t &index)
    {
        return m_collection[index];
    }
};

// specialization for vector of type bool because he use not bool but bit !!!!
template<>
class CT_StandardCloudStdVectorT<bool> : virtual public CT_AbstractCloudT<bool>
{

protected:
    std::vector<qint8>                              m_collection;
};

#include "ct_cloud/ct_standardcloudstdvectort.hpp"

#endif // CT_STANDARDCLOUDSTDVECTORT_H
