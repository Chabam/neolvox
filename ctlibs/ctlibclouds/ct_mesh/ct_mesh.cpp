#include "ct_mesh.h"

#include "ct_cloud/tools/abstract/ct_abstractglobalcloudmanagert.h"
#include "ct_mesh/tools/ct_meshallocatort.h"
#include "ct_mesh/ct_face.h"
#include "ct_mesh/ct_edge.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#endif

CT_Mesh::CT_Mesh()
{
    m_pVert = nullptr;
    m_pFace = nullptr;
    m_pHedge = nullptr;
}

CT_Mesh::~CT_Mesh()
{
    clear();
}

CT_AbstractModifiablePointCloudIndex& CT_Mesh::vert() const
{
    return *pVert();
}

CT_AbstractModifiableFaceCloudIndex& CT_Mesh::face() const
{
    return *pFace();
}

CT_AbstractModifiableEdgeCloudIndex& CT_Mesh::hedge() const
{
    return *pHedge();
}

CT_AbstractModifiablePointCloudIndex* CT_Mesh::pVert() const
{
    if(m_vert == nullptr)
        return nullptr;

    return m_vert->abstractModifiableCloudIndexT();
}

CT_AbstractModifiableFaceCloudIndex* CT_Mesh::pFace() const
{
    if(m_face == nullptr)
        return nullptr;

    return m_face->abstractModifiableCloudIndexT();
}

CT_AbstractModifiableEdgeCloudIndex* CT_Mesh::pHedge() const
{
    if(m_hedge == nullptr)
        return nullptr;

    return m_hedge->abstractModifiableCloudIndexT();
}

CT_AbstractPointCloudIndex* CT_Mesh::abstractVert() const
{
    return m_pVert;
}

CT_AbstractFaceCloudIndex* CT_Mesh::abstractFace() const
{
    return m_pFace;
}

CT_AbstractEdgeCloudIndex* CT_Mesh::abstractHedge() const
{
    return m_pHedge;
}

CT_MPCIR CT_Mesh::registeredVert() const
{
    return m_vert;
}

CT_MFCIR CT_Mesh::registeredFace() const
{
    return m_face;
}

CT_MECIR CT_Mesh::registeredHedge() const
{
    return m_hedge;
}

void CT_Mesh::createCylinder(double radius, double height, int sides, double xPos, double yPos, double zPos)
{
    clear();

    double inc = (2.0 * M_PI) / ((double)sides);
    double theta = 0;

    CT_MutablePointIterator vi = CT_MeshAllocatorT<CT_Mesh>::AddVertices(this, 2*sides);
    CT_MutableFaceIterator fi = CT_MeshAllocatorT<CT_Mesh>::AddFaces(this, 2*sides);
    CT_MutableEdgeIterator ei = CT_MeshAllocatorT<CT_Mesh>::AddHEdges(this, 6*sides);

    vi.next();
    fi.next();
    ei.next();

    double minH = 0;
    double maxH = height;

    CT_Point v;
    v(0) = maxH + xPos;
    v(1) = radius + yPos;
    v(2) = zPos;

    vi.replaceCurrentPoint(v);

    size_t p = vi.cIndex();

    for(size_t i=0; static_cast<int>(i)<sides; ++i)
    {
        const CT_Point &v0 = vi.cT();
        size_t p0 = vi.cIndex();
        vi.next();

        CT_Point v1;

        v1(0) = minH + xPos;
        v1(1) = v0(1);
        v1(2) = v0(2);

        vi.replaceCurrentPoint(v1);
        size_t p1 = vi.cIndex();
        vi.next();

        theta += inc;

        size_t p2;
        size_t p3;

        if(static_cast<int>(i)<sides-1)
        {
            CT_Point v2;
            p2 = vi.cIndex();
            p3 = p2+1;

            v2(0) = maxH + xPos;
            v2(1) = (radius * cos(theta)) + yPos;
            v2(2) = (radius * sin(theta)) + zPos;

            vi.replaceCurrentPoint(v2);
        }
        else
        {
            p2 = p;
            p3 = p+1;
        }

        CT_Face &face1 = fi.cT();
        size_t faceIndex = fi.cIndex();

        size_t e1Index = ei.cIndex();
        size_t e2Index;
        size_t e3Index;

        face1.setEdge(e1Index);

        CT_Edge &e1 = ei.cT();
        e1.setPoint0(p0);
        e1.setPoint1(p1);
        e1.setFace(faceIndex);

        CT_Edge &e2 = ei.next().cT();
        e2.setPoint0(p1);
        e2.setPoint1(p2);
        e1.setFace(faceIndex);
        e2Index = ei.cIndex();

        CT_Edge &e3 = ei.next().cT();
        e3.setPoint0(p2);
        e3.setPoint1(p0);
        e3.setFace(faceIndex);
        e3Index = ei.cIndex();
        ei.next();

        e1.setNext(e2Index);
        e1.setPrevious(e3Index);
        e2.setNext(e3Index);
        e2.setPrevious(e1Index);
        e3.setNext(e1Index);
        e3.setPrevious(e2Index);

        CT_Face &face2 = fi.next().cT();
        faceIndex = fi.cIndex();

        e1Index = ei.cIndex();

        face2.setEdge(e1Index);

        CT_Edge &e11 = ei.cT();
        e11.setPoint0(p1);
        e11.setPoint1(p3);
        e11.setFace(faceIndex);

        CT_Edge &e22 = ei.next().cT();
        e22.setPoint0(p3);
        e22.setPoint1(p2);
        e11.setFace(faceIndex);
        e2Index = ei.cIndex();

        CT_Edge &e33 = ei.next().cT();
        e33.setPoint0(p2);
        e33.setPoint1(p1);
        e33.setFace(faceIndex);
        e3Index = ei.cIndex();

        e11.setNext(e2Index);
        e11.setPrevious(e3Index);
        e22.setNext(e3Index);
        e22.setPrevious(e1Index);
        e33.setNext(e1Index);
        e33.setPrevious(e2Index);

        ei.next();
        fi.next();
    }
}

void CT_Mesh::clear()
{
    PS_REPOSITORY->begingDeleteMultiCloud();

    // clear all index so it will not be sync when we
    // clear ege, face, point that have been new created
    m_vert.clear();
    m_face.clear();
    m_hedge.clear();

    // disable sync of all edge created
    size_t s = m_newEdge.size();

    for(size_t i=0; i<s; ++i)
        PS_REPOSITORY->globalCloudManager<CT_Edge>()->setEnableSyncForCIR(m_newEdge[i].data(), false);

    // disable sync of all face created
    s = m_newFace.size();

    for(size_t i=0; i<s; ++i)
        PS_REPOSITORY->globalCloudManager<CT_Face>()->setEnableSyncForCIR(m_newFace[i].data(), false);

    // clear edge
    s = m_newEdge.size();

    for(size_t i=0; i<s; ++i)
        m_newEdge[i].clear();

    // clear face
    s = m_newFace.size();

    for(size_t i=0; i<s; ++i)
        m_newFace[i].clear();

    // clear point
    s = m_newVert.size();

    for(size_t i=0; i<s; ++i)
        m_newVert[i].clear();

    PS_REPOSITORY->endDeleteMultiCloud();
}
