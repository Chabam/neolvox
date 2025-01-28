#ifndef CT_GLOBALEDGECLOUDMANAGER_H
#define CT_GLOBALEDGECLOUDMANAGER_H

#include "ct_defines.h"
#include "ct_cloud/tools/ct_globalcloudmanagert.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"
#include "ct_mesh/ct_edge.h"

/**
 * Class who manage globals cloud of edge. The manager create a CT_EdgeCloudStdVector that have
 * a collection of CT_Edge.
 *
 * It's an utility class who manage a global cloud. It will inform the CT_AbstractCloudIndexRegistrationManagerT and all
 * CT_AbstractCloudSyncToGlobalCloudManagerT when edge of the cloud is deleted or added. So we can sync index and
 * other cloud with this global cloud.
 *
 * It will also sync pointer in CT_Edge.
 */
class CTLIBCLOUDS_EXPORT CT_GlobalEdgeCloudManager : public CT_GlobalCloudManagerT< CT_Edge, CT_EdgeCloudStdVector >
{
public:
    CT_GlobalEdgeCloudManager();

    void setGlobalCloudManager(const CT_AbstractGlobalPointCloudManager *globalPointCloudManager);
    void setGlobalCloudManager(const CT_AbstractGlobalFaceCloudManager *globalFaceCloudManager);

private:
    // sync edge when edge is deleted
    class SyncEdgeE : public IGlobalCloudListener
    {
    public:
        CT_GlobalEdgeCloudManager *m_manager;

    protected:
        void cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size);
        void cloudAdded(const size_t &size) { Q_UNUSED(size) }
    };

    // sync edge when face is deleted
    class SyncEdgeF : public IGlobalCloudListener
    {
    public:
        CT_GlobalEdgeCloudManager *m_manager;

        void cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size);
        void cloudAdded(const size_t &size) { Q_UNUSED(size) }
    };

    // sync edge when points is deleted
    class SyncEdgeP : public IGlobalCloudListener
    {
    public:
        CT_GlobalEdgeCloudManager *m_manager;

        void cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size);
        void cloudAdded(const size_t &size) { Q_UNUSED(size) }
    };


    CT_AbstractGlobalPointCloudManager          *m_gpcm;
    CT_AbstractGlobalFaceCloudManager           *m_gfcm;
    SyncEdgeE                                   m_syncEdgeE;
    SyncEdgeF                                   m_syncEdgeF;
    SyncEdgeP                                   m_syncEdgeP;
};

#endif // CT_GLOBALEDGECLOUDMANAGER_H
