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

#ifndef CT_ABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUD_H
#define CT_ABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUD_H

#include "ct_itemdrawable/abstract/ct_abstractgeometricalitem.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithoutpointclouddrawmanager.h"

/**
 * @brief Represent an item that is a leaf of a group in the tree structure. Only singular
 *        item can have item attributes.
 *
 *        This item has a bounding box ! Singular item has only coordinates of the center.
 *
 *        This item doesn't contains a point cloud.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractItemDrawableWithoutPointCloud : public CT_AbstractGeometricalItem
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractItemDrawableWithoutPointCloud, CT_AbstractGeometricalItem, Item without points)

    using SuperClass = CT_AbstractGeometricalItem;

public:
    CT_AbstractItemDrawableWithoutPointCloud();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Min and Max coordinates (bounding box)
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractItemDrawableWithoutPointCloud(const CT_AbstractItemDrawableWithoutPointCloud& other) = default;

private:
    const static CT_StandardAbstractItemDrawableWithoutPointCloudDrawManager IDWITHOUTPC_DRAW_MANAGER;
};

#endif // CT_ABSTRACTITEMDRAWABLEWITHOUTPOINTCLOUD_H
