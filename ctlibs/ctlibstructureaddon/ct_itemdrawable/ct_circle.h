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

#ifndef CT_CIRCLE_H
#define CT_CIRCLE_H

#include "ct_itemdrawable/abstract/ct_abstractshape.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardcircledrawmanager.h"

#include "ct_shapedata/ct_circledata.h"

/*!
 * \class CT_Circle
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_CircleData</b>
 *
 * It represents a circle in 3D, defined by a center, a direction and a radius.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Circle : public CT_AbstractShape
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Circle, CT_AbstractShape, 3D circle)

    using SuperClass = CT_AbstractShape;

public:

    CT_Circle();
    CT_Circle(CT_CircleData* data);
    CT_Circle(const CT_Circle& other) = default;

    double getRadius() const;
    double getError() const;

    /**
      * \brief Retourne un cercle 2D  partir du nuage de points pass en paramtre.
      *
      * \return nullptr si le nombre de points est inférieur à 3.
      */
    static CT_Circle* staticCreateZAxisAlignedCircleFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                   double z = 0);

    CT_ITEM_COPY_IMP(CT_Circle)

private:
    static const CT_StandardCircleDrawManager   CIRCLE_DRAW_MANAGER;

    CT_DEFAULT_IA_BEGIN(CT_Circle)
    CT_DEFAULT_IA_V2(CT_Circle, CT_AbstractCategory::staticInitDataRadius(), &CT_Circle::getRadius, QObject::tr("Rayon du cercle"))
    CT_DEFAULT_IA_V2(CT_Circle, CT_AbstractCategory::staticInitDataR2(), &CT_Circle::getError, QObject::tr("Erreur d'ajustement du cercle"))
    CT_DEFAULT_IA_END(CT_Circle)
};

#endif // CT_CIRCLE_H
