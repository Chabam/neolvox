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

#ifndef CT_DELAUNAYVOROVERTEX_H
#define CT_DELAUNAYVOROVERTEX_H

#include "ctlibmath_global.h"
#include "ct_delaunayvertex.h"
#include "ct_delaunaytriangle.h"


/**
 * DelaunayVoroVertex - side of a Voronoi Polygon.
 *
 * @author A. Piboule - february 2004
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayVoroVertex
{

public:
    CT_DelaunayVoroVertex (const CT_DelaunayTriangle *trit, CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t);

    ~CT_DelaunayVoroVertex();

    // give the other vertex of the side
    CT_DelaunayVertex *next(const CT_DelaunayVertex *vt);

    inline double x() const {return _x;}
    inline double y() const {return _y;}

private:
    friend class CT_DelaunayVertex;
    friend class CT_DelaunayTriangulation;

    CT_DelaunayVertex* _v1;
    CT_DelaunayVertex* _v2;

    double _x;
    double _y;

};


#endif // CT_DELAUNAYVOROVERTEX_H
