#include "ct_globaledgecloudmanager.h"

CT_GlobalEdgeCloudManager::CT_GlobalEdgeCloudManager() : CT_GlobalCloudManagerT< CT_Edge, CT_EdgeCloudStdVector >()
{
    // add a edge to begin index to 1 (invalid index of a face is 0)
    this->m_cloud.addT(CT_Edge());

    m_gpcm = nullptr;
    m_gfcm = nullptr;

    m_syncEdgeE.m_manager = this;
    m_syncEdgeF.m_manager = this;
    m_syncEdgeP.m_manager = this;

    this->addGlobalCloudListener(&m_syncEdgeE);
}

void CT_GlobalEdgeCloudManager::setGlobalCloudManager(const CT_AbstractGlobalPointCloudManager *globalPointCloudManager)
{
    m_gpcm = (CT_AbstractGlobalPointCloudManager*)globalPointCloudManager;
    m_gpcm->addGlobalCloudListener(&m_syncEdgeP);
}

void CT_GlobalEdgeCloudManager::setGlobalCloudManager(const CT_AbstractGlobalFaceCloudManager *globalFaceCloudManager)
{
    m_gfcm = (CT_AbstractGlobalFaceCloudManager*)globalFaceCloudManager;
    m_gfcm->addGlobalCloudListener(&m_syncEdgeF);
}

