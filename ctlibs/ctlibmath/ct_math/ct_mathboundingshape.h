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

#ifndef CT_MATHBOUNDINGSHAPE_H
#define CT_MATHBOUNDINGSHAPE_H

#include "ctlibmath_global.h"

#include <Eigen/Core>

class CTLIBMATH_EXPORT CT_MathBoundingShape
{
public:

    static bool aabbIntersects(const Eigen::Vector3d &min1, const Eigen::Vector3d &max1, const Eigen::Vector3d &min2, const Eigen::Vector3d &max2);
    static bool aabbIntersects2D(const Eigen::Vector3d &min1, const Eigen::Vector3d &max1, const Eigen::Vector3d &min2, const Eigen::Vector3d &max2);
    static bool containsPoint(const Eigen::Vector3d &min, const Eigen::Vector3d &max, const Eigen::Vector3d &point);
    static bool containsPointIn2D(const Eigen::Vector3d &min, const Eigen::Vector3d &max, const Eigen::Vector3d &point);

};

#endif // CT_MATHBOUNDINGSHAPE_H
