/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#ifndef CT_CIRCLE2DDATA_H
#define CT_CIRCLE2DDATA_H

#include "ct_areashape2ddata.h"
#include "ct_pointcloud/abstract/ct_abstractpointcloud.h"
#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_Circle2DDataPreProcessingAction
{
public:
    virtual ~CT_Circle2DDataPreProcessingAction() {}
    virtual void preProcessing(const CT_Point& point, CT_Point& newPoint) = 0;
};

class CTLIBSTRUCTUREADDON_EXPORT CT_Circle2DData : public CT_AreaShape2DData
{
    using SuperClass = CT_AreaShape2DData;

public:
    CT_Circle2DData();
    CT_Circle2DData(const Eigen::Vector2d& center, double radius);
    CT_Circle2DData(const CT_Circle2DData& other) = default;

    void setRadius(double radius);
    double getRadius() const;

    void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const override;

    bool contains(double x, double y) const override;
    double getArea() const override;

    CT_SHAPEDATA2D_CLONE_IMP(CT_Circle2DData)

    CT_Circle2DData& operator= (const CT_Circle2DData& o) = default;

private:
    double _radius;
};

#endif // CT_CIRCLE2DDATA_H
