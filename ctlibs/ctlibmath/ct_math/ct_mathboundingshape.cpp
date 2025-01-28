/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_mathboundingshape.h"
#include "ct_math/ct_mathpoint.h"

bool CT_MathBoundingShape::aabbIntersects(const Eigen::Vector3d &min1, const Eigen::Vector3d &max1, const Eigen::Vector3d &min2, const Eigen::Vector3d &max2)
{
    if (max1(0) < min2(0)) {return false;}
    if (max2(0) < min1(0)) {return false;}
    if (max1(1) < min2(1)) {return false;}
    if (max2(1) < min1(1)) {return false;}
    if (max1(2) < min2(2)) {return false;}
    if (max2(2) < min1(2)) {return false;}
    return true;
}

bool CT_MathBoundingShape::aabbIntersects2D(const Eigen::Vector3d &min1, const Eigen::Vector3d &max1, const Eigen::Vector3d &min2, const Eigen::Vector3d &max2)
{     
    if (max1(0) < min2(0)) {return false;}
    if (max2(0) < min1(0)) {return false;}
    if (max1(1) < min2(1)) {return false;}
    if (max2(1) < min1(1)) {return false;}
    return true;
}

bool CT_MathBoundingShape::containsPoint(const Eigen::Vector3d &min, const Eigen::Vector3d &max, const Eigen::Vector3d& point)
{
    if (point(0) < min(0)) {return false;}
    if (point(0) > max(0)) {return false;}
    if (point(1) < min(1)) {return false;}
    if (point(1) > max(1)) {return false;}
    if (point(2) < min(2)) {return false;}
    if (point(2) > max(2)) {return false;}
    return true;
}

bool CT_MathBoundingShape::containsPointIn2D(const Eigen::Vector3d &min, const Eigen::Vector3d &max, const Eigen::Vector3d& point)
{
    if (point(0) < min(0)) {return false;}
    if (point(0) > max(0)) {return false;}
    if (point(1) < min(1)) {return false;}
    if (point(1) > max(1)) {return false;}
    return true;
}

