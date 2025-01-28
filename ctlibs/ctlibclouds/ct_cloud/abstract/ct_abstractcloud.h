#ifndef CT_ABSTRACTCLOUD_H
#define CT_ABSTRACTCLOUD_H

#include "ctlibclouds_global.h"
#include <QtOpenGL/QGL>

/**
 * @brief A cloud
 */
class CTLIBCLOUDS_EXPORT CT_AbstractCloud
{
public:
    CT_AbstractCloud();
    virtual ~CT_AbstractCloud() {}

    /**
     * @brief return the size of the cloud
     */
    virtual size_t size() const = 0;

    /**
     * @brief return the memory used by this cloud
     */
    virtual size_t memoryUsed() const = 0;

    /**
     * @brief return a copy
     */
    virtual CT_AbstractCloud* copy() const = 0;

protected:

    friend class CT_AbstractCloudSyncToGlobalCloudManager;
    template<typename T, typename CLOUD> friend class CT_GlobalCloudManagerT;

    /**
     * @brief delete all elements of the cloud between beginIndex and (beginIndex+size-1) included. (Called by CT_AbstractCloudSyncToGlobalCloudManager)
     */
    virtual void erase(const size_t &beginIndex, const size_t &sizes) = 0;

    /**
     * @brief resize the collection (Called by CT_GlobalCloudManagerT and CT_AbstractCloudSyncToGlobalCloudManager)
     */
    virtual void resize(const size_t &newSize) = 0;

    /**
     * @brief reserve the collection
     */
    virtual void reserve(const size_t &newSize) = 0;
};

#endif // CT_ABSTRACTCLOUD_H
