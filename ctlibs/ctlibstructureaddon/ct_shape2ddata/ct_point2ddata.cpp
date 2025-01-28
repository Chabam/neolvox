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

#include "ct_point2ddata.h"

CT_Point2DData::CT_Point2DData() : SuperClass()
{
}

CT_Point2DData::CT_Point2DData(const Eigen::Vector2d& point) : SuperClass(point)
{
}

CT_Point2DData::CT_Point2DData(const double& x, const double& y) : SuperClass(x, y)
{
}

double CT_Point2DData::x() const
{
    return getCenter()(0);
}

double CT_Point2DData::y() const
{
    return getCenter()(1);
}

void CT_Point2DData::getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min(0) = getCenter()(0);
    min(1) = getCenter()(1);
    min(2) = 0;
    max(0) = getCenter()(0);
    max(1) = getCenter()(1);
    max(2) = 0;
}
