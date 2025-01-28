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

#include "ct_polygon2d.h"

const CT_StandardPolygon2DDrawManager CT_Polygon2D::POLYGON2D_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Polygon2D)

CT_Polygon2D::CT_Polygon2D() : SuperClass()
{
    setBaseDrawManager(&POLYGON2D_DRAW_MANAGER);
}

CT_Polygon2D::CT_Polygon2D(CT_Polygon2DData* data) : SuperClass(data)
{
    setBaseDrawManager(&POLYGON2D_DRAW_MANAGER);
}

const QVector<Eigen::Vector2d>& CT_Polygon2D::getVertices() const
{
    return dataConstCastAs<CT_Polygon2DData>()->getVertices();
}

