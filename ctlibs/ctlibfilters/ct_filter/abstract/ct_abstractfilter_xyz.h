#ifndef CT_ABSTRACTFILTER_XYZ_H
#define CT_ABSTRACTFILTER_XYZ_H

#include "ctlibfilters_global.h"

// from pluginshared
#include "ct_filter/abstract/ct_abstractfilter.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"
#include "ct_pointcloudindex/ct_pointcloudindexvector.h"

/**
 * @brief A filter XYZ just filter points of a point cloud (it has a point cloud in input and output a filtered point cloud)
 */
class CTLIBFILTERS_EXPORT CT_AbstractFilter_XYZ : public CT_AbstractFilter
{
    Q_OBJECT
public:
    CT_AbstractFilter_XYZ(QString pluginName);
    CT_AbstractFilter_XYZ(const CT_AbstractFilter_XYZ &other);
    ~CT_AbstractFilter_XYZ();

    /**
     * @brief Set the input point cloud
     * @param inItem : a item drawable with a point cloud
     * @return true if the itemdrawable has a point cloud
     */
    bool setPointCloud(const CT_AbstractItemDrawableWithPointCloud* inItem);

    /**
     * @brief Set the input point cloud
     * @param ci : point cloud indexes
     * @return true if the input cloud is not nullptr
     */
    bool setPointCloud(const CT_AbstractPointCloudIndex* ci);

    /**
     * @brief Update the filter before use the method "filterPointCloudIndex" or "validatePoint". By default
     *        just update the bounding box
     */
    virtual void updateAll();

    /**
     * @brief Call to filter entire input cloud. Call "updateAll" before this method.
     * @return false if the input cloud was not set or another error is occured
     */
    virtual bool filterPointCloudIndex();

    /**
     * @brief Return the output cloud generated in method "filterPointCloudIndex".
     */
    CT_PointCloudIndexVector* outputPointCloudIndex() const;

    /**
     * @brief Take the output cloud generated in method "filterPointCloudIndex". You will be the owner of the cloud and must manage is life in memory.
     */
    CT_PointCloudIndexVector* takeOutputCloudIndex();

    /**
     * @brief Call to check if a point is valid (not filtered) or not (filtered). Call "updateAll" before this method.
     * @param pointIt : the iterator that was on the current point to check
     * @return true if the point must be kept (not filtered)
     */
    virtual bool validatePoint(const CT_PointIterator& pointIt) = 0;

private:
    CT_AbstractPointCloudIndex*         _inCloud;
    CT_PointCloudIndexVector*           _outCloud;
    Eigen::Vector3d                     m_min;
    Eigen::Vector3d                     m_max;
    bool                                m_minMaxUpdated;

protected:
    /**
     * @brief Returns true if min and max is updated
     */
    bool isMinAndMaxUpdated() const;

    /**
     * @brief Returns the minimum coordinate of the bbox
     */
    Eigen::Vector3d minBBox() const;

    /**
     * @brief Returns the maximum coordinate of the bbox
     */
    Eigen::Vector3d maxBBox() const;

    /**
     * @brief Call to update min and max if you want to use it
     * @param force : If "isMinAndMaxUpdated" return true the method will do nothing, set "force" to true if you want to force the update.
     * @return false if input cloud is nullptr
     */
    bool updateMinMax(bool force = false);

    /**
     * @brief Return the input cloud
     */
    CT_AbstractPointCloudIndex* inputPointCloudIndex() const;
};

#endif // CT_ABSTRACTFILTER_XYZ_H
