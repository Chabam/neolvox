#include "ct_itemdrawable/tools/drawmanager/ct_standardprofiledrawmanager.h"
#include "ct_itemdrawable/ct_profile.h"
#include "painterinterface.h"
#include <typeinfo>
#include <QObject>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_COLUMN_MODE_ENABLED = CT_StandardProfileDrawManager<DataT>::staticInitConfigColumnModeEnabled();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_COLUMN_RADIUS = CT_StandardProfileDrawManager<DataT>::staticInitConfigColumnRadius();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_GRAPH_MODE_ENABLED = CT_StandardProfileDrawManager<DataT>::staticInitConfigGraphModeEnabled();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_DRAW_AXIS = CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawAxis();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_DRAW_BARS = CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawBars();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_DRAW_POINTS = CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawPoints();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_POINTS_SIZE = CT_StandardProfileDrawManager<DataT>::staticInitConfigPointsSize();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_DRAW_CURVE = CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawCurve();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_GRAPH_ORIENTATION = CT_StandardProfileDrawManager<DataT>::staticInitConfigGraphOrientation();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_SCALE = CT_StandardProfileDrawManager<DataT>::staticInitConfigScale();
template< typename DataT > const QString CT_StandardProfileDrawManager<DataT>::INDEX_CONFIG_SCALE8COEFFICIENT = CT_StandardProfileDrawManager<DataT>::staticInitConfigScaleCoeff();

template< typename DataT >
CT_StandardProfileDrawManager<DataT>::CT_StandardProfileDrawManager(QString drawConfigurationName)
    : SuperClass(drawConfigurationName.isEmpty() ? CT_Profile<DataT>::staticName() : drawConfigurationName)
{
    
}

template< typename DataT >
void CT_StandardProfileDrawManager<DataT>::draw(GraphicsViewInterface &view, PainterInterface &painter, const CT_AbstractItemDrawable &itemDrawable) const
{
    SuperClass::draw(view, painter, itemDrawable);

    const CT_Profile<DataT> &item = static_cast<const CT_Profile<DataT>&>(itemDrawable);

    // Getting the configuration values
    bool    columnMode = drawConfiguration()->variableValue(INDEX_CONFIG_COLUMN_MODE_ENABLED).toBool();
    double  radius = drawConfiguration()->variableValue(INDEX_CONFIG_COLUMN_RADIUS).toDouble();
    bool    graphMode = drawConfiguration()->variableValue(INDEX_CONFIG_GRAPH_MODE_ENABLED).toBool();
    bool    drawAxis = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_AXIS).toBool();
    bool    drawBars = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_BARS).toBool();
    bool    drawPoints = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_POINTS).toBool();
    int     pointsSize = drawConfiguration()->variableValue(INDEX_CONFIG_POINTS_SIZE).toInt();
    bool    drawCurve = drawConfiguration()->variableValue(INDEX_CONFIG_DRAW_CURVE).toBool();
    int     orientation = drawConfiguration()->variableValue(INDEX_CONFIG_GRAPH_ORIENTATION).toInt();
    bool    scale = drawConfiguration()->variableValue(INDEX_CONFIG_SCALE).toBool();
    double  scaleCoeff = drawConfiguration()->variableValue(INDEX_CONFIG_SCALE8COEFFICIENT).toDouble();

    if (radius <= 0) {radius = 0.50;}
    if (orientation < 0) {orientation = 0;}
    if (scaleCoeff <= 0) {scaleCoeff = 0.1;}
    if (orientation > 360) {orientation = 360;}

    double amplitude = item.dataMax() - item.dataMin();
    double scaling = 2*scaleCoeff / amplitude;
    double orientationRad = M_PI * (orientation % 360) / 180.0;
    const Eigen::Vector3d &direction = item.getDirection();

    double lastX = 0;
    double lastY = 0;
    double lastZ = 0;
    bool lastNa = false;

    for (size_t index = 0 ; index < item.nCells() ; index++)
    {
        bool isNa = (item.valueAtIndex(index) == item.NA());
        double value = item.valueAtIndexAsDouble(index);

        Eigen::Vector3d center;

        if (item.getCellCenterXYZ(index, center))
        {
            const QColor color = painter.getColor();
            if (columnMode)
            {
                if (isNa)
                {
                    painter.setColor(QColor(125, 0, 0));
                }  else {
                    int colorLevel = ((value - static_cast<double>(item.dataMin())) / amplitude)*255;
                    painter.setColor(QColor(colorLevel, colorLevel, colorLevel));
                }

                painter.drawPoint(center(0), center(1), center(2));
                Eigen::Vector3d cc(center.x(), center.y(), center.z());
                Eigen::Vector3d dd(direction.x(), direction.y(), direction.z());

                painter.drawCylinder3D(cc, dd, radius, item.resolution());
            }
            painter.setColor(color);

            if (graphMode)
            {
                if (scale) {value = (value - static_cast<double>(item.dataMin())) * scaling;}

                Eigen::Vector3d normalVector;
                if ((direction(0) >= direction(1)) && (direction(0) >= direction(2)))
                {
                    normalVector(0) = 0; // = 0
                    normalVector(1) = cos(orientationRad);
                    normalVector(2) = -sin(orientationRad);
                } else if ((direction(1) >= direction(0)) && (direction(1) >= direction(2))){
                    normalVector(0) = sin(orientationRad);
                    normalVector(1) = 0; // = 0
                    normalVector(2) = cos(orientationRad);
                } else {
                    normalVector(0) = cos(orientationRad);
                    normalVector(1) = -sin(orientationRad);
                    normalVector(2) = 0; // = 0
                }
                normalVector.normalize();

                if (isNa)
                {
                    value = 0;
                    painter.setColor(QColor(125, 0, 0));
                }

                if (drawBars)
                {
                    Eigen::Vector3d pt1, pt2, pt3, pt4;

                    if (item.getCellCoordinates(index, pt1, pt2))
                    {
                        pt3 = pt2 + normalVector*value;
                        pt4 = pt1 + normalVector*value;

                        painter.drawLine(pt1(0), pt1(1), pt1(2), pt2(0), pt2(1), pt2(2));
                        painter.drawLine(pt2(0), pt2(1), pt2(2), pt3(0), pt3(1), pt3(2));
                        painter.drawLine(pt3(0), pt3(1), pt3(2), pt4(0), pt4(1), pt4(2));
                        painter.drawLine(pt4(0), pt4(1), pt4(2), pt1(0), pt1(1), pt1(2));
                    }
                }

                if (drawPoints || drawCurve)
                {
                    double x = center(0) + normalVector(0)*value;
                    double y = center(1) + normalVector(1)*value;
                    double z = center(2) + normalVector(2)*value;

                    if (drawCurve && index > 0)
                    {
                        if (lastNa) {painter.setColor(QColor(125, 0, 0));}
                        painter.drawLine(lastX, lastY, lastZ, x, y, z);
                    }

                    if (drawPoints && index > 0)
                    {
                        painter.setPointSize(pointsSize);
                        painter.enableSetPointSize(false);
                        painter.drawPoint(x, y, z);
                        painter.enableSetPointSize(true);
                        painter.restoreDefaultPointSize();
                    }

                    lastX = x;
                    lastY = y;
                    lastZ = z;
                    lastNa = isNa;
                }
                painter.setColor(color);
            }
        }
    }

    if (graphMode && drawAxis)
    {
        painter.drawLine(item.minX(), item.minY(), item.minZ(), item.maxX(), item.maxY(), item.maxZ());
    }
}

