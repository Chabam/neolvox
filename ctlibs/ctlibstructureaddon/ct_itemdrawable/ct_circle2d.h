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

#ifndef CT_CIRCLE2D_H
#define CT_CIRCLE2D_H

#include "ct_itemdrawable/abstract/ct_abstractareashape2d.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardcircle2ddrawmanager.h"

#include "ct_shape2ddata/ct_circle2ddata.h"

/*!
 * \class CT_Circle2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_Circle2DData</b>
 *
 * It represents a circle in 2D, defined by a center and a radius.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Circle2D : public CT_AbstractAreaShape2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Circle2D, CT_AbstractAreaShape2D, 2D circle)

    using SuperClass = CT_AbstractAreaShape2D;

public:
    CT_Circle2D();
    CT_Circle2D(CT_Circle2DData* data);
    CT_Circle2D(const CT_Circle2D& other) = default;

    double getRadius() const;

    CT_ITEM_COPY_IMP(CT_Circle2D)

private:
    static const CT_StandardCircle2DDrawManager   CIRCLE2D_DRAW_MANAGER;

    CT_DEFAULT_IA_BEGIN(CT_Circle2D)
    CT_DEFAULT_IA_V2(CT_Circle2D, CT_AbstractCategory::staticInitDataRadius(), &CT_Circle2D::getRadius, QObject::tr("Rayon du cercle"))
    CT_DEFAULT_IA_END(CT_Circle2D)
};

#endif // CT_CIRCLE2D_H
