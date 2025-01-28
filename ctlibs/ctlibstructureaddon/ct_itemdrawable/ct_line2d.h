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

#ifndef CT_LINE2D_H
#define CT_LINE2D_H

#include "ct_itemdrawable/abstract/ct_abstractshape2d.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardline2ddrawmanager.h"

#include "ct_shape2ddata/ct_line2ddata.h"

/*!
 * \class CT_Line2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_Line2DData</b>
 *
 * It represents a line in 2D, defined by two points.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Line2D : public CT_AbstractShape2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Line2D, CT_AbstractShape2D, 2D line)
    using SuperClass = CT_AbstractShape2D;

public:
    CT_Line2D();
    CT_Line2D(CT_Line2DData *data);
    CT_Line2D(const CT_Line2D& other) = default;

    const Eigen::Vector2d &getP1() const;
    const Eigen::Vector2d &getP2() const;

    double x1() const;
    double y1() const;

    double x2() const;
    double y2() const;

    double length() const;

    CT_ITEM_COPY_IMP(CT_Line2D)

private:

    CT_DEFAULT_IA_BEGIN(CT_Line2D)
    CT_DEFAULT_IA_V2(CT_Line2D, CT_AbstractCategory::staticInitDataLength(), &CT_Line2D::length, QObject::tr("Longueur de la ligne"))
    CT_DEFAULT_IA_V2(CT_Line2D, CT_AbstractCategory::staticInitDataX(), &CT_Line2D::x1, QObject::tr("X1"))
    CT_DEFAULT_IA_V2(CT_Line2D, CT_AbstractCategory::staticInitDataY(), &CT_Line2D::y1, QObject::tr("Y1"))
    CT_DEFAULT_IA_V2(CT_Line2D, CT_AbstractCategory::staticInitDataX(), &CT_Line2D::x2, QObject::tr("X2"))
    CT_DEFAULT_IA_V2(CT_Line2D, CT_AbstractCategory::staticInitDataY(), &CT_Line2D::y2, QObject::tr("Y2"))
    CT_DEFAULT_IA_END(CT_Line2D)

    const static CT_StandardLine2DDrawManager  LINE2D_DRAW_MANAGER;
};

#endif // CT_LINE2D_H
