#ifndef CT_PCLTOOLS_H
#define CT_PCLTOOLS_H

#include "ct_cloudindex/abstract/ct_abstractcloudindex.h"
#include "ct_point.h"
#include "ct_accessibility/ct_iaccesspointcloud.h"

#include "ctlibpcl/ctlibpcl_global.h"
#include "ctlibpcl/ct_pcldefines.h"

#include <boost/shared_ptr.hpp>

class CTLIBPCL_EXPORT CT_PCLTools
{
public:
    CT_PCLTools();

    /**
     * @brief Convert a cloud index to a pcl point cloud
     */
    static boost::shared_ptr< CT_PCLCloud > staticConvertToPCLCloud(CT_PCIR pcir);
    static boost::shared_ptr< CT_PCLCloud > staticConvertToPCLCloud(const CT_IAccessPointCloud *item);
    static boost::shared_ptr< CT_PCLCloud > staticConvertToPCLCloud(const CT_AbstractPointCloudIndex *pci);
    static boost::shared_ptr< CT_PCLCloud > staticConvertToPCLCloud(const CT_AbstractCloudT<CT_PointData> *ipc);

    /**
     * @brief Convert a cloud index to a pcl cloud index
     */
    static boost::shared_ptr< CT_PCLCloudIndex > staticConvertToPCLCloudIndex(CT_PCIR pcir);
    static boost::shared_ptr< CT_PCLCloudIndex > staticConvertToPCLCloudIndex(const CT_AbstractPointCloudIndex *pci);

    /**
     * @brief Convert a index from a pcl cloud to a global index for Computree
     * @param pcir / item / pci : the cloud index (or item) that you have passed to staticConvertToPCLCloud(...) OR the item to which you have called the method getPCLCloud()
     * @warning !!!! the index you want to convert must be issued from the original pcl cloud returned by the method staticConvertToPCLCloud(...). If the pcl cloud was reduced or modified
     * this method will returns a bad global index !!!
     */
    static ct_index_type staticPCLIndexToGlobalIndex(CT_PCIR pcir, const ct_index_type &index);
    static ct_index_type staticPCLIndexToGlobalIndex(const CT_IAccessPointCloud *item, const ct_index_type &index);
    static ct_index_type staticPCLIndexToGlobalIndex(const CT_AbstractPointCloudIndex *pci, const ct_index_type &index);

    /**
     * @brief Convert an internal point to a PCL point
     */
    static CT_PCLPoint convertCTPointDataToPCL(const CT_PointData &data);

private:

    template<typename T>
    static void staticNoDelete(T *d)
    {
        Q_UNUSED(d)

        // do nothing because we dont want to delete it
    }
};

#endif // CT_PCLTOOLS_H
