#ifndef CT_STANDARDTRIANGULATION2DDRAWMANAGER_H
#define CT_STANDARDTRIANGULATION2DDRAWMANAGER_H

#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

class CT_Triangulation2D;

class CTLIBSTRUCTUREADDON_EXPORT CT_StandardTriangulation2DDrawManager : public CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager
{
    using SuperClass = CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager;

public:
    CT_StandardTriangulation2DDrawManager(QString drawConfigurationName = QObject::tr("Triangulation2D"));

    void draw(GraphicsViewInterface& view,
              PainterInterface& painter,
              const CT_AbstractItemDrawable& itemDrawable) const override;

protected:

    const static QString INDEX_CONFIG_NODES_VISIBLE;
    const static QString INDEX_CONFIG_EDGES_VISIBLE;
    const static QString INDEX_CONFIG_HULL_VISIBLE;
    const static QString INDEX_CONFIG_VORONOI_VISIBLE;

    static QString staticInitConfigNodesVisible();
    static QString staticInitConfigEdgesVisible();
    static QString staticInitConfigHullVisible();
    static QString staticInitConfigVoronoiVisible();

    CT_ItemDrawableConfiguration createDrawConfiguration(QString drawConfigurationName) const override;

    void drawNodes(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const;
    void drawEdges(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const;
    void drawHull(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const;
    void drawVoronoi(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const;
};

#endif // CT_STANDARDTRIANGULATION2DDRAWMANAGER_H
