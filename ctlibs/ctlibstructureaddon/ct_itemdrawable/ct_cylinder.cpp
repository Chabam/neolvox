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

#include "ct_cylinder.h"

const CT_StandardCylinderDrawManager CT_Cylinder::CYLINDER_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Cylinder)

CT_Cylinder::CT_Cylinder() : SuperClass()
{
    setBaseDrawManager(&CYLINDER_DRAW_MANAGER);
}

CT_Cylinder::CT_Cylinder(CT_CylinderData* data) : SuperClass(data)
{
    setBaseDrawManager(&CYLINDER_DRAW_MANAGER);

    const Eigen::Vector3d& center = data->getCenter();
    const double length = std::max(getRadius(), getHeight());

    setBoundingBox(center(0) - length,
                   center(1) - length,
                   center(2) - length,
                   center(0) + length,
                   center(1) + length,
                   center(2) + length);
}

double CT_Cylinder::getRadius() const
{
    return dataConstCastAs<CT_CylinderData>()->getRadius();
}

double CT_Cylinder::getHeight() const
{
    return dataConstCastAs<CT_CylinderData>()->getHeight();
}

double CT_Cylinder::getLineError() const
{
    return dataConstCastAs<CT_CylinderData>()->getLineError();
}

double CT_Cylinder::getCircleError() const
{
    return dataConstCastAs<CT_CylinderData>()->getCircleError();
}

CT_Cylinder* CT_Cylinder::staticCreate3DCylinderFromPointCloud(const CT_AbstractPointCloudIndex &pointCloudIndex,
                                                               const Eigen::Vector3d &pointCloudBarycenter)
{
    CT_CylinderData *data = CT_CylinderData::staticCreate3DCylinderDataFromPointCloud(pointCloudIndex,
                                                                                      pointCloudBarycenter);

    if(data == nullptr)
        return nullptr;

    return new CT_Cylinder(data);
}
