#ifndef CT_STANDARDMESHMODELDRAWMANAGER_H
#define CT_STANDARDMESHMODELDRAWMANAGER_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/tools/drawmanager/abstract/ct_abstractitemdrawabledrawmanager.h"

#include "ct_accessor/ct_faceaccessor.h"
#include "ct_accessor/ct_edgeaccessor.h"

class CT_Mesh;

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardMeshModelDrawManager : public CT_AbstractItemDrawableDrawManager
{
    using SuperClass = CT_AbstractItemDrawableDrawManager;

public:
    CT_StandardMeshModelDrawManager(const QString& drawConfigurationName = "");

    void draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const override;

    virtual void drawFaces(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void beginDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void drawFaceAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void endDrawMultipleFace(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);

    virtual void drawEdges(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void beginDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void drawEdgeAt(const size_t &index, GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);
    virtual void endDrawMultipleEdge(GraphicsViewInterface &view, PainterInterface &painter, const CT_Mesh *mesh);

protected:

    const static QString INDEX_CONFIG_SHOW_FACES;
    const static QString INDEX_CONFIG_SHOW_EDGES;
    const static QString INDEX_CONFIG_SHOW_POINTS;
    const static QString INDEX_CONFIG_BOUNDING_SHAPE_VISIBLE;
    const static QString INDEX_CONFIG_BOUNDING_SHAPE_POINT_SIZE;
    const static QString INDEX_CONFIG_BOUNDING_SHAPE_CENTER_POINT_VISIBLE;

    CT_FaceAccessor m_fAccess;
    CT_EdgeAccessor m_eAccess;

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

    static QString staticInitConfigShowFaces();
    static QString staticInitConfigShowEdges();
    static QString staticInitConfigShowPoints();
    static QString staticInitConfigBoundingShapeVisible();
    static QString staticInitConfigBoundingShapePointSize();
    static QString staticInitConfigBoundingShapeCenterPointVisible();
};

#endif // CT_STANDARDMESHMODELDRAWMANAGER_H
