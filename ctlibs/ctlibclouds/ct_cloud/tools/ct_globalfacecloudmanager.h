#ifndef CT_GLOBALFACECLOUDMANAGER_H
#define CT_GLOBALFACECLOUDMANAGER_H

#include "ct_cloud/tools/ct_globalcloudmanagert.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"
#include "ct_mesh/ct_face.h"

/**
 * Class who manage globals cloud of face. The manager create a CT_FaceCloudStdVector that have
 * a collection of CT_Face.
 *
 * It's an utility class who manage a global cloud. It will inform the CT_AbstractCloudIndexRegistrationManagerT and all
 * CT_AbstractCloudSyncToGlobalCloudManagerT when face of the cloud is deleted or added. So we can sync index and
 * other cloud with this global cloud.
 *
 * It will also sync pointer in CT_Face.
 */
class CTLIBCLOUDS_EXPORT CT_GlobalFaceCloudManager : public CT_GlobalCloudManagerT< CT_Face, CT_FaceCloudStdVector >, public IGlobalCloudListener
{
public:
    CT_GlobalFaceCloudManager(const CT_AbstractGlobalEdgeCloudManager *globalEdgeCloudManager);

private:
    CT_AbstractGlobalEdgeCloudManager *m_gecm;

protected:
    /**
     * @brief Called when the global cloud of edge is imputed of elements between beginIndex and endIndex (endIndex = beginIndex+size).
     */
    void cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size);

    /**
     * @brief Called when the size of the global cloud of edge has increased
     */
    void cloudAdded(const size_t &size);
};

#endif // CT_GLOBALFACECLOUDMANAGER_H
