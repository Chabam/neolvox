#ifndef CT_MESHMODEL_H
#define CT_MESHMODEL_H

#include "ct_itemdrawable/abstract/ct_abstractmeshmodel.h"

#include "ct_itemdrawable/tools/drawmanager/ct_standardmeshmodeldrawmanager.h"
#include "ct_mesh/ct_mesh.h"

/**
 * @brief A singular item that can manage a CT_Mesh
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_MeshModel : public CT_AbstractMeshModel
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_MeshModel, CT_AbstractMeshModel, Mesh model)
    using SuperClass = CT_AbstractMeshModel;

public:
    CT_MeshModel();
    CT_MeshModel(CT_Mesh* mesh);
    CT_MeshModel(const CT_MeshModel& other);
    ~CT_MeshModel();

    /**
     * @brief Define if the mesh must be automatically deleted from memory. (true by default)
     */
    void setAutoDeleteMesh(bool enable);

    /**
     * @brief Return true if the mesh must be automatically deleted from memory
     */
    bool autoDeleteMesh() const;

    /**
     * @brief Set the mesh. If a mesh already exist it will be deleted from memory.
     */
    void setMesh(CT_Mesh *mesh);

    /**
     * @brief Getter for mesh
     */
    CT_Mesh* mesh() const;

    /**
     * @brief Getter for mesh. (Same as MeshLab => not a attribute but a method)
     */
    CT_Mesh& cm();

    /**
     * @brief Clear mesh (delete from memory if method "autoDeleteMesh" returns true)
     */
    void clearMesh();

    /**
     * @brief Draw only faces
     */
    void drawFaces(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Begin draw multiple faces
     */
    void beginDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Draw only face at index 'index'
     */
    void drawFaceAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Finish draw multiple faces
     */
    void endDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Draw only edges
     */
    void drawEdges(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Begin draw multiple edges
     */
    void beginDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Draw only edge at index 'index'
     */
    void drawEdgeAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Finish draw multiple edges
     */
    void endDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter) override;

    /**
     * @brief Returns the point cloud index
     */
    const CT_AbstractPointCloudIndex* pointCloudIndex() const override;

    /**
     * @brief Returns the face cloud index
     */
    const CT_AbstractFaceCloudIndex* faceCloudIndex() const override;

    /**
     * @brief Returns the edge cloud index
     */
    const CT_AbstractEdgeCloudIndex* edgeCloudIndex() const override;

    /**
     * @brief Returns the point cloud index registered
     */
    CT_PCIR pointCloudIndexRegistered() const override;

    /**
     * @brief Returns the face cloud index registered
     */
    CT_FCIR faceCloudIndexRegistered() const override;

    /**
     * @brief Returns the edge cloud index registered
     */
    CT_ECIR edgeCloudIndexRegistered() const override;

    CT_ITEM_COPY_IMP(CT_MeshModel)

protected:

    const static CT_StandardMeshModelDrawManager CT_MESHMODEL_DRAW_MANAGER;

private:
    CT_Mesh *m_mesh;
    bool    m_autoDeleteMesh;
};

#endif // CT_MESHMODEL_H
