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

#ifndef CT_LINE_H
#define CT_LINE_H

#include "ct_itemdrawable/abstract/ct_abstractshape.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardlinedrawmanager.h"

#include "ct_shapedata/ct_linedata.h"

/*!
 * \class CT_Line
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a CT_LineData</b>
 *
 *
 * It represents a line in 3D, defined by two points.
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Line : public CT_AbstractShape
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Line, CT_AbstractShape, 3D line)
    using SuperClass = CT_AbstractShape;

public:
    CT_Line();
    CT_Line(CT_LineData* data);
    CT_Line(const CT_Line& other) = default;

    inline const Eigen::Vector3d& getP1() const {return dataConstCastAs<CT_LineData>()->getP1();}
    inline const Eigen::Vector3d& getP2() const {return dataConstCastAs<CT_LineData>()->getP2();}

    inline double getP1_X() const {return dataConstCastAs<CT_LineData>()->x1();}
    inline double getP1_Y() const {return dataConstCastAs<CT_LineData>()->y1();}
    inline double getP1_Z() const {return dataConstCastAs<CT_LineData>()->z1();}
    inline double getP2_X() const {return dataConstCastAs<CT_LineData>()->x2();}
    inline double getP2_Y() const {return dataConstCastAs<CT_LineData>()->y2();}
    inline double getP2_Z() const {return dataConstCastAs<CT_LineData>()->z2();}
    inline double getLength() const {return dataConstCastAs<CT_LineData>()->length();}
    inline double getError() const {return dataConstCastAs<CT_LineData>()->getError();}

    /**
      * \brief Retourne une ligne 3D (rgression linaire)  partir du nuage de points pass en paramtre.
      *
      * \return nullptr si le nombre de points est infrieur  2.
      */
    static CT_Line* staticCreateLineFromPointCloud(const CT_AbstractPointCloudIndex &pointCloudIndex);

    CT_ITEM_COPY_IMP(CT_Line)

private:
    const static CT_StandardLineDrawManager   LINE_DRAW_MANAGER;

    CT_DEFAULT_IA_BEGIN(CT_Line)
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataX(), &CT_Line::getP1_X, QObject::tr("X1"))
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataY(), &CT_Line::getP1_Y, QObject::tr("Y1"))
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataZ(), &CT_Line::getP1_Z, QObject::tr("Z1"))

    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataX(), &CT_Line::getP2_X, QObject::tr("X2"))
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataY(), &CT_Line::getP2_Y, QObject::tr("Y2"))
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataZ(), &CT_Line::getP2_Z, QObject::tr("Z2"))

    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataLength(), &CT_Line::getLength, QObject::tr("Longueur"))
    CT_DEFAULT_IA_V2(CT_Line, CT_AbstractCategory::staticInitDataR2(), &CT_Line::getError, QObject::tr("Erreur d'ajustement de la ligne"))
    CT_DEFAULT_IA_END(CT_Line)
};

#endif // CT_LINE_H
