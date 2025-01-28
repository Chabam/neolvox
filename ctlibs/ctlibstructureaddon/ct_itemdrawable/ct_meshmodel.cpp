#include "ct_meshmodel.h"

const CT_StandardMeshModelDrawManager CT_MeshModel::CT_MESHMODEL_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_MeshModel)

CT_MeshModel::CT_MeshModel() : SuperClass(),
    m_mesh(nullptr),
    m_autoDeleteMesh(true)
{
    setBaseDrawManager(&CT_MESHMODEL_DRAW_MANAGER);
}

CT_MeshModel::CT_MeshModel(CT_Mesh* mesh) : SuperClass(),
    m_mesh(mesh),
    m_autoDeleteMesh(true)
{
    setBaseDrawManager(&CT_MESHMODEL_DRAW_MANAGER);
}

CT_MeshModel::CT_MeshModel(const CT_MeshModel& other) : SuperClass(other)
{
    m_mesh = other.m_mesh;
    m_autoDeleteMesh = false;
}

CT_MeshModel::~CT_MeshModel()
{
    clearMesh();
}

void CT_MeshModel::setAutoDeleteMesh(bool enable)
{
    m_autoDeleteMesh = enable;
}

bool CT_MeshModel::autoDeleteMesh() const
{
    return m_autoDeleteMesh;
}

void CT_MeshModel::setMesh(CT_Mesh *mesh)
{
    clearMesh();
    m_mesh = mesh;
}

CT_Mesh* CT_MeshModel::mesh() const
{
    return m_mesh;
}

CT_Mesh& CT_MeshModel::cm()
{
    return *mesh();
}

void CT_MeshModel::clearMesh()
{
    if(autoDeleteMesh())
        delete m_mesh;

    m_mesh = nullptr;
}

void CT_MeshModel::drawFaces(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->drawFaces(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->drawFaces(view, painter, this->mesh());
}

void CT_MeshModel::beginDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->beginDrawMultipleFace(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->beginDrawMultipleFace(view, painter, this->mesh());
}

void CT_MeshModel::drawFaceAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->drawFaceAt(index, view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->drawFaceAt(index, view, painter, this->mesh());
}

void CT_MeshModel::endDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->endDrawMultipleFace(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->endDrawMultipleFace(view, painter, this->mesh());
}

void CT_MeshModel::drawEdges(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->drawEdges(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->drawEdges(view, painter, this->mesh());
}

void CT_MeshModel::beginDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->beginDrawMultipleEdge(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->beginDrawMultipleEdge(view, painter, this->mesh());
}

void CT_MeshModel::drawEdgeAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->drawEdgeAt(index, view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->drawEdgeAt(index, view, painter, this->mesh());
}

void CT_MeshModel::endDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter)
{
    if(alternativeDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)alternativeDrawManager())->endDrawMultipleEdge(view, painter, this->mesh());
    else if(baseDrawManager() != nullptr)
        ((CT_StandardMeshModelDrawManager*)baseDrawManager())->endDrawMultipleEdge(view, painter, this->mesh());
}

const CT_AbstractPointCloudIndex* CT_MeshModel::pointCloudIndex() const
{
    if(m_mesh == nullptr)
        return nullptr;

    return m_mesh->abstractVert();
}

const CT_AbstractFaceCloudIndex* CT_MeshModel::faceCloudIndex() const
{
    if(m_mesh == nullptr)
        return nullptr;

    return m_mesh->abstractFace();
}

const CT_AbstractEdgeCloudIndex* CT_MeshModel::edgeCloudIndex() const
{
    if(m_mesh == nullptr)
        return nullptr;

    return m_mesh->abstractHedge();
}

CT_PCIR CT_MeshModel::pointCloudIndexRegistered() const
{
    if(m_mesh == nullptr)
        return CT_PCIR(nullptr);

    return m_mesh->registeredVert();
}

CT_FCIR CT_MeshModel::faceCloudIndexRegistered() const
{
    if(m_mesh == nullptr)
        return CT_FCIR(nullptr);

    return m_mesh->registeredFace();
}

CT_ECIR CT_MeshModel::edgeCloudIndexRegistered() const
{
    if(m_mesh == nullptr)
        return CT_ECIR(nullptr);

    return m_mesh->registeredHedge();
}
