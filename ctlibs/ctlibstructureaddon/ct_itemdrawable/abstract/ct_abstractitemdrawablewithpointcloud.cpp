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

#include "ct_abstractitemdrawablewithpointcloud.h"

#include <limits>

#include "ct_cloudindex/abstract/ct_abstractcloudindext.h"
#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"
#include "ct_iterator/ct_pointiterator.h"

CT_StandardAbstractItemDrawableWithPointCloudDrawManager CT_AbstractItemDrawableWithPointCloud::IDWITHPC_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_AbstractItemDrawableWithPointCloud)

CT_AbstractItemDrawableWithPointCloud::CT_AbstractItemDrawableWithPointCloud() : SuperClass()
{
    m_shortcutToPointCloudIndexFromRegistered = nullptr;
    setBaseDrawManager(&IDWITHPC_DRAW_MANAGER);
}

CT_AbstractItemDrawableWithPointCloud::CT_AbstractItemDrawableWithPointCloud(CT_PCIR pcir) : SuperClass()
{
    m_shortcutToPointCloudIndexFromRegistered = nullptr;
    setPointCloudIndexRegistered(pcir);
    setBaseDrawManager(&IDWITHPC_DRAW_MANAGER);
}

CT_AbstractItemDrawableWithPointCloud::~CT_AbstractItemDrawableWithPointCloud()
{
    deletePointCloudIndexRegistered();
}

void CT_AbstractItemDrawableWithPointCloud::updateBoundingBox()
{
    Eigen::Vector3d min;
    Eigen::Vector3d max;

    min(0) = std::numeric_limits<double>::max();
    min(1) = std::numeric_limits<double>::max();
    min(2) = std::numeric_limits<double>::max();

    max(0) = -std::numeric_limits<double>::max();
    max(1) = -std::numeric_limits<double>::max();
    max(2) = -std::numeric_limits<double>::max();

    CT_PointIterator it(pointCloudIndex());

    while(it.hasNext())
    {
        it.next();

        const CT_Point& point = it.currentPoint();

        if (point(0) < min(0)) {min(0) = point(0);}
        if (point(1) < min(1)) {min(1) = point(1);}
        if (point(2) < min(2)) {min(2) = point(2);}

        if (point(0) > max(0)) {max(0) = point(0);}
        if (point(1) > max(1)) {max(1) = point(1);}
        if (point(2) > max(2)) {max(2) = point(2);}
    }

    setBoundingBox(min, max);
}

void CT_AbstractItemDrawableWithPointCloud::setPointCloudIndexRegistered(CT_PCIR pcir, const bool& mustUpdateBoundingBox)
{
    deletePointCloudIndexRegistered();

    m_pointCloudIndexRegistered = pcir;

    if(m_pointCloudIndexRegistered.data() != nullptr)
        m_shortcutToPointCloudIndexFromRegistered = dynamic_cast<CT_AbstractPointCloudIndex*>(m_pointCloudIndexRegistered->abstractCloudIndexT());
    else
        m_shortcutToPointCloudIndexFromRegistered = nullptr;

    if(mustUpdateBoundingBox)
        updateBoundingBox();
}

const CT_AbstractPointCloudIndex* CT_AbstractItemDrawableWithPointCloud::pointCloudIndex() const
{
    return m_shortcutToPointCloudIndexFromRegistered;
}

CT_PCIR CT_AbstractItemDrawableWithPointCloud::pointCloudIndexRegistered() const
{
    return m_pointCloudIndexRegistered;
}

size_t CT_AbstractItemDrawableWithPointCloud::pointCloudIndexSize() const
{
    if(pointCloudIndex() == nullptr)
        return 0;

    return pointCloudIndex()->size();
}

void CT_AbstractItemDrawableWithPointCloud::deletePointCloudIndexRegistered()
{
    m_pointCloudIndexRegistered = CT_PCIR();
    m_shortcutToPointCloudIndexFromRegistered = nullptr;
}
