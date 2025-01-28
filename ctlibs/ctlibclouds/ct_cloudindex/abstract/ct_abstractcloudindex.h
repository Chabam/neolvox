#ifndef CT_ABSTRACTCLOUDINDEX_H
#define CT_ABSTRACTCLOUDINDEX_H

#include "ct_cloud/abstract/ct_abstractcloud.h"
#include "ct_tools/ct_sharedpointer.h"
#include <vector>

// index of cloud index can be only int for two reason ;
//  - the maximum points (if a cloud index link to points) with a "int" type can take 24Go in memory. I think it wil be sufficient.
//  - we can shared the vector to pcl easily and without create a new
typedef int                 ct_index_type;

// if we really want to use a size_t we can by uncomment this line and comment the other.
//typedef size_t              ct_index_type;

/**
 * A cloud of index of type ct_index_type
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCloudIndex : public CT_AbstractCloud
{
public:
    typedef ct_index_type IndexType;

    enum SortType
    {
        SortedInAscendingOrder,
        NotSorted
    };

    CT_AbstractCloudIndex();
    virtual ~CT_AbstractCloudIndex() {}

    /**
     * @brief return the sort type
     */
    SortType sortType() const;

    /**
     * @brief Returns true if the first and next index until end follow each other
     */
    virtual bool doIndexesFollowEachOther() const { return false; }

    /**
      * \brief return the index in the cloud at index 'i'.
      *
      *        You can probably have better performance (speed) if you use
      *        the method indexAt(const size_t &i, size_t &index)
      */
    virtual size_t indexAt(const size_t &i) const = 0;

    /**
      * \brief return the index in the cloud at index 'i'.
      *
      *        You can probably have better performance (speed) if you use
      *        the method indexAt(const size_t &i, size_t &index)
      */
    //virtual size_t operator[](const size_t &i) const = 0;

    /**
      * \brief return the index in the cloud at index 'i'.
      *
      * \param i :the location of the index you want
      * \param index : the index (out parameter)
      */
    virtual void indexAt(const size_t &i, size_t &index) const = 0;

    /**
     * @brief Returns a const reference of the index at the location 'i'
     */
    virtual const ct_index_type& constIndexAt(const size_t &i) const = 0;
    const ct_index_type& operator[](const size_t &i) const { return constIndexAt(i); }

    /**
     * @brief return the first index of the cloud index
     * @warning undefined if the cloud is empty !
     */
    virtual size_t first() const = 0;

    /**
     * @brief return the last index of the cloud index
     * @warning undefined if the cloud is empty !
     */
    virtual size_t last() const = 0;

    /**
     * @brief return true if the cloud index contains the index passed in parameter
     * @param index : index to search
     * @return true if the cloud contains at least one time the index passed in parameter
     */
    virtual bool contains(const size_t &index) const = 0;

    /**
     * @brief Return the index (local) of the index (global) passed in parameter
     */
    virtual size_t indexOf(const size_t &index) const = 0;

    /**
     * @brief return the location of the first element that is greater or egual the value passed as a parameter
     * @param value : value to search
     */
    virtual size_t lowerBound(const size_t &value) const = 0;

    /**
     * @brief return the location of the first member is greater than the value passed as parameter
     * @param value : value to search
     */
    virtual size_t upperBound(const size_t &value) const = 0;

    /**
     * @brief Return a special shared pointer to a std::vector<int> that contains all indexes of this cloud. The vector
     *        can be created (because the cloud use another type of container) or it can be shared.
     */
    virtual CT_SharedPointer< std::vector<int> > toStdVectorInt() const = 0;

    /**
     * @brief Returns true if it must be unregistered when it was empty (true by default)
     */
    bool mustBeUnregisteredWhenIsEmpty() const;

private:
    SortType        m_sortType;
    bool            m_unregisterWhenEmpty;

protected:

    void internalSetSortType(SortType st);

    void internalSetUnregisteredWhenIsEmpty(bool enable);
};

#endif // CT_ABSTRACTCLOUDINDEX_H
