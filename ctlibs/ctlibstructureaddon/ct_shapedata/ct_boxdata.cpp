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

#include "ct_boxdata.h"

CT_BoxData::CT_BoxData() : SuperClass(),
    _width(0),
    _height(0),
    _depth(0)
{
}

CT_BoxData::CT_BoxData(const Eigen::Vector3d& center,
                       const Eigen::Vector3d& direction,
                       const Eigen::Vector3d& widthDirection,
                       double height,
                       double width,
                       double depth) : SuperClass(center, direction),
    _widthDirection(widthDirection),
    _width(width),
    _height(height),
    _depth(depth)
{
}

const Eigen::Vector3d& CT_BoxData::getWidthDirection() const
{
    return _widthDirection;
}

float CT_BoxData::getHeight() const
{
    return _height;
}

float CT_BoxData::getWidth() const
{
    return _width;
}

float CT_BoxData::getDepth() const
{
    return _depth;
}

void CT_BoxData::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    double dist = std::max(_width, _height);

    min(0) = _center(0) - dist;
    min(1) = _center(1) - dist;
    min(2) = _center(2) - dist;

    max(0) = _center(0) + dist;
    max(1) = _center(1) + dist;
    max(2) = _center(2) + dist;
}
