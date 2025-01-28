/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre Piboule (ONF)

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

#ifndef CT_POLYLINE2D_H
#define CT_POLYLINE2D_H

#include "ct_itemdrawable/abstract/ct_abstractshape2d.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardpolyline2ddrawmanager.h"

#include "ct_shape2ddata/ct_polyline2ddata.h"

/*!
 * \class CT_Polyline2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_Polyline2DData</b>
 *
 * It represents a polygon in 2D.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Polyline2D : public CT_AbstractShape2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Polyline2D, CT_AbstractShape2D, 2D polyline)
    using SuperClass = CT_AbstractShape2D;

public:    

    CT_Polyline2D();
    CT_Polyline2D(CT_Polyline2DData *data);
    CT_Polyline2D(const CT_Polyline2D& other) = default;

    const QVector<Eigen::Vector2d>& getVertices() const;

    inline int getNumberOfVertices() const {return getVertices().size();}

    CT_ITEM_COPY_IMP(CT_Polyline2D)

private:
    const static CT_StandardPolyline2DDrawManager POLYLINE2D_DRAW_MANAGER;

    CT_DEFAULT_IA_BEGIN(CT_Polyline2D)
    CT_DEFAULT_IA_V2(CT_Polyline2D, CT_AbstractCategory::staticInitDataSize(), &CT_Polyline2D::getNumberOfVertices, QObject::tr("Number Of Vertices"))
    CT_DEFAULT_IA_END(CT_Polyline2D)
};

#endif // CT_POLYLINE2D_H
