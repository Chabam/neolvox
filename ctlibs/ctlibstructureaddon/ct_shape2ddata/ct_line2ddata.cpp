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

#include "ct_line2ddata.h"

#include "ct_math/ct_mathpoint.h"
#include "ct_math/ct_math2dlines.h"
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>


CT_Line2DData::CT_Line2DData() : SuperClass()
{
}

CT_Line2DData::CT_Line2DData(const Eigen::Vector2d& p1, const Eigen::Vector2d& p2) : SuperClass((p1 + p2) / 2.0),
    _p1(p1),
    _p2(p2)
{
}

const Eigen::Vector2d& CT_Line2DData::getP1() const
{
    return _p1;
}

const Eigen::Vector2d& CT_Line2DData::getP2() const
{
    return _p2;
}

double CT_Line2DData::x1() const
{
    return _p1(0);
}

double CT_Line2DData::y1() const
{
    return _p1(1);
}

double CT_Line2DData::x2() const
{
    return _p2(0);
}

double CT_Line2DData::y2() const
{
    return _p2(1);
}

double CT_Line2DData::length() const
{
    return CT_Math2DLines::distance2D(_p1, _p2);
}

void CT_Line2DData::getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min(0) = std::min(_p1(0), _p2(0));
    min(1) = std::min(_p1(1), _p2(1));
    min(2) = 0;
    max(0) = std::max(_p1(0), _p2(0));
    max(1) = std::max(_p1(1), _p2(1));
    max(2) = 0;
}
