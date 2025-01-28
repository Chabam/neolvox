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

#ifndef CT_CYLINDER_H
#define CT_CYLINDER_H

#include "ct_itemdrawable/abstract/ct_abstractshape.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardcylinderdrawmanager.h"
#include "ct_shapedata/ct_cylinderdata.h"

/*!
 * \class CT_Cylinder
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_CylinderData</b>
 *
 * It represents a cylinder in 3D, defined by a center, a direction a radius and a height.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Cylinder : public CT_AbstractShape
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Cylinder, CT_AbstractShape, Cylinder)

    using SuperClass = CT_AbstractShape;

public:
    CT_Cylinder();
    CT_Cylinder(CT_CylinderData* data);
    CT_Cylinder(const CT_Cylinder& other) = default;

    double getRadius() const;
    double getHeight() const;
    double getLineError() const;
    double getCircleError() const;

    /**
      * \brief Retourne un cercle 2D  partir du nuage de points pass en paramtre.
      *
      * \return nullptr si le nombre de points est infrieur  3.
      */
    static CT_Cylinder* staticCreate3DCylinderFromPointCloud(const CT_AbstractPointCloudIndex &pointCloudIndex,
                                                             const Eigen::Vector3d &pointCloudBarycenter);

    CT_ITEM_COPY_IMP(CT_Cylinder)

private:

    static const CT_StandardCylinderDrawManager CYLINDER_DRAW_MANAGER;

    double pDirectionX() const {return this->getDirectionX();}
    double pDirectionY() const {return this->getDirectionY();}
    double pDirectionZ() const {return this->getDirectionZ();}

    CT_DEFAULT_IA_BEGIN(CT_Cylinder)
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataXDirection(), &CT_Cylinder::pDirectionX, QObject::tr("Direction X"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataYDirection(), &CT_Cylinder::pDirectionY, QObject::tr("Direction Y"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataZDirection(), &CT_Cylinder::pDirectionZ, QObject::tr("Direction Z"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataRadius(), &CT_Cylinder::getRadius, QObject::tr("Rayon"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataLength(), &CT_Cylinder::getHeight, QObject::tr("Longueur"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataR2(), &CT_Cylinder::getCircleError, QObject::tr("RMSE d'ajustement du cercle"))
    CT_DEFAULT_IA_V2(CT_Cylinder, CT_AbstractCategory::staticInitDataR2(), &CT_Cylinder::getLineError, QObject::tr("RMSE d'ajustement de la ligne"))
    CT_DEFAULT_IA_END(CT_Cylinder)
};

#endif // CT_CYLINDER_H
