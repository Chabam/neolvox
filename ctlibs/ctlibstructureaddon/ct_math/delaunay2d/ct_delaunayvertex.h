/****************************************************************************

 Copyright (C) 2010-2015 the Office National des Forêts (ONF), France
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre Piboule (ONF)

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

#ifndef CT_DELAUNAYVERTEX_H
#define CT_DELAUNAYVERTEX_H

#include "ctlibmath_global.h"
#include "ct_shape2ddata/ct_polygon2ddata.h"

#include <Eigen/Core>

#include <QList>

class CT_DelaunayVoroVertex;

/**
 * DelaunayVertex - vertex of the Delaunay triangulation.
 *
 * @author A. Piboule - february 2004
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayVertex
{
public:
    CT_DelaunayVertex (Eigen::Vector3d* data, bool deleteData);

    CT_DelaunayVertex (double x, double y);

    ~CT_DelaunayVertex();


    // initialize the neighbors list
    void initNeighbors ();

    // initialize the voroVertices list
    void initVoroVertices ();

    // add a neighbor
    void addNeighbor (CT_DelaunayVertex* vt);

    // add a voroVertex to voroVertices
    void addVoroVertex (CT_DelaunayVoroVertex* vrt);

    // gives the neighbors list
    inline QList<CT_DelaunayVertex*> &getNeighbors () {return _neighbors;}

    // gives the Voronoi's polygon vertices list
    inline const QList<CT_DelaunayVoroVertex*> &getVoroVertices () {return _voroVertices;}


    // 2 DelaunayVertex are equals if their positions are identical
    bool equals (CT_DelaunayVertex* other);

    // return all vertices at a distance <= dist of this vertex
    // N.B.: uses neighbors, so these neighbors should have been computed before...
    void getVerticesWithin (double dist, QList<CT_DelaunayVertex*> &lst);

    // returns a drawable shape (a general path) of the Voronoi Polygon
    CT_Polygon2DData* getVoroShape ();

    inline double x() const {return (*_data)(0);}
    inline double y() const {return (*_data)(1);}

    inline Eigen::Vector3d* getData() const {return _data;}
    inline const Eigen::Vector3d* getDataConst() const {return _data;}


private:

    friend class CT_DelaunayTriangulation;
    friend class CT_DelaunayTriangle;
    friend class CT_DelaunayOutline;

    Eigen::Vector3d* _data;
    bool             _deleteData;

    QList<CT_DelaunayVertex*>      _neighbors;    // neighbors vertices of this vertex
    QList<CT_DelaunayVoroVertex*>  _voroVertices; // Voronoi polygon's vertices of this vertex

};


#endif // CT_DELAUNAYVERTEX_H
