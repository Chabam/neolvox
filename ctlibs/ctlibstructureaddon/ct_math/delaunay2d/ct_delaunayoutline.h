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

#ifndef CT_DELAUNAYOUTLINE_H
#define CT_DELAUNAYOUTLINE_H

#include "ct_shape2ddata/ct_polygon2ddata.h"
#include "ct_delaunayvertex.h"
#include "ct_itemdrawable/ct_image2d.h"

#include <QList>

#include <stdlib.h>     /* srand, rand */


/**
 * DelaunayOutline - Outline of a Delaunay triangulation.
 *
 * @author A. Piboule - february 2004
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayOutline
{

public:
    CT_DelaunayOutline ();

    ~CT_DelaunayOutline ();


    void addVertex (CT_DelaunayVertex* vt);

    inline int getN () {return _vertices.size ();}

    const QList<CT_DelaunayVertex*> &getVertices () const {return _vertices;}

    double area ();

    bool contains (double x, double y, bool optimize = true) const;

    void clear();

    // returns a drawable shape (a general path) of the outline Polygon
    CT_Polygon2DData* getShape ();

    void computeOptimizationGrid();

private:
    QList<CT_DelaunayVertex*> _vertices;    
    CT_Image2D<char>*         _optimizationGrid;


};


#endif // CT_DELAUNAYOUTLINE_H
