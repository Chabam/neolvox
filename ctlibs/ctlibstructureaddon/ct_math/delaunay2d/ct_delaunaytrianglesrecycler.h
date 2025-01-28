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

#ifndef CT_DELAUNAYTRIANGLESRECYCLER_H
#define CT_DELAUNAYTRIANGLESRECYCLER_H

#include "ct_delaunaytriangle.h"
#include "ct_delaunayvertex.h"


/**
 * DelaunaySide - side of the Delaunay triangulation.
 *
 * @author A. Piboule - february 2004 / modified october 2016
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayTriangleRecycler
{

public:
    CT_DelaunayTriangleRecycler ();

    ~CT_DelaunayTriangleRecycler();

    void addTriangle(CT_DelaunayTriangle* triangle);
    void addTriangles(QList<CT_DelaunayTriangle*> &triangles);

    void clearAndDelete();
    void clear();

    CT_DelaunayTriangle *getTriangle(CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t, CT_DelaunayVertex *v3t, bool &isnew);
private:
    friend class CT_DelaunayTriangulation;

    QList<CT_DelaunayTriangle*>   _tri;

};



#endif // CT_DELAUNAYTRIANGLESRECYCLER_H
