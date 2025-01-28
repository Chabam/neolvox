/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre PIBOULE (ONF)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_POLYGON2DDATA_H
#define CT_POLYGON2DDATA_H

#include "ct_areashape2ddata.h"
#include "ct_pointcloudindex/ct_pointcloudindexvector.h"
#include "painterinterface.h"

#include <QVector>
#include <QStack>

class CTLIBSTRUCTUREADDON_EXPORT CT_Polygon2DData : public CT_AreaShape2DData
{
    using SuperClass = CT_AreaShape2DData;

public:
    CT_Polygon2DData();
    CT_Polygon2DData(const QVector<Eigen::Vector2d>& vertices);
    CT_Polygon2DData(const QVector<Eigen::Vector3d>& vertices);
    CT_Polygon2DData(const CT_Polygon2DData& other) = default;

    void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const override;

    void computeCentroid();

    inline const QVector<Eigen::Vector2d>& getVertices() const {return _vertices;}

    double getAreaIfNotSelfIntersecting();
    double getSignedArea();
    double getPerimeter() const;
    double getMaxLengthBetweenTwoVertices() const;
    inline int getVerticesNumber() const {return _vertices.size();}

    virtual double getArea() const override;

    bool contains(double x, double y) const override;

    void draw(PainterInterface& painter, bool drawPoints, bool drawLines, double zPlane) const;

    static void orderPointsByXY(QList<Eigen::Vector2d>& pointList);
    static void orderPointsByXY(QList<Eigen::Vector2d*>& pointList);
    static void orderPointsByXY(QList<Eigen::Vector3d>& pointList);
    static void orderPointsByXY(QList<Eigen::Vector3d*>& pointList);


    // Use Andrew's monotone chain convex hull algorithm
    static CT_Polygon2DData* createConvexHull(QList<Eigen::Vector2d>& orderedCandidates);
    static CT_Polygon2DData* createConvexHull(QList<Eigen::Vector2d*>& orderedCandidates);
    static CT_Polygon2DData* createConvexHull(QList<Eigen::Vector3d>& orderedCandidates);
    static CT_Polygon2DData* createConvexHull(QList<Eigen::Vector3d*>& orderedCandidates);

    static CT_Polygon2DData* chainHull_2D(QList<Eigen::Vector2d>& orderedCandidates);

    CT_SHAPEDATA2D_CLONE_IMP(CT_Polygon2DData)

private:
    QVector<Eigen::Vector2d>    _vertices;

    double                      _area;
    double                      _areaComputed;

    friend class CT_Polygon2D;

    static double cross(const Eigen::Vector2d& O, const Eigen::Vector2d& A, const Eigen::Vector2d& B);
    static double cross(const Eigen::Vector3d& O, const Eigen::Vector3d& A, const Eigen::Vector3d& B);

    static bool compareV2d(const Eigen::Vector2d& p1, const Eigen::Vector2d& p2);
    static bool compareV2dptr(const Eigen::Vector2d* p1, const Eigen::Vector2d* p2);
    static bool compareV3d(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2);
    static bool compareV3dptr(const Eigen::Vector3d* p1, const Eigen::Vector3d* p2);
};

#endif // CT_POLYGON2DDATA_H
