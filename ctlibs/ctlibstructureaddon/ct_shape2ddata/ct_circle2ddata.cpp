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

#include "ct_circle2ddata.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include "ct_math/ct_mathpoint.h"

CT_Circle2DData::CT_Circle2DData() : SuperClass(),
    _radius(0)
{
}

CT_Circle2DData::CT_Circle2DData(const Eigen::Vector2d& center, double radius) : SuperClass(center),
    _radius(radius)
{
    _min(0) = center(0) - radius;
    _min(1) = center(1) - radius;
    _max(0) = center(0) + radius;
    _max(1) = center(1) + radius;
}

void CT_Circle2DData::setRadius(double radius)
{
    _radius = radius;

    _min(0) = _center(0) - radius;
    _min(1) = _center(1) - radius;
    _max(0) = _center(0) + radius;
    _max(1) = _center(1) + radius;
}

double CT_Circle2DData::getRadius() const
{
    return _radius;
}

void CT_Circle2DData::getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min(0) = _min(0);
    min(1) = _min(1);
    min(2) = 0;
    max(0) = _max(0);
    max(1) = _max(1);
    max(2) = 0;
}

bool CT_Circle2DData::contains(double x, double y) const
{
    if (x < _min(0)) {return false;}
    if (y < _min(1)) {return false;}
    if (x > _max(0)) {return false;}
    if (y > _max(1)) {return false;}

    double distance = sqrt(pow(_center(0) - x, 2) + pow(_center(1) - y, 2));
    return (distance <= _radius);
}

double CT_Circle2DData::getArea() const
{
    return M_PI*_radius*_radius;
}
