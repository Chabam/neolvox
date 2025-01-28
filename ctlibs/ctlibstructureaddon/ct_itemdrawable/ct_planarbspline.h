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

#ifndef CT_PLANARBSPLINE_H
#define CT_PLANARBSPLINE_H

#include "ct_itemdrawable/abstract/ct_abstractshape.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardplanarbsplinedrawmanager.h"
#include "ct_shapedata/ct_planarbsplinedata.h"

/*!
 * \class CT_PlanarBSpline
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_PlanarBSplineData</b>
 *
 * It represents a planar B-Spline in 3D.
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_PlanarBSpline : public CT_AbstractShape
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_PlanarBSpline, CT_AbstractShape, Planar B-spline)
    using SuperClass = CT_AbstractShape;

public:

    CT_PlanarBSpline();
    CT_PlanarBSpline(CT_PlanarBSplineData* data);
    CT_PlanarBSpline(const CT_PlanarBSpline& other) = default;

    int getDegree() const;
    int getNCP() const;

    void setPolyline(const QList<Eigen::Vector3d>& list);
    const QList<Eigen::Vector3d>& getPolyline() const;

    CT_ITEM_COPY_IMP(CT_PlanarBSpline)

private:

    QList<Eigen::Vector3d>    _polyline;

    CT_DEFAULT_IA_BEGIN(CT_PlanarBSpline)
    CT_DEFAULT_IA_V2(CT_PlanarBSpline, CT_AbstractCategory::staticInitDataNumber(), &CT_PlanarBSpline::getDegree, QObject::tr("Degré de polynome"))
    CT_DEFAULT_IA_V2(CT_PlanarBSpline, CT_AbstractCategory::staticInitDataNumber(), &CT_PlanarBSpline::getNCP, QObject::tr("Nombre de points de contrôle"))
    CT_DEFAULT_IA_END(CT_PlanarBSpline)

    const static CT_StandardPlanarBSplineDrawManager   PLANARBSPLINE_DRAW_MANAGER;
};

#endif // CT_PLANARBSPLINE_H
