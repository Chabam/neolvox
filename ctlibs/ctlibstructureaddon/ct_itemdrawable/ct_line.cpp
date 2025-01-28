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

#include "ct_line.h"

const CT_StandardLineDrawManager CT_Line::LINE_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Line)

CT_Line::CT_Line() : SuperClass()
{
    setBaseDrawManager(&LINE_DRAW_MANAGER);
}

CT_Line::CT_Line(CT_LineData* data) : SuperClass(data)
{
    setBaseDrawManager(&LINE_DRAW_MANAGER);

    setBoundingBox(qMin(data->x1(), data->x2()),
                   qMin(data->y1(), data->y2()),
                   qMin(data->z1(), data->z2()),
                   qMax(data->x1(), data->x2()),
                   qMax(data->y1(), data->y2()),
                   qMax(data->z1(), data->z2()));
}

CT_Line* CT_Line::staticCreateLineFromPointCloud(const CT_AbstractPointCloudIndex &pointCloudIndex)
{

    CT_LineData* data = CT_LineData::staticCreateLineDataFromPointCloud(pointCloudIndex);

    if(data == nullptr)
        return nullptr;

    return new CT_Line(data);
}
