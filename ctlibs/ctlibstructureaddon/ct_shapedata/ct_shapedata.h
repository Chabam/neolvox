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

#ifndef CT_SHAPEDATA_H
#define CT_SHAPEDATA_H

#include "ctlibstructureaddon_global.h"

#include <Eigen/Core>

#define CT_SHAPEDATA_CLONE_IMP(argClass) CT_ShapeData* copy() const override { return clone(); } \
                                         argClass* clone() const { return new argClass(*this); }

/**
  * Represent data of geometrical item
  */
class CTLIBSTRUCTUREADDON_EXPORT CT_ShapeData
{
public:
    CT_ShapeData() = default;
    CT_ShapeData(const Eigen::Vector3d& center, const Eigen::Vector3d& direction);
    CT_ShapeData(const CT_ShapeData& other) = default;
    virtual ~CT_ShapeData() = 0;

    const Eigen::Vector3d& getCenter() const;
    const Eigen::Vector3d& getDirection() const;

    void setCenterX(const double& x);
    void setCenterY(const double& y);
    void setCenterZ(const double& z);
    void setCenter(const Eigen::Vector3d& center);
    void setDirection(const Eigen::Vector3d& direction);

    virtual void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const = 0;

    virtual CT_ShapeData* copy() const = 0;

protected:
    Eigen::Vector3d   _center;
    Eigen::Vector3d   _direction;
};

#endif // CT_SHAPEDATA_H
