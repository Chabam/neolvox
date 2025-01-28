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

#ifndef CT_AREASHAPE2DDATA_H
#define CT_AREASHAPE2DDATA_H

#include "ct_shape2ddata.h"

/**
  * Represent data of 2D geometrical item
  */
class CTLIBSTRUCTUREADDON_EXPORT CT_AreaShape2DData : public CT_Shape2DData
{
    using SuperClass = CT_Shape2DData;

public:
    CT_AreaShape2DData() = default;
    CT_AreaShape2DData(const Eigen::Vector2d& center);
    CT_AreaShape2DData(const CT_AreaShape2DData& other) = default;

    virtual bool contains(double x, double y) const = 0;

    virtual double getArea() const = 0;

protected:
    Eigen::Vector2d _min;
    Eigen::Vector2d _max;
};

#endif // CT_AREASHAPE2DDATA_H