template< typename DataT >
CT_ItemDrawableConfiguration CT_StandardProfileDrawManager<DataT>::createDrawConfiguration(QString drawConfigurationName) const
{
    CT_ItemDrawableConfiguration item(drawConfigurationName);

    item.addNewConfiguration(staticInitConfigColumnModeEnabled(), QObject::tr("Mode colonne"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigColumnRadius(), QObject::tr("Rayon de colonne"), CT_ItemDrawableConfiguration::Double, 0.50);
    item.addNewConfiguration(staticInitConfigGraphModeEnabled(), QObject::tr("Mode graph"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigDrawAxis(), QObject::tr("Dessiner axe"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigDrawBars(), QObject::tr("Dessiner barres"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigDrawPoints(), QObject::tr("Dessiner points"), CT_ItemDrawableConfiguration::Bool, false);
    item.addNewConfiguration(staticInitConfigPointsSize(), QObject::tr("Taille des points"), CT_ItemDrawableConfiguration::Int, 5);
    item.addNewConfiguration(staticInitConfigDrawCurve(), QObject::tr("Dessiner courbe"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigGraphOrientation(), QObject::tr("Orientation graph"), CT_ItemDrawableConfiguration::Int, 0);
    item.addNewConfiguration(staticInitConfigScale(), QObject::tr("Mise à l'échelle"), CT_ItemDrawableConfiguration::Bool, true);
    item.addNewConfiguration(staticInitConfigScaleCoeff(), QObject::tr("Coefficient d'échelle (m)"), CT_ItemDrawableConfiguration::Double, 5);

    return item;
}

// PROTECTED //

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigColumnModeEnabled()
{
    return "PROF_COL";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigColumnRadius()
{
    return "PROF_RAD";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigGraphModeEnabled()
{
    return "PROF_GRA";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawAxis()
{
    return "PROF_AXI";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawBars()
{
    return "PROF_BAR";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawPoints()
{
    return "PROF_PTS";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigPointsSize()
{
    return "PROF_SZP";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigDrawCurve()
{
    return "PROF_CUR";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigGraphOrientation()
{
    return "PROF_ORI";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigScale()
{
    return "PROF_SCA";
}

template< typename DataT >
QString CT_StandardProfileDrawManager<DataT>::staticInitConfigScaleCoeff()
{
    return "PROF_COE";
}
