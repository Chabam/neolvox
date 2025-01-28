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

#include "lvox3_abstractgrid3d.h"

CT_TYPE_IMPL_INIT_MACRO(LVOX3_AbstractGrid3D)

LVOX3_AbstractGrid3D::LVOX3_AbstractGrid3D() : CT_AbstractItemDrawableWithoutPointCloud()
{
}

LVOX3_AbstractGrid3D::LVOX3_AbstractGrid3D(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result) : CT_AbstractItemDrawableWithoutPointCloud()
{
    Q_UNUSED(model)
    Q_UNUSED(result)
}

LVOX3_AbstractGrid3D::LVOX3_AbstractGrid3D(const QString &modelName, const CT_AbstractResult *result) : CT_AbstractItemDrawableWithoutPointCloud()
{
    Q_UNUSED(modelName)
    Q_UNUSED(result)
}

LVOX3_AbstractGrid3D::~LVOX3_AbstractGrid3D()
{

}
