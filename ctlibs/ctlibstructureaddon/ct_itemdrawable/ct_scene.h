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

#ifndef CT_SCENE_H
#define CT_SCENE_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"

/**
 * @brief Represent a point cloud scene
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Scene : public CT_AbstractItemDrawableWithPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Scene, CT_AbstractItemDrawableWithPointCloud, Point scene)

public:
    CT_Scene() = default;

    /**
     * @brief Create a scene
     * @param pcir : a point cloud registered to the repository. You can pass a point cloud registered by another ItemDrawable if you
     *               want because sharing is managed automatically.
     */
    CT_Scene(CT_PCIR pcir);

    /**
      * @brief Copy constructor
      */
    CT_Scene(const CT_Scene& other) = default;

    CT_ITEM_COPY_IMP(CT_Scene)
};

#endif // CT_SCENE_H
