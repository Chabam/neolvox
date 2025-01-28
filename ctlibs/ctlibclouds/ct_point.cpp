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

#include "ct_point.h"
#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"

CT_AbstractCoordinateSystem::realEx CT_PointTools::staticXExport(const CT_PointData &p, const size_t &globalIndex)
{
    CT_AbstractCoordinateSystem::realEx out;
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), out, out, out, CT_AbstractCoordinateSystem::CONVERT_X);

    return out;
}

CT_AbstractCoordinateSystem::realEx CT_PointTools::staticYExport(const CT_PointData &p, const size_t &globalIndex)
{
    CT_AbstractCoordinateSystem::realEx out;
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), out, out, out, CT_AbstractCoordinateSystem::CONVERT_Y);

    return out;
}

CT_AbstractCoordinateSystem::realEx CT_PointTools::staticZExport(const CT_PointData &p, const size_t &globalIndex)
{
    CT_AbstractCoordinateSystem::realEx out;
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), out, out, out, CT_AbstractCoordinateSystem::CONVERT_Z);

    return out;
}

void CT_PointTools::staticXYZExport(const CT_PointData &p, const size_t &globalIndex, CT_AbstractCoordinateSystem::realEx &x, CT_AbstractCoordinateSystem::realEx &y, CT_AbstractCoordinateSystem::realEx &z)
{
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), x, y, z, CT_AbstractCoordinateSystem::CONVERT_ALL);
}

CT_Point CT_PointTools::staticRealPoint(const CT_PointData &p, const size_t &globalIndex)
{
    CT_Point cp;
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), cp(0), cp(1), cp(2), CT_AbstractCoordinateSystem::CONVERT_ALL);

    return cp;
}

void CT_PointTools::staticRealPoint(const CT_PointData &p, CT_Point &cp, const size_t &globalIndex)
{
    PS_COORDINATES_SYS_MANAGER->coordinateSystemForPointAt(globalIndex)->convertExport(p(0), p(1), p(2), cp(0), cp(1), cp(2), CT_AbstractCoordinateSystem::CONVERT_ALL);
}
