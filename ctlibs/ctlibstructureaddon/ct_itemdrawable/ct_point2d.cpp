/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#include "ct_point2d.h"
#define _USE_MATH_DEFINES
#include "math.h"

const CT_StandardPoint2DDrawManager CT_Point2D::POINT2D_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_Point2D)

CT_Point2D::CT_Point2D() : SuperClass()
{
    setBaseDrawManager(&POINT2D_DRAW_MANAGER);
}

CT_Point2D::CT_Point2D(CT_Point2DData *data) : SuperClass(data)
{
    setBaseDrawManager(&POINT2D_DRAW_MANAGER);
}

double CT_Point2D::x() const
{
    return ((const CT_Point2DData&)getData()).x();
}

double CT_Point2D::y() const
{
    return ((const CT_Point2DData&)getData()).y();
}
