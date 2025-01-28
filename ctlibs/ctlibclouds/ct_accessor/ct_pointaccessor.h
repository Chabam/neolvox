#ifndef CT_POINTACCESSOR_H
#define CT_POINTACCESSOR_H

#include "ct_defines.h"
#include "ct_point.h"

/**
 * @brief Use this class to access point in the global point cloud
 */
class CTLIBCLOUDS_EXPORT CT_PointAccessor
{
public:
    CT_PointAccessor();

    /**
     * @brief Returns the size of the global point cloud
     */
    size_t size() const;

    /**
     * @brief Return the point at 'globalIndex'
     */
    CT_Point pointAt(const size_t &globalIndex) const;

    /**
     * @brief Get the point at 'globalIndex' with the use of your reference
     */
    void pointAt(const size_t &globalIndex, CT_Point &point) const;

    /**
     * @brief Return a const reference to the point at 'globalIndex'
     */
    const CT_Point& constPointAt(const size_t &globalIndex) const;

    /**
     * @brief Replace the point at 'globalIndex' by a new point
     */
    void replacePointAt(const size_t &globalIndex, const CT_Point &p);


    // INTERNAL POINT

    /**
     * @brief Return a reference to the point at 'globalIndex' (internal point used in cloud)
     * @warning Not intended for direct use by plugin developper
     */
    CT_PointData& internalPointAt(const size_t &globalIndex);

    /**
     * @brief Return a reference to the point at 'globalIndex' (internal point used in cloud)
     * @warning Not intended for direct use by plugin developper
     */
    const CT_PointData& constInternalPointAt(const size_t &globalIndex) const;

    /**
     * @brief Replace the current point by a new point
     * @warning Not intended for direct use by plugin developper
     */
    void replaceInternalPointAt(const size_t &globalIndex, const CT_PointData &p);

    // CLOUD

    /**
     * @brief Return the cloud used by this accessor
     */
    CT_AbstractPointCloud *cloud() const;

private:
    mutable CT_Point        m_p;
    CT_AbstractPointCloud   *m_pCloud;
};

#endif // CT_POINTACCESSOR_H
