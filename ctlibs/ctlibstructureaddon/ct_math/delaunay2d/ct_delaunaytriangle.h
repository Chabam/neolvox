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

#ifndef CT_DELAUNAYTRIANGLE_H
#define CT_DELAUNAYTRIANGLE_H

#include "ct_shape2ddata/ct_polygon2ddata.h"
#include "ct_delaunayvertex.h"

/**
 * DelaunayTriangle - Triangle of the Delaunay triangulation.
 *
 * @author A. Piboule - february 2004
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayTriangle
{

public:
    CT_DelaunayTriangle (CT_DelaunayVertex* v1t, CT_DelaunayVertex* v2t, CT_DelaunayVertex* v3t);

    ~CT_DelaunayTriangle();

    void init (CT_DelaunayVertex* v1t, CT_DelaunayVertex* v2t, CT_DelaunayVertex* v3t);


    // set the good neighbor, in function of the two vertices given
    void setNeighbor (CT_DelaunayVertex* vt1, CT_DelaunayVertex* vt2, CT_DelaunayTriangle* ngb);

    // calculate center coordinates and radius of the triangle defined circle
    void calculateCircle ();

    // test if a point is in triangle defined circle
    bool circleContains (double xt, double yt);

    // test if the point (xt, yt) is in the triangle
    // for this purpose we cut the triangle in two part, by an horizontal (y=constant) line
    // and we test if the point is in each part (more easy because of coordinates simplfication)
    bool contains (double xt, double yt) const;

    // get the  gravity center of the triangle X coordinate
    double getBaryX ();

   // get the  gravity center of the triangle Y coordinate
    double getBaryY ();

    inline const CT_DelaunayVertex* getVertex1() const {return _v1;}
    inline const CT_DelaunayVertex* getVertex2() const {return _v2;}
    inline const CT_DelaunayVertex* getVertex3() const {return _v3;}

    // two vertices are given, get the third
    CT_DelaunayVertex* getThirdVertex (CT_DelaunayVertex* vt1, CT_DelaunayVertex* vt2);

    // idem but with a CT_DelaunayVertex[]
    CT_DelaunayVertex* getThirdVertex (const QVector<CT_DelaunayVertex*> &vt);

    // get next neighbor triangle to destination point (xt, yt)
    CT_DelaunayTriangle *getNextTriangleTo(double xt, double yt);

    // returns a drawable shape (a general path) of the triangle
    CT_Polygon2DData* getShape ();

    inline void setToRemoveFlag(bool flag) {_toRemove = flag;}
    inline bool getToRemoveFlag() const {return _toRemove;}

private:

    friend class CT_DelaunayTriangulation;
    friend class CT_DelaunayVoroVertex;

    CT_DelaunayVertex* _v1; // 3 triangle vertices
    CT_DelaunayVertex* _v2;
    CT_DelaunayVertex* _v3;

    CT_DelaunayTriangle* _n12; // neighbor triangles, each between two vertices
    CT_DelaunayTriangle* _n23;
    CT_DelaunayTriangle* _n31;

    double _ccX; // triangle defined circle center X coordinates
    double _ccY; // triangle defined circle center Y coordinates
    double _r2;   // triangle defined circle radius (from orthocenter)

    bool _toRemove;
};


#endif // CT_DELAUNAYTRIANGLE_H
