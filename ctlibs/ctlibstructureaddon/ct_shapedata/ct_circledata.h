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

#ifndef CT_CIRCLEDATA_H
#define CT_CIRCLEDATA_H

#include "ct_shapedata.h"
#include "ct_defines.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_CircleDataPreProcessingAction
{
public:
    virtual ~CT_CircleDataPreProcessingAction() {}

    virtual void preProcessing(const CT_Point& point, CT_Point& newPoint) = 0;
};

class CTLIBSTRUCTUREADDON_EXPORT CT_CircleData : public CT_ShapeData
{
    using SuperClass = CT_ShapeData;

public:
    CT_CircleData();
    CT_CircleData(const Eigen::Vector3d& center, const Eigen::Vector3d& direction, double radius);
    CT_CircleData(const Eigen::Vector3d& center, const Eigen::Vector3d& direction, double radius, double error);
    CT_CircleData(const CT_CircleData& other) = default;

    void setRadius(double radius);
    void setError(double error);

    double getRadius() const;
    double getError() const;

    virtual void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const override;


    CT_SHAPEDATA_CLONE_IMP(CT_CircleData)

    CT_CircleData& operator= (const CT_CircleData& o);

    /**
      * \brief Retourne les donnes d'un cercle 2D  partir du nuage de points pass en paramtre.
      *
      * \return nullptr si le nombre de points est infrieur  3.
      */
    static CT_CircleData* staticCreateZAxisAlignedCircleDataFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                           double z = 0);

    /**
      * \brief Retourne les donnes d'un cercle 2D  partir du nuage de points pass en paramtre.
      *
      * \param preProcessingAction : l'interface pass en paramtre peut permettre de modifier un point avant de procder au fitting (par exemple faire une rotation). Peut tre nullptr.
      *
      * \return nullptr si le nombre de points est infrieur  3.
      */
    static CT_CircleData* staticCreateZAxisAlignedCircleDataFromPointCloudWithPreProcessing(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                                            CT_CircleDataPreProcessingAction* preProcessingAction,
                                                                                            double z = 0);
private:
    double _radius;
    double _error;
};

#endif // CT_CIRCLEDATA_H
