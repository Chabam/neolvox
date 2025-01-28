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

#include "ct_delaunayvertex.h"
#include "ct_delaunayvorovertex.h"

CT_DelaunayVertex::CT_DelaunayVertex(Eigen::Vector3d *data, bool deleteData)
{
    _data = data;
    _deleteData = deleteData;
}

CT_DelaunayVertex::CT_DelaunayVertex(double x, double y)
{
    _data = new Eigen::Vector3d(x, y, 0);
    _deleteData = true;
}

CT_DelaunayVertex::~CT_DelaunayVertex()
{
    if (_deleteData) {delete _data;}
    _neighbors.clear();

    qDeleteAll(_voroVertices);
    _voroVertices.clear();
}

void CT_DelaunayVertex::initNeighbors()
{
    _neighbors.clear();
}

void CT_DelaunayVertex::initVoroVertices()
{
    _voroVertices.clear();
}

void CT_DelaunayVertex::addNeighbor(CT_DelaunayVertex *vt)
{        if (!_neighbors.contains(vt)) {_neighbors.append(vt);}

}

void CT_DelaunayVertex::addVoroVertex(CT_DelaunayVoroVertex *vrt)
{
    if (!_voroVertices.contains(vrt)) {_voroVertices.append(vrt);}
}

bool CT_DelaunayVertex::equals(CT_DelaunayVertex *other)
{
    return (((other)->x() == x()) && ((other)->y() == y()));
}

void CT_DelaunayVertex::getVerticesWithin(double dist, QList<CT_DelaunayVertex *> &lst)
{
    CT_DelaunayVertex* n;
    int i;

    if (!_neighbors.isEmpty()) {
        // test all direct neighbors
        for (i = 0 ; i < _neighbors.size() ; i++)
        {
            n = _neighbors.at(i);
            if (sqrt(pow(x() - n->x(), 2) + pow (y() - n->y(), 2)) <= dist) {lst.append(n);}
        }

        // test neighbors of [neighbors which are within dist] and so on
        for (i = 0 ; i < lst.size() ; i++) {

            const QList<CT_DelaunayVertex*> &neigh = lst.at(i)->getNeighbors();

            for (int j = 0 ; j < neigh.size() ; j++)
            {
                n =  neigh.at(j);

                // do not add this and already added vertices, to avoid infinite loop
                if ((n != this) && (!lst.contains(n)))
                {
                    if (sqrt(pow(x() - n->x(), 2) + pow(y() - n->y(), 2)) <= dist) {lst.append(n);}
                }
            }
        }
    }
}

CT_Polygon2DData *CT_DelaunayVertex::getVoroShape()
{
    const int size = _voroVertices.size();
    QVector<Eigen::Vector2d> vertices(size);

    for (int i = 0 ; i <size ; i++)
    {
        const CT_DelaunayVoroVertex* vt = _voroVertices[i];
        vertices[i] = Eigen::Vector2d(vt->x(), vt->y());
    }

    return new CT_Polygon2DData(vertices);
}
