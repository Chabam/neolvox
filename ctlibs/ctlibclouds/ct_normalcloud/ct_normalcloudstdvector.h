#ifndef CT_NORMALCLOUDSTDVECTOR_H
#define CT_NORMALCLOUDSTDVECTOR_H

#include "ct_normalcloud/abstract/ct_abstractnormalcloud.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"

/**
 * A cloud of normals that use a std::vector
 */
class CTLIBCLOUDS_EXPORT CT_NormalCloudStdVector : public CT_StandardCloudStdVectorT<CT_Normal>, public CT_AbstractNormalCloud
{
public:
    CT_NormalCloudStdVector(size_t size = 0);

    /**
     * @brief Returns the number of normals
     */
    size_t size() const;

    /**
     * @brief Returns the memory used in bytes for all normals in this object
     */
    size_t memoryUsed() const;

    /**
     * @brief Add the normal passed in parameter to the cloud
     */
    void addNormal(const CT_Normal &normal);

    /**
     * @brief Add a default normal to the cloud and return a reference to it
     */
    CT_Normal& addNormal();

    /**
     * @brief Returns a reference to the normal at index 'index'
     */
    CT_Normal& normalAt(const size_t &index);

    /**
     * @brief Returns a const reference to the normal at index 'index'
     */
    const CT_Normal& constNormalAt(const size_t &index) const;

    /**
     * @brief Returns a reference to the normal at index 'index'
     */
    CT_Normal& operator[](const size_t &index);

    /**
     * @brief Returns a const reference to the normal at index 'index'
     */
    const CT_Normal& operator[](const size_t &index) const;

    /**
     * @brief Replace a normal at index specified
     */
    void replaceNormal(const size_t &index, CT_Normal &normal);

    /**
     * @brief Returns a copy of this cloud
     */
    CT_AbstractCloud* copy() const;

protected:
    /**
     * @brief delete all elements of the cloud between beginIndex and (beginIndex+size-1) included.
     */
    void erase(const size_t &beginIndex, const size_t &sizes);

    /**
     * @brief resize the collection
     */
    void resize(const size_t &newSize);

    /**
     * @brief reserve the collection
     */
    void reserve(const size_t &newSize);
};

#endif // CT_NORMALCLOUDSTDVECTOR_H
