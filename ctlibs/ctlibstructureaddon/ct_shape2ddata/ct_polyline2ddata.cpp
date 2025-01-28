/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#include "ct_polyline2ddata.h"

#include <math.h>
#include <limits>
#include <algorithm>

CT_Polyline2DData::CT_Polyline2DData() : SuperClass(),
    _minX(std::numeric_limits<double>::max()),
    _maxX(-std::numeric_limits<double>::max()),
    _minY(_minX),
    _maxY(_maxX)
{
}

CT_Polyline2DData::CT_Polyline2DData(const QVector<Eigen::Vector2d>& vertices) : SuperClass(),
    _vertices(vertices)
{
    _minX = std::numeric_limits<double>::max();
    _minY = std::numeric_limits<double>::max();
    _maxX = -std::numeric_limits<double>::max();
    _maxY = -std::numeric_limits<double>::max();

    const int size = _vertices.size();

    for (int i = 0 ; i < size ; i++)
    {
        const Eigen::Vector2d& dest = _vertices[i];

        if (dest(0) < _minX) {_minX = dest(0);}
        if (dest(0) > _maxX) {_maxX = dest(0);}
        if (dest(1) < _minY) {_minY = dest(1);}
        if (dest(1) > _maxY) {_maxY = dest(1);}
    }

    _center(0) = (_maxX + _minX) / 2.0;
    _center(1) = (_maxY + _minY) / 2.0;
}

void CT_Polyline2DData::getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min(0) = _minX;
    min(1) = _minY;
    min(2) = 0;
    max(0) = _maxX;
    max(1) = _maxY;
    max(2) = 0;
}
