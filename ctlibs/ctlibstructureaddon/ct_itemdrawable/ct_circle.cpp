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

#include "ct_circle.h"
#define _USE_MATH_DEFINES
#include "math.h"

const CT_StandardCircleDrawManager CT_Circle::CIRCLE_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Circle)

CT_Circle::CT_Circle() : SuperClass()
{
    setBaseDrawManager(&CIRCLE_DRAW_MANAGER);
}

CT_Circle::CT_Circle(CT_CircleData* data) : SuperClass(data)
{   
    setBaseDrawManager(&CIRCLE_DRAW_MANAGER);
}

double CT_Circle::getRadius() const
{
    return dataConstCastAs<CT_CircleData>()->getRadius();
}

double CT_Circle::getError() const
{
    return dataConstCastAs<CT_CircleData>()->getError();
}

CT_Circle* CT_Circle::staticCreateZAxisAlignedCircleFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                   double z)
{
    CT_CircleData* data = CT_CircleData::staticCreateZAxisAlignedCircleDataFromPointCloud(pointCloudIndex, z);

    if(data == nullptr)
        return nullptr;

    return new CT_Circle(data);
}
