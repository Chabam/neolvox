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

#include "ct_shape2ddata.h"

CT_Shape2DData::CT_Shape2DData(const Eigen::Vector2d& center)
{
    _center = center;
}

CT_Shape2DData::CT_Shape2DData(const double& x, const double& y)
{
    _center(0) = x;
    _center(1) = y;
}

const Eigen::Vector2d& CT_Shape2DData::getCenter() const
{
    return _center;
}

void CT_Shape2DData::setCenterX(const double& x)
{
    _center(0) = x;
}

void CT_Shape2DData::setCenterY(const double& y)
{
    _center(1) = y;
}

void CT_Shape2DData::setCenter(const Eigen::Vector2d& center)
{
    _center(0) = center(0);
    _center(1) = center(1);
}

void CT_Shape2DData::setCenter(const Eigen::Vector3d& center)
{
    _center(0) = center(0);
    _center(1) = center(1);
}

bool CT_Shape2DData::doesBBIntersect2D(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    Eigen::Vector3d minBB, maxBB;
    getBoundingBox(minBB, maxBB);

    if (maxBB(0) < min(0)) {return false;}
    if (maxBB(1) < min(1)) {return false;}
    if (minBB(0) > max(0)) {return false;}
    if (minBB(1) > max(1)) {return false;}

    return true;
}

