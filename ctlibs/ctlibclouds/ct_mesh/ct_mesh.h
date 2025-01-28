#ifndef CT_MESH_H
#define CT_MESH_H

#include "ctlibclouds_global.h"
#include "ct_defines.h"

#include "ct_cloudindex/tools/abstract/ct_abstractcloudindexregistrationmanagert.h"

/**
 * A Mesh. You must use a CT_MeshAllocatorT to add Point or Face or Edge to a CT_Mesh.
 */
class CTLIBCLOUDS_EXPORT CT_Mesh
{
public:
    CT_Mesh();
    ~CT_Mesh();

    /**
     * @brief vert, face, hedge is the name used in the class TriMesh of vcg
     *        and the attributes is public. To help users to migrate we decided to do
     *        the same thing but use functions instead of public attributes.
     */
    CT_AbstractModifiablePointCloudIndex& vert() const;
    CT_AbstractModifiableFaceCloudIndex& face() const;
    CT_AbstractModifiableEdgeCloudIndex& hedge() const;

    CT_AbstractModifiablePointCloudIndex* pVert() const;
    CT_AbstractModifiableFaceCloudIndex* pFace() const;
    CT_AbstractModifiableEdgeCloudIndex* pHedge() const;

    CT_AbstractPointCloudIndex* abstractVert() const;
    CT_AbstractFaceCloudIndex* abstractFace() const;
    CT_AbstractEdgeCloudIndex* abstractHedge() const;

    CT_MPCIR registeredVert() const;
    CT_MFCIR registeredFace() const;
    CT_MECIR registeredHedge() const;

    /**
     * @brief Change this mesh to a cylinder
     */
    void createCylinder(double radius, double height, int sides, double xPos = 0, double yPos = 0, double zPos = 0);

    /**
     * @brief Clear this mesh
     */
    void clear();

protected:

    template<typename MeshType> friend class CT_MeshAllocatorT;

    /**
     * @brief set of vertices (index)
     */
    CT_MPCIR                                m_vert;
    CT_AbstractModifiablePointCloudIndex    *m_pVert;    // used by CT_MeshAllocatorT to avoid dynamic_cast

    /**
     * @brief set of face (index)
     */
    CT_MFCIR                                m_face;
    CT_AbstractModifiableFaceCloudIndex     *m_pFace;    // used by CT_MeshAllocatorT to avoid dynamic_cast

    /**
     * @brief set of hedge (index)
     */
    CT_MECIR                                m_hedge;
    CT_AbstractModifiableEdgeCloudIndex     *m_pHedge;    // used by CT_MeshAllocatorT to avoid dynamic_cast

    /**
     * @brief a collection of vertices created in the global points cloud
     */
    std::vector< CT_NMPCIR >    m_newVert;

    /**
     * @brief a collection of faces created in the global faces cloud
     */
    std::vector< CT_NMFCIR >    m_newFace;

    /**
     * @brief a collection of edges created in the global edges cloud
     */
    std::vector< CT_NMECIR >    m_newEdge;
};

#endif // CT_MESH_H
