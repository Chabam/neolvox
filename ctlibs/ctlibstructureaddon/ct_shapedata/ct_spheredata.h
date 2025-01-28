/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël Krebs (ENSAM / ARTS)

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

#ifndef CT_SPHEREDATA_H
#define CT_SPHEREDATA_H

#include "ct_shapedata.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_SphereData : public CT_ShapeData
{
    using SuperClass = CT_ShapeData;

public:
    CT_SphereData();
    CT_SphereData(const Eigen::Vector3d& center,
                  double radius);
    CT_SphereData(const CT_SphereData& other) = default;

    double getRadius() const;

    virtual void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const override;

    CT_SHAPEDATA_CLONE_IMP(CT_SphereData)

private:
    double            m_radius;
};

#endif // CT_SPHEREDATA_H