// called when edge is deleted from this cloud
void CT_GlobalEdgeCloudManager::SyncEdgeE::cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size)
{
    if(size > 0)
    {
        // this method is called when a list of edge has been removed from this global cloud

        // we will remove edge of all edges that contains a edge that have been removed
        // and shift them that is after the list of edge deleted
        CT_EdgeCloudStdVector *cloud = m_manager->globalCloud();

        size_t currentIndex = 0;

        // first we will loop over edge between 0 and beginIndex
        CT_EdgeCloudStdVector::Iterator begin = cloud->begin()+currentIndex;
        CT_EdgeCloudStdVector::Iterator end = cloud->begin()+beginIndex;

        if(begin != end)
        {
            size_t beginIndexToIgnore = 0;
            size_t endIndexToIgnore = 0;

            size_t edgeIndex;
            size_t endIndex = beginIndex+size-1;

            CT_EdgeCloudStdVector::Iterator beginOrig = begin;
            CT_EdgeCloudStdVector::Iterator nextEnd = end;

            while(begin != nextEnd)
            {
                if(m_manager->nextIndexToIgnoreToSync(currentIndex, beginIndexToIgnore, endIndexToIgnore))
                {
                    nextEnd = beginOrig + beginIndexToIgnore;

                    if(nextEnd > end)
                        nextEnd = end;
                }

                while(begin != nextEnd)
                {
                    CT_Edge &edge = (*begin);

                    for(int i=0; i<3; ++i)
                    {
                        edgeIndex = edge.iEdge(i);

                        if(edgeIndex >= beginIndex)
                        {
                            if(edgeIndex <= endIndex)
                            {
                                edge.setEdge(i, (size_t)0);
                            }
                            else
                            {
                                edge.setEdge(i, edgeIndex-size);
                            }
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

        // second we will loop over edge between 0+beginIndex+size and end of cloud
        currentIndex = beginIndex+size;
        begin = cloud->begin()+currentIndex;
        end = cloud->end();

        if(begin != end)
        {
            size_t beginIndexToIgnore = 0;
            size_t endIndexToIgnore = 0;

            size_t edgeIndex;
            size_t endIndex = beginIndex+size-1;

            CT_EdgeCloudStdVector::Iterator beginOrig = begin;
            CT_EdgeCloudStdVector::Iterator nextEnd = end;

            while(begin != nextEnd)
            {
                if(m_manager->nextIndexToIgnoreToSync(currentIndex, beginIndexToIgnore, endIndexToIgnore))
                {
                    nextEnd = beginOrig + beginIndexToIgnore;

                    if(nextEnd > end)
                        nextEnd = end;
                }

                while(begin != nextEnd)
                {
                    CT_Edge &edge = (*begin);

                    for(int i=0; i<3; ++i)
                    {
                        edgeIndex = edge.iEdge(i);

                        if(edgeIndex >= beginIndex)
                        {
                            if(edgeIndex <= endIndex)
                            {
                                edge.setEdge(i, (size_t)0);
                            }
                            else
                            {
                                edge.setEdge(i, edgeIndex-size);
                            }
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

// called when face is deleted from global cloud
void CT_GlobalEdgeCloudManager::SyncEdgeF::cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size)
{
    if(size > 0)
    {
        // this method is called when a list of face has been removed from the global cloud

        // we will remove face of all edges that contains a face that have been removed
        // and shift them that is after the list of face deleted
        CT_EdgeCloudStdVector *cloud = m_manager->globalCloud();

        size_t currentIndex = 0;

        CT_EdgeCloudStdVector::Iterator begin = cloud->begin() + currentIndex;
        CT_EdgeCloudStdVector::Iterator end = cloud->end();

        if(begin != end)
        {
            size_t beginIndexToIgnore = 0;
            size_t endIndexToIgnore = 0;

            size_t faceIndex;
            size_t endIndex = beginIndex+size-1;

            CT_EdgeCloudStdVector::Iterator beginOrig = begin;
            CT_EdgeCloudStdVector::Iterator nextEnd = end;

            while(begin != nextEnd)
            {
                if(m_manager->nextIndexToIgnoreToSync(currentIndex, beginIndexToIgnore, endIndexToIgnore))
                {
                    nextEnd = beginOrig + beginIndexToIgnore;

                    if(nextEnd > end)
                        nextEnd = end;
                }

                while(begin != nextEnd)
                {
                    CT_Edge &edge = (*begin);

                    faceIndex = edge.iFace();

                    if(faceIndex >= beginIndex)
                    {
                        if(faceIndex <= endIndex)
                        {
                            edge.setFace((size_t)0);
                        }
                        else
                        {
                            edge.setFace(faceIndex-size);
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

// called when point is deleted from global cloud
void CT_GlobalEdgeCloudManager::SyncEdgeP::cloudIndexShiftedAfterThisCloud(const size_t &beginIndex, const size_t &size)
{
    if(size > 0)
    {
        // this method is called when a list of point has been removed from the global cloud

        // we will remove point of all edges that contains a point that have been removed
        // and shift them that is after the list of point deleted
        CT_EdgeCloudStdVector *cloud =  m_manager->globalCloud();

        size_t currentIndex = 0;

        CT_EdgeCloudStdVector::Iterator begin = cloud->begin()+currentIndex;
        CT_EdgeCloudStdVector::Iterator end = cloud->end();

        if(begin != end)
        {
            size_t beginIndexToIgnore = 0;
            size_t endIndexToIgnore = 0;

            size_t pointIndex;
            size_t endIndex = beginIndex+size-1;

            CT_EdgeCloudStdVector::Iterator beginOrig = begin;
            CT_EdgeCloudStdVector::Iterator nextEnd = end;

            while(begin != nextEnd)
            {
                if(m_manager->nextIndexToIgnoreToSync(currentIndex, beginIndexToIgnore, endIndexToIgnore))
                {
                    nextEnd = beginOrig + beginIndexToIgnore;

                    if(nextEnd > end)
                        nextEnd = end;
                }

                while(begin != nextEnd)
                {
                    CT_Edge &edge = (*begin);

                    for(int i=0; i<2; ++i)
                    {
                        pointIndex = edge.iPointAt(i);

                        if(pointIndex >= beginIndex)
                        {
                            if(pointIndex <= endIndex)
                            {
                                edge.setPoint(i, (size_t)0);
                            }
                            else
                            {
                                edge.setPoint(i, pointIndex-size);
                            }
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
