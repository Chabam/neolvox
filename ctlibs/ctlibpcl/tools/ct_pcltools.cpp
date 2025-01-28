#include "ct_pcltools.h"

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_pointcloud/ct_internalpointcloud.h"

#if defined(_WIN32) && defined(_MSC_VER) // Microsoft Visual Studio Compiler
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__) // GNU Compiler (gcc,g++) for Linux, Unix, and MinGW (Windows)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#if __GNUC__ > 7
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#else
#pragma GCC diagnostic ignored "-Wattributes" // See: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=89325
#endif
#elif defined(__APPLE__) // Clang Compiler (Apple)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(_WIN32) && defined(_MSC_VER)
#elif (defined(__linux__) || defined(_WIN32)) && defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__APPLE__)
#pragma GCC diagnostic pop
#endif

CT_PCLTools::CT_PCLTools()
{
}

boost::shared_ptr< CT_PCLCloud > CT_PCLTools::staticConvertToPCLCloud(CT_PCIR pcir)
{
    return staticConvertToPCLCloud(pcir.data() == NULL ? NULL : pcir.data()->abstractCloudIndexT());
}

boost::shared_ptr< CT_PCLCloud > CT_PCLTools::staticConvertToPCLCloud(const CT_IAccessPointCloud *item)
{
    return staticConvertToPCLCloud(item->pointCloudIndex());
}

boost::shared_ptr< CT_PCLCloud > CT_PCLTools::staticConvertToPCLCloud(const CT_AbstractPointCloudIndex *pci)
{
    size_t size = 0;

    if(pci != NULL)
        size = pci->size();

    boost::shared_ptr< CT_PCLCloud > collection = boost::shared_ptr< CT_PCLCloud >(new CT_PCLCloud());
    collection->width = size;
    collection->height = 1;

    if(size > 0) {
        collection->points.resize(size);

        size_t i = 0;
        CT_PointIterator it(pci);

        while(it.hasNext()) {
            const CT_PointData &internalPoint = it.next().currentConstInternalPoint();;
            collection->points[i] = convertCTPointDataToPCL(internalPoint);
            ++i;
        }
    }

    return collection;
}

boost::shared_ptr<CT_PCLCloud> CT_PCLTools::staticConvertToPCLCloud(const CT_AbstractCloudT<CT_PointData> *ipc)
{
    size_t size = 0;

    if(ipc != NULL)
        size = ipc->size();

    boost::shared_ptr< CT_PCLCloud > collection = boost::shared_ptr< CT_PCLCloud >(new CT_PCLCloud());
    collection->width = size;
    collection->height = 1;

    if(size > 0) {
        collection->points.resize(size);

        size_t i = 0;

        while(i < size) {
            const CT_PointData &internalPoint = ipc->constTAt(i);
            collection->points[i] = convertCTPointDataToPCL(internalPoint);
            ++i;
        }
    }

    return collection;
}

boost::shared_ptr<CT_PCLCloudIndex> CT_PCLTools::staticConvertToPCLCloudIndex(CT_PCIR pcir)
{
    return staticConvertToPCLCloudIndex(pcir.data() == NULL ? NULL : pcir.data()->abstractCloudIndexT());
}

boost::shared_ptr<CT_PCLCloudIndex> CT_PCLTools::staticConvertToPCLCloudIndex(const CT_AbstractPointCloudIndex *pci)
{
    CT_SharedPointer< std::vector<int> > ptr = pci->toStdVectorInt();

    // if the shared pointer must delete the vector
    if(ptr.autoDelete())
        return boost::shared_ptr<CT_PCLCloudIndex>(ptr.take()); // we will take it and pass it to the boost shared pointer

    // otherwise we will pass it to the boost shared pointer and set a custom deleter that will not delete it
    return boost::shared_ptr<CT_PCLCloudIndex>(ptr.take(), &staticNoDelete< std::vector<int> >);
}

ct_index_type CT_PCLTools::staticPCLIndexToGlobalIndex(CT_PCIR pcir, const ct_index_type &index)
{
    return staticPCLIndexToGlobalIndex(pcir.data() == NULL ? NULL : pcir.data()->abstractCloudIndexT(), index);
}

ct_index_type CT_PCLTools::staticPCLIndexToGlobalIndex(const CT_IAccessPointCloud *item, const ct_index_type &index)
{
    return staticPCLIndexToGlobalIndex(item->pointCloudIndex(), index);
}

ct_index_type CT_PCLTools::staticPCLIndexToGlobalIndex(const CT_AbstractPointCloudIndex *pci, const ct_index_type &index)
{
    if((pci == NULL) || (pci->size() == 0))
        return 0;

    return pci->first()+index;
}

CT_PCLPoint CT_PCLTools::convertCTPointDataToPCL(const CT_PointData &data)
{
    return CT_PCLPoint(data[0], data[1], data[2]);
}
