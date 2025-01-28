#include "ct_standardplanarbsplinedrawmanager.h"

#include "ct_itemdrawable/ct_planarbspline.h"
#include "painterinterface.h"

const QString CT_StandardPlanarBSplineDrawManager::INDEX_CONFIG_DRAW_CONTROL_POINTS= CT_StandardPlanarBSplineDrawManager::staticInitConfigDrawControlPoints();
const QString CT_StandardPlanarBSplineDrawManager::INDEX_CONFIG_CONTROL_POINTS_SIZE = CT_StandardPlanarBSplineDrawManager::staticInitConfigControlPointsSize();
const QString CT_StandardPlanarBSplineDrawManager::INDEX_CONFIG_DRAW_POLYLINE = CT_StandardPlanarBSplineDrawManager::staticInitConfigDrawPolyline();

CT_StandardPlanarBSplineDrawManager::CT_StandardPlanarBSplineDrawManager(QString drawConfigurationName) : SuperClass(drawConfigurationName.isEmpty() ? CT_PlanarBSpline::staticName() : drawConfigurationName)
{
}

void CT_StandardPlanarBSplineDrawManager::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_PlanarBSpline& item = dynamic_cast<const CT_PlanarBSpline&>(itemDrawable);
    const CT_PlanarBSplineData& data =  (const CT_PlanarBSplineData&)item.getData();

    bool drawCP = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_CONTROL_POINTS).toBool();
    int CPsize = drawConfiguration()->variableValue(INDEX_CONFIG_CONTROL_POINTS_SIZE).toInt();
    bool drawPolyline = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POLYLINE).toBool();


    const QVector<Eigen::Vector3d> &cps = data.getControlPoints();
    const QList<Eigen::Vector3d> &polyline = item.getPolyline();

    if (drawCP)
    {
        painter.setPointSize(CPsize);
        const int size = cps.size();
        for (int i = 0 ; i < size ; i++)
        {
            painter.drawPoint(cps[i](0), cps[i](1), cps[i](2));
        }
        painter.restoreDefaultPointSize();
    }

    const int size = polyline.size()-1;
    if (drawPolyline)
    {
        if (size > 0)
        {
            for (int i = 0 ; i < size ; i++)
            {
                painter.drawLine(polyline[i](0), polyline[i](1), polyline[i](2),polyline[i+1](0), polyline[i+1](1), polyline[i+1](2));
            }
            painter.drawLine(polyline[size](0), polyline[size](1), polyline[size](2),polyline[0](0), polyline[0](1), polyline[0](2));
        }
    }

}

QString CT_StandardPlanarBSplineDrawManager::staticInitConfigDrawControlPoints()
{
    return "PLBSPL_DCP";
}

QString CT_StandardPlanarBSplineDrawManager::staticInitConfigControlPointsSize()
{
    return "PLBSPL_CPS";
}

QString CT_StandardPlanarBSplineDrawManager::staticInitConfigDrawPolyline()
{
    return "PLBSPL_DPL";
}


CT_ItemDrawableConfiguration CT_StandardPlanarBSplineDrawManager::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addAllConfigurationOf(SuperClass::createDrawConfiguration(drawConfigurationName));

    // Adding lines to this config dialog box
    item.addNewConfiguration(staticInitConfigDrawControlPoints(), QObject::tr("Dessiner les Points de contrôle"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigControlPointsSize(), QObject::tr("Taille des points de contrôle"), CT_ItemDrawableConfiguration::Int, 3);
    item.addNewConfiguration(staticInitConfigDrawPolyline(), QObject::tr("Dessiner la courbe B-Spline"), CT_ItemDrawableConfiguration::Bool, true);

    return item;

}
