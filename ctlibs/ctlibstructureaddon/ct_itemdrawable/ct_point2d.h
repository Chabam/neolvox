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

#ifndef CT_POINT2D_H
#define CT_POINT2D_H

#include "ct_itemdrawable/abstract/ct_abstractshape2d.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardpoint2ddrawmanager.h"

#include "ct_shape2ddata/ct_point2ddata.h"

/*!
 * \class CT_Point2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_Point2DData</b>
 *
 * It represents a line in 2D, defined by two points.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Point2D : public CT_AbstractShape2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Point2D, CT_AbstractShape2D, 2D point)
    using SuperClass = CT_AbstractShape2D;

public:

    CT_Point2D();
    CT_Point2D(CT_Point2DData *data);
    CT_Point2D(const CT_Point2D& other) = default;

    double x() const;
    double y() const;

    CT_ITEM_COPY_IMP(CT_Point2D)

private:
    const static CT_StandardPoint2DDrawManager  POINT2D_DRAW_MANAGER;
};

#endif // CT_POINT2D_H
