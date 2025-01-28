/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#ifndef CT_TRIANGULATION2D_H
#define CT_TRIANGULATION2D_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardtriangulation2ddrawmanager.h"

#include "ct_math/delaunay2d/ct_delaunaytriangulation.h"

/*!
 * \class CT_Triangulation2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 2D triangulation</b>
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Triangulation2D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Triangulation2D, CT_AbstractItemDrawableWithoutPointCloud, 2D triangulation)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_Triangulation2D();
    CT_Triangulation2D(CT_DelaunayTriangulation* delaunay);
    CT_Triangulation2D(const CT_Triangulation2D& other);
    ~CT_Triangulation2D() override;

    CT_DelaunayTriangulation* getDelaunayT() const;

    CT_ITEM_COPY_IMP(CT_Triangulation2D)

private:
    const static CT_StandardTriangulation2DDrawManager   TRIANGULATION2D_DRAW_MANAGER;

protected:
    CT_DelaunayTriangulation   *_delaunay;
};

#endif // CT_TRIANGULATION2D_H
