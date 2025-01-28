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

#ifndef CT_ABSTRACTITEMDRAWABLEWITHPOINTCLOUD_H
#define CT_ABSTRACTITEMDRAWABLEWITHPOINTCLOUD_H

#include "ctlibstructureaddon_global.h"

#include "ct_itemdrawable/abstract/ct_abstractgeometricalitem.h"
#include "ct_accessibility/ct_iaccesspointcloud.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardabstractitemdrawablewithpointclouddrawmanager.h"

#include "ct_cloud/tools/ct_globalpointcloudmanager.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_iterator/ct_mutablepointiterator.h"

#include "ct_handle/ct_handleoutpointcolor.h"
#include "ct_handle/ct_handleoutpointnormal.h"
#include "ct_handle/ct_handleoutpointscalar.h"
#include "ct_handle/ct_handleoutpointscalarmask.h"

#include "ct_handle/ct_handleinpointcolor.h"
#include "ct_handle/ct_handleinpointnormal.h"
#include "ct_handle/ct_handleinpointscalar.h"
#include "ct_handle/ct_handleinpointscalarmask.h"

#include <Eigen/Core>

/**
 * @brief Represent an item that is a leaf of a group in the tree structure. Only singular
 *        item can have item attributes.
 *
 *        This item has a bounding box ! Singular item has only coordinates of the center.
 *
 *        This item contains a point cloud.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractItemDrawableWithPointCloud : public CT_AbstractGeometricalItem, public CT_IAccessPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractItemDrawableWithPointCloud, CT_AbstractGeometricalItem, Item with points)

    using SuperClass = CT_AbstractGeometricalItem;

public:
    CT_AbstractItemDrawableWithPointCloud();

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
     *          - Point cloud index registered (shared so just pointer is copied)
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractItemDrawableWithPointCloud(const CT_AbstractItemDrawableWithPointCloud& other) = default;

    /**
     * @brief Constructor
     * @param pcir : a point cloud registered to the repository. You can pass a point cloud registered by another ItemDrawable if you
     *               want because sharing is managed automatically.
     */
    CT_AbstractItemDrawableWithPointCloud(CT_PCIR pcir);
    ~CT_AbstractItemDrawableWithPointCloud();

    /**
     * @brief Update the bounding box. If this item has a valid point cloud index all
     *        points are covered to calculate the bounding box.
     */
    void updateBoundingBox();

    /**
     * @brief Return the point cloud index
     */
    const CT_AbstractPointCloudIndex* pointCloudIndex() const override;

    /**
     * @brief Set a new point cloud index registered
     * @param mustUpdateBoundingBox : set to true if the bounding box must be updated at the same time. True by default
     */
    void setPointCloudIndexRegistered(CT_PCIR pcir, const bool& mustUpdateBoundingBox = true);

    /**
     * @brief Call to delete the registered point cloud index from this item
     */
    void deletePointCloudIndexRegistered();

    /**
     * @brief Return the registered point cloud index
     */
    CT_PCIR pointCloudIndexRegistered() const override;

    /**
     * @brief Return the number of points in the cloud index
     */
    size_t pointCloudIndexSize() const;

private:
    static CT_StandardAbstractItemDrawableWithPointCloudDrawManager IDWITHPC_DRAW_MANAGER;

    // default attributes
    CT_DEFAULT_IA_BEGIN(CT_AbstractItemDrawableWithPointCloud)
    CT_DEFAULT_IA_V2(CT_AbstractItemDrawableWithPointCloud, CT_AbstractCategory::staticInitDataNumber(), &CT_AbstractItemDrawableWithPointCloud::pointCloudIndexSize, QObject::tr("Number of points"))
    CT_DEFAULT_IA_END(CT_AbstractItemDrawableWithPointCloud)

private:
    CT_PCIR                     m_pointCloudIndexRegistered;
    CT_AbstractPointCloudIndex* m_shortcutToPointCloudIndexFromRegistered;
};

#endif // CT_ABSTRACTITEMDRAWABLEWITHPOINTCLOUD_H
