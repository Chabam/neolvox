#ifndef CT_MESHALLOCATORT_H
#define CT_MESHALLOCATORT_H

#include <vector>
#include <stdlib.h>
#include <QSharedPointer>

#include "ct_defines.h"

#include "ct_cloudindex/tools/ct_cloudindexregistrationmanagert.h"

/**
 * The class that you must use if you want to add Point, Face or Edge to a CT_Mesh.
 */
template<typename MeshType>
class CT_MeshAllocatorT
{
public:
    typedef MeshType Mesh;

    /**
     * @brief Add new vertices that already exist in the global points cloud => Add indexes
     * @param mesh : the mesh where add indexes
     * @param n : number of indexes to add
     * @return an iterator to the first index added
     */
    static CT_MutablePointIndexIterator AddVerticeIndexes(Mesh *mesh, const size_t &n);
    static CT_MutableFaceIndexIterator AddFaceIndexes(Mesh *mesh, const size_t &n);
    static CT_MutableEdgeIndexIterator AddHEdgeIndexes(Mesh *mesh, const size_t &n);

    /**
     * @brief Add new vertices to the mesh and in consequence to the global points cloud
     * @param mesh : the mesh where add vertices
     * @param n : number of vertices to add
     * @return an iterator to the first index added
     */
    static CT_MutablePointIterator AddVertices(Mesh *mesh, const size_t &n);
    static CT_MutableFaceIterator AddFaces(Mesh *mesh, const size_t &n);
    static CT_MutableEdgeIterator AddHEdges(Mesh *mesh, const size_t &n);

private:
    template<typename T, typename CloudIndex, typename Iterator>
    static Iterator addT(const size_t &n,
                         std::vector< QSharedPointer< CT_AbstractNotModifiableCloudIndexRegisteredT<T> > > &CIRCollection,
                         typename CT_AbstractCloudIndexRegistrationManagerT<T>::CT_AbstractModifiableCIR &meshCir,
                         CloudIndex **meshCI);
};

#include "ct_mesh/tools/ct_meshallocatort.hpp"

#endif // CT_MESHALLOCATORT_H
