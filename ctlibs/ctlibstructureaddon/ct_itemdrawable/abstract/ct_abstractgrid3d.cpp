/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Metiers (ENSAM), Cluny, France.
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

#include "ct_abstractgrid3d.h"

CT_TYPE_IMPL_INIT_MACRO(CT_AbstractGrid3D)

CT_AbstractGrid3D::CT_AbstractGrid3D() : SuperClass()
{
    _res = 1;
    _dimx = 0;
    _dimy = 0;
    _dimz = 0;
}

CT_AbstractGrid3D::CT_AbstractGrid3D(int dimx, int dimy, int dimz, double resolution)
{
    _res = resolution;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;
}

CT_AbstractGrid3D::CT_AbstractGrid3D(double xmin, double ymin, double zmin, int dimx, int dimy, int dimz, double resolution)
{
    _res = resolution;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;

    setBoundingBox(xmin, ymin, zmin,
                   xmin + resolution * dimx, ymin + resolution * dimy, zmin + resolution * dimz);
}

CT_AbstractGrid3D::CT_AbstractGrid3D(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, double resolution)
{
    _res = resolution;
    _dimx = int(ceil((xmax - xmin)/resolution));
    _dimy = int(ceil((ymax - ymin)/resolution));
    _dimz = int(ceil((zmax - zmin)/resolution));

    Eigen::Vector3d maxCoordinates(xmin + resolution * _dimx,
                                   ymin + resolution * _dimy,
                                   zmin + resolution * _dimz);

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= maxCoordinates(0))
    {
        _dimx++;
        maxCoordinates(0) = (maxCoordinates(0) + resolution);
    }

    while (ymax >= maxCoordinates(1))
    {
        _dimy++;
        maxCoordinates(1) = (maxCoordinates(1) + resolution);
    }

    while (zmax >= maxCoordinates(2))
    {
        _dimz++;
        maxCoordinates(2) = (maxCoordinates(2) + resolution);
    }

    setBoundingBox(xmin, ymin, zmin,
                   maxCoordinates(0), maxCoordinates(1), maxCoordinates(2));
}
