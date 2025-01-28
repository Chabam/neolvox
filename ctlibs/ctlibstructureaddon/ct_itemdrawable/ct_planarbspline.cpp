/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

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

#include "ct_planarbspline.h"

const CT_StandardPlanarBSplineDrawManager CT_PlanarBSpline::PLANARBSPLINE_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_PlanarBSpline)

CT_PlanarBSpline::CT_PlanarBSpline() : SuperClass()
{
    setBaseDrawManager(&PLANARBSPLINE_DRAW_MANAGER);
}

CT_PlanarBSpline::CT_PlanarBSpline(CT_PlanarBSplineData *data) : SuperClass(data)
{
    setBaseDrawManager(&PLANARBSPLINE_DRAW_MANAGER);
}

int CT_PlanarBSpline::getDegree() const
{
    return ((const CT_PlanarBSplineData&)getData()).degree();
}

int CT_PlanarBSpline::getNCP() const
{
    return ((const CT_PlanarBSplineData&)getData()).nCP();
}

void CT_PlanarBSpline::setPolyline(const QList<Eigen::Vector3d> &list)
{
    _polyline << list;
}

const QList<Eigen::Vector3d> &CT_PlanarBSpline::getPolyline() const
{
    return _polyline;
}

