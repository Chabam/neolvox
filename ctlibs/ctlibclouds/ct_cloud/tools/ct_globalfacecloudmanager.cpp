#include "ct_globalfacecloudmanager.h"
#include "ct_mesh/ct_edge.h"

CT_GlobalFaceCloudManager::CT_GlobalFaceCloudManager(const CT_AbstractGlobalEdgeCloudManager *globalEdgeCloudManager) : CT_GlobalCloudManagerT< CT_Face, CT_FaceCloudStdVector >()
{
    // add a face to begin index to 1 (invalid index of a face is 0)
    this->m_cloud.addT(CT_Face());

    m_gecm = (CT_AbstractGlobalEdgeCloudManager*)globalEdgeCloudManager;
    m_gecm->addGlobalCloudListener(this);
}

void CT_GlobalFaceCloudManager::cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size)
{
    if(size > 0)
    {
        // this method is called when a list of edge has been removed from the global cloud of edge

        // we will remove edge of all faces that contains a edge that have been removed
        // and shift them that is after the list of edge deleted
        CT_FaceCloudStdVector *cloud = globalCloud();

        size_t currentIndex = 0;

        CT_FaceCloudStdVector::Iterator begin = cloud->begin()+currentIndex;
        CT_FaceCloudStdVector::Iterator end = cloud->end();

        if(begin != end)
        {
            size_t beginIndexToIgnore = 0;
            size_t endIndexToIgnore = 0;

            size_t edgeIndex;
            size_t endIndex = beginIndex+size-1;

            CT_FaceCloudStdVector::Iterator beginOrig = begin;
            CT_FaceCloudStdVector::Iterator nextEnd = end;

            while(begin != nextEnd)
            {
                if(this->nextIndexToIgnoreToSync(currentIndex, beginIndexToIgnore, endIndexToIgnore))
                {
                    nextEnd = beginOrig + beginIndexToIgnore;

                    if(nextEnd > end)
                        nextEnd = end;
                }

                while(begin != nextEnd)
                {
                    CT_Face &face = (*begin);

                    edgeIndex = face.iEdge();

                    if(edgeIndex >= beginIndex)
                    {
                        if(edgeIndex <= endIndex)
                        {
                            face.setEdge((size_t)0);
                        }
                        else
                        {
                            face.setEdge(edgeIndex - size);
                        }
                    }

                    ++begin;
                }

                if(nextEnd < end)
                {
                    currentIndex = endIndexToIgnore+1;
                    begin = beginOrig + currentIndex;

                    if(begin > end)
                        begin = end;

                    nextEnd = end;
                }
            }
        }
    }
}

void CT_GlobalFaceCloudManager::cloudAdded(const size_t &size)
{
    // do nothing
    Q_UNUSED(size)
}
