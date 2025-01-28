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

#include "ct_delaunaytrianglesrecycler.h"


CT_DelaunayTriangleRecycler::CT_DelaunayTriangleRecycler()
{
}

CT_DelaunayTriangleRecycler::~CT_DelaunayTriangleRecycler()
{
    clear();
}

void CT_DelaunayTriangleRecycler::addTriangle(CT_DelaunayTriangle *triangle)
{
    _tri.append(triangle);
}

void CT_DelaunayTriangleRecycler::addTriangles(QList<CT_DelaunayTriangle *> &triangles)
{
    _tri.append(triangles);
}

void CT_DelaunayTriangleRecycler::clearAndDelete()
{
    qDeleteAll(_tri);
    _tri.clear();
}

void CT_DelaunayTriangleRecycler::clear()
{
    _tri.clear();
}

CT_DelaunayTriangle* CT_DelaunayTriangleRecycler::getTriangle(CT_DelaunayVertex* v1t, CT_DelaunayVertex* v2t, CT_DelaunayVertex* v3t, bool &isnew)
{
    if (_tri.size() > 0)
    {
        isnew = false;
        CT_DelaunayTriangle* triangle = _tri.takeLast();
        triangle->init(v1t, v2t, v3t);
        return triangle;
    }

    isnew = true;
    return new CT_DelaunayTriangle(v1t, v2t, v3t);
}

