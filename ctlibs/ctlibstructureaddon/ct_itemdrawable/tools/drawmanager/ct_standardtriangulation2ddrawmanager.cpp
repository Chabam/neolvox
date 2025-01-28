#include "ct_standardtriangulation2ddrawmanager.h"

#include "ct_itemdrawable/ct_triangulation2d.h"

const QString CT_StandardTriangulation2DDrawManager::INDEX_CONFIG_NODES_VISIBLE = CT_StandardTriangulation2DDrawManager::staticInitConfigNodesVisible();
const QString CT_StandardTriangulation2DDrawManager::INDEX_CONFIG_EDGES_VISIBLE = CT_StandardTriangulation2DDrawManager::staticInitConfigEdgesVisible();
const QString CT_StandardTriangulation2DDrawManager::INDEX_CONFIG_HULL_VISIBLE = CT_StandardTriangulation2DDrawManager::staticInitConfigHullVisible();
const QString CT_StandardTriangulation2DDrawManager::INDEX_CONFIG_VORONOI_VISIBLE = CT_StandardTriangulation2DDrawManager::staticInitConfigVoronoiVisible();

CT_StandardTriangulation2DDrawManager::CT_StandardTriangulation2DDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_Triangulation2D::staticName() : drawConfigurationName)
{
}

void CT_StandardTriangulation2DDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::draw(view, painter, itemDrawable);

    const CT_Triangulation2D &item = dynamic_cast<const CT_Triangulation2D&>(itemDrawable);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_NODES_VISIBLE).toBool())
        drawNodes(view, painter, item);


    if(drawConfiguration()->variableValue(INDEX_CONFIG_EDGES_VISIBLE).toBool())
        drawEdges(view, painter, item);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_HULL_VISIBLE).toBool())
        drawHull(view, painter, item);

    if(drawConfiguration()->variableValue(INDEX_CONFIG_VORONOI_VISIBLE).toBool())
        drawVoronoi(view, painter, item);
}

CT_ItemDrawableConfiguration CT_StandardTriangulation2DDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item = CT_ItemDrawableConfiguration(drawConfigurationName);

    item.addAllConfigurationOf(CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager::createDrawConfiguration(drawConfigurationName));
    item.addNewConfiguration(CT_StandardTriangulation2DDrawManager::staticInitConfigNodesVisible(), QObject::tr("Points"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardTriangulation2DDrawManager::staticInitConfigEdgesVisible(), QObject::tr("Aretes"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(CT_StandardTriangulation2DDrawManager::staticInitConfigHullVisible(), QObject::tr("Enveloppe convexe"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(CT_StandardTriangulation2DDrawManager::staticInitConfigVoronoiVisible(), QObject::tr("Voronoï"), CT_ItemDrawableConfiguration::Bool, false);

    return item;
}

// PROTECTED //

QString CT_StandardTriangulation2DDrawManager::staticInitConfigNodesVisible()
{
    return "T2D_NV";
}

QString CT_StandardTriangulation2DDrawManager::staticInitConfigEdgesVisible()
{
    return "T2D_EV";
}

QString CT_StandardTriangulation2DDrawManager::staticInitConfigHullVisible()
{
    return "T2D_HV";
}

QString CT_StandardTriangulation2DDrawManager::staticInitConfigVoronoiVisible()
{
    return "T2D_VV";
}

void CT_StandardTriangulation2DDrawManager::drawNodes(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const
{
    Q_UNUSED(view)

    painter.setPointSize(10.0);
    painter.setColor(Qt::white);

    CT_DelaunayTriangulation* delaunayT = item.getDelaunayT();
    const QList<CT_DelaunayVertex*> &vertices = delaunayT->getInsertedVertices();

    for (int i = 0 ; i < vertices.size() ; i++)
    {
        CT_DelaunayVertex* vertex = vertices.at(i);
        Eigen::Vector3d *p = vertex->getData();

        painter.drawPoint((*p)(0), (*p)(1), (*p)(2));
    }


    painter.restoreDefaultPointSize();
}

void CT_StandardTriangulation2DDrawManager::drawEdges(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const
{
    Q_UNUSED(view)

    painter.setColor(Qt::white);

    CT_DelaunayTriangulation* delaunayT = item.getDelaunayT();

    QMultiMap<CT_DelaunayVertex *, CT_DelaunayVertex *> edges = delaunayT->getEdges();

    QMapIterator<CT_DelaunayVertex *, CT_DelaunayVertex *> it(edges);
    while(it.hasNext())
    {
        it.next();
        CT_DelaunayVertex *v1 = it.key();
        CT_DelaunayVertex *v2 = it.value();

        Eigen::Vector3d *p1 = v1->getData();
        Eigen::Vector3d *p2 = v2->getData();

        painter.drawLine((*p1)(0), (*p1)(1), (*p1)(2), (*p2)(0), (*p2)(1), (*p2)(2));
    }
}

void CT_StandardTriangulation2DDrawManager::drawHull(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const
{
    Q_UNUSED(view)

    painter.setColor(Qt::red);

    CT_DelaunayTriangulation* delaunayT = item.getDelaunayT();

   const CT_DelaunayOutline &outline = delaunayT->getOutline();

   const QList<CT_DelaunayVertex*> &vertices = outline.getVertices();

   for (int j = 0 ; j < vertices.size() ; j++)
   {
       Eigen::Vector3d *p1 = vertices.at(j)->getData();
       Eigen::Vector3d *p2 = nullptr;

       if (j < vertices.size() - 1)
       {
           p2 = vertices.at(j+1)->getData();
       } else {
           p2 = vertices.at(0)->getData();
       }

       painter.drawLine((*p1)(0), (*p1)(1), (*p1)(2), (*p2)(0), (*p2)(1), (*p2)(2));
   }

}

void CT_StandardTriangulation2DDrawManager::drawVoronoi(GraphicsViewInterface &view, PainterInterface &painter, const CT_Triangulation2D &item) const
{
    Q_UNUSED(view)

    painter.setColor(255, 0, 255);

    CT_DelaunayTriangulation* delaunayT = item.getDelaunayT();
    const QList<CT_DelaunayVertex*> &voronoi = delaunayT->getCleanVoronoiDiagram();

    for (int i = 0 ; i < voronoi.size() ; i++)
    {
        CT_DelaunayVertex* vertex = voronoi.at(i);

        CT_Polygon2DData* poly = vertex->getVoroShape();
        poly->draw(painter, false, true, 0);
    }
}
