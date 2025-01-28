#ifndef CT_ABSTRACTNORMALCLOUD_H
#define CT_ABSTRACTNORMALCLOUD_H

#include "ct_cloud/abstract/ct_abstractcloud.h"
#include "ct_normal.h"

/**
 * A cloud of normals
 */
class CTLIBCLOUDS_EXPORT CT_AbstractNormalCloud : virtual public CT_AbstractCloud
{
public:
    CT_AbstractNormalCloud();
    virtual ~CT_AbstractNormalCloud();

    /**
     * @brief Returns the number of normals
     */
    virtual size_t size() const = 0;

    /**
     * @brief Returns a reference to the normal at index 'index'
     */
    virtual CT_Normal& normalAt(const size_t &index) = 0;

    /**
     * @brief Returns a const reference to the normal at index 'index'
     */
    virtual const CT_Normal& constNormalAt(const size_t &index) const = 0;

    /**
     * @brief Returns a reference to the normal at index 'index'
     */
    virtual CT_Normal& operator[](const size_t &index) = 0;

    /**
     * @brief Returns a const reference to the normal at index 'index'
     */
    virtual const CT_Normal& operator[](const size_t &index) const = 0;

    /**
     * @brief Replace a normal at index specified
     */
    virtual void replaceNormal(const size_t &index, CT_Normal &normal) = 0;
};

#endif // CT_ABSTRACTNORMALCLOUD_H
