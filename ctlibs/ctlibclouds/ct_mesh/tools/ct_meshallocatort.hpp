#include "ct_mesh/tools/ct_meshallocatort.h"

#include "ct_global/ct_cloudscontext.h"

#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"

#include "ct_cloud/tools/ct_globalpointcloudmanager.h"

#include "ct_iterator/ct_mutablepointiterator.h"
#include "ct_iterator/ct_mutablefaceiterator.h"
#include "ct_iterator/ct_mutableedgeiterator.h"

#include "ct_iterator/ct_mutablepointindexiterator.h"
#include "ct_iterator/ct_mutablefaceindexiterator.h"
#include "ct_iterator/ct_mutableedgeindexiterator.h"

template<typename Mesh>
CT_MutablePointIndexIterator CT_MeshAllocatorT<Mesh>::AddVerticeIndexes(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutablePointIndexIterator(nullptr);

    CT_PointCloudIndexVector *v = const_cast<CT_PointCloudIndexVector*>(mesh->m_pVert);
    size_t lastSize = 0;

    if(v == nullptr)
    {
        v = new CT_PointCloudIndexVector(n);
        mesh->m_vert = PS_REPOSITORY->registerCloudIndex(v);
        mesh->m_pVert = v;
    }
    else
    {
        lastSize = v->size();
        v->resize(lastSize + n);
    }

    CT_MutablePointIndexIterator it(mesh->m_vert);
    it.jump(lastSize);

    return it;
}

template<typename Mesh>
CT_MutableFaceIndexIterator CT_MeshAllocatorT<Mesh>::AddFaceIndexes(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutableFaceIndexIterator(nullptr);

    CT_FaceCloudIndexVector *f = const_cast<CT_FaceCloudIndexVector*>(mesh->m_pFace);
    size_t lastSize = 0;

    if(f == nullptr)
    {
        f = new CT_FaceCloudIndexVector(n);
        mesh->m_face = PS_REPOSITORY->registerCloudIndex(f);
        mesh->m_pFace = f;
    }
    else
    {
        f->resize(f->size() + n);
    }

    CT_MutableFaceIndexIterator it(mesh->m_face);
    it.jump(lastSize);

    return it;
}

template<typename Mesh>
CT_MutableEdgeIndexIterator CT_MeshAllocatorT<Mesh>::AddHEdgeIndexes(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutableEdgeIndexIterator(nullptr);

    CT_EdgeCloudIndexVector *e = const_cast<CT_EdgeCloudIndexVector*>(mesh->m_pHedge);
    size_t lastSize = 0;

    if(e == nullptr)
    {
        e = new CT_EdgeCloudIndexVector(n);
        mesh->m_hedge = PS_REPOSITORY->registerCloudIndex(e);
        mesh->m_pHedge = e;
    }
    else
    {
        e->resize(e->size() + n);
    }

    CT_MutableEdgeIndexIterator it(mesh->m_hedge);
    it.jump(lastSize);

    return it;
}

template<typename Mesh>
CT_MutablePointIterator CT_MeshAllocatorT<Mesh>::AddVertices(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutablePointIterator(nullptr);

    CT_PointCloudIndexVector *v = static_cast<CT_PointCloudIndexVector*>(mesh->m_pVert);

    CT_MutablePointIterator it = addT<CT_PointData, CT_PointCloudIndexVector, CT_MutablePointIterator>(n,
                                                                    mesh->m_newVert,
                                                                    mesh->m_vert,
                                                                    &v);
    mesh->m_pVert = v;

    return it;
}

template<typename Mesh>
CT_MutableFaceIterator CT_MeshAllocatorT<Mesh>::AddFaces(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutableFaceIterator(nullptr);

    CT_FaceCloudIndexVector *f = static_cast<CT_FaceCloudIndexVector*>(mesh->m_pFace);

    CT_MutableFaceIterator  it = addT<CT_Face, CT_FaceCloudIndexVector, CT_MutableFaceIterator>(n,
                                                                mesh->m_newFace,
                                                                mesh->m_face,
                                                                &f);
    mesh->m_pFace = f;

    return it;
}

template<typename Mesh>
CT_MutableEdgeIterator CT_MeshAllocatorT<Mesh>::AddHEdges(Mesh *mesh, const size_t &n)
{
    if(mesh == nullptr)
        return CT_MutableEdgeIterator(nullptr);

    CT_EdgeCloudIndexVector *e = static_cast<CT_EdgeCloudIndexVector*>(mesh->m_pHedge);

    CT_MutableEdgeIterator it = addT<CT_Edge, CT_EdgeCloudIndexVector, CT_MutableEdgeIterator>(n,
                                                                 mesh->m_newEdge,
                                                                 mesh->m_hedge,
                                                                 &e);
    mesh->m_pHedge = e;

    return it;
}

template<typename Mesh>
template<typename T, typename CloudIndex, typename Iterator>
Iterator CT_MeshAllocatorT<Mesh>::addT(const size_t &n,
                                       std::vector< QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > > &meshCIRCollection,
                                       typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractModifiableCIR &meshCir,
                                       CloudIndex **meshCI)
{
    QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > cir;

    // first index in the global cloud for the new cloud
    ct_index_type firstPIndex = 0;

    // if the mesh has already a cloud created by the cloud manager
    if(!meshCIRCollection.empty())
    {
        QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > tmpCir = meshCIRCollection[meshCIRCollection.size()-1];

        size_t s = tmpCir->size();
        firstPIndex = ct_index_type(tmpCir->last()+1);

        // resize last point cloud index added to mesh if it was at the end of the global points cloud (resize is the same time the global points cloud)
        cir = PS_REPOSITORY->resizeCloudIndexAndGlobalCloud<T>(tmpCir, s+n);
    }

    // if the mesh don't have a points cloud or if the last created can't be resized
    if(cir.data() == nullptr)
    {
        // create a new points cloud
        cir = PS_REPOSITORY->createNewCloud<T>(n);

        firstPIndex = ct_index_type(cir->first());

        // add them to the collection
        meshCIRCollection.push_back(cir);
    }

    // if the mesh don't have a cloud index we create one
    if(meshCir.data() == nullptr)
    {
        *meshCI = new CloudIndex();
        meshCir = PS_REPOSITORY->registerCloudIndex<T>(const_cast<CloudIndex*>(*meshCI));
    }

    CloudIndex *index = const_cast<CloudIndex*>(*meshCI);

    // begin of the first new index
    size_t begin = index->size();

    // resize the index
    index->resize(index->size() + n);

    // end of the index
    size_t s = index->size();

    ct_index_type a = firstPIndex;

    for(size_t i=begin; i<s; ++i)
    {
        index->replaceIndex(i, a, false);
        ++a;
    }

    Iterator iterator(index);
    iterator.jump(begin);

    return iterator;
}
