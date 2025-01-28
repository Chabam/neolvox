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

#include "ct_ellipse.h"

const CT_StandardEllipseDrawManager CT_Ellipse::ELLIPSE_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Ellipse)

CT_Ellipse::CT_Ellipse(): SuperClass()
{
    setBaseDrawManager(&ELLIPSE_DRAW_MANAGER);
}

CT_Ellipse::CT_Ellipse(CT_EllipseData* data) : SuperClass(data)
{
    setBaseDrawManager(&ELLIPSE_DRAW_MANAGER);

    const Eigen::Vector3d& center = data->getCenter();
    const double length = std::max(getAxisA().length(), getAxisB().length());

    setBoundingBox(center(0) - length,
                   center(1) - length,
                   center(2) - length,
                   center(0) + length,
                   center(1) + length,
                   center(2) + length);
}

const CT_LineData& CT_Ellipse::getAxisA() const
{
    return dataConstCastAs<CT_EllipseData>()->getAxisA();
}

const CT_LineData& CT_Ellipse::getAxisB() const
{
    return dataConstCastAs<CT_EllipseData>()->getAxisB();
}

double CT_Ellipse::getError() const
{
    return dataConstCastAs<CT_EllipseData>()->getError();
}

double CT_Ellipse::getAxisRatio() const
{
    return getAxisA().length()/getAxisB().length();
}

double CT_Ellipse::getAxisAP1X() const
{
    return getAxisA().getP1().x();
}

double CT_Ellipse::getAxisAP1Y() const
{
    return getAxisA().getP1().y();
}

double CT_Ellipse::getAxisAP1Z() const
{
    return getAxisA().getP1().z();
}

double CT_Ellipse::getAxisAP2X() const
{
    return getAxisA().getP2().x();
}

double CT_Ellipse::getAxisAP2Y() const
{
    return getAxisA().getP2().y();
}

double CT_Ellipse::getAxisAP2Z() const
{
    return getAxisA().getP2().z();
}

double CT_Ellipse::getAxisALength() const
{
    return getAxisA().length();
}

double CT_Ellipse::getAxisBP1X() const
{
    return getAxisB().getP1().x();
}

double CT_Ellipse::getAxisBP1Y() const
{
    return getAxisB().getP1().y();
}

double CT_Ellipse::getAxisBP1Z() const
{
    return getAxisB().getP1().z();
}

double CT_Ellipse::getAxisBP2X() const
{
    return getAxisB().getP2().x();
}

double CT_Ellipse::getAxisBP2Y() const
{
    return getAxisB().getP2().y();
}

double CT_Ellipse::getAxisBP2Z() const
{
    return getAxisB().getP2().z();
}

double CT_Ellipse::getAxisBLength() const
{
    return getAxisB().length();
}

CT_Ellipse* CT_Ellipse::staticCreateZAxisAlignedEllipseFromPointCloud(const CT_AbstractPointCloudIndex* pointCloudIndex)
{
    CT_EllipseData* data = CT_EllipseData::staticCreateZAxisAlignedEllipseDataFromPointCloud(pointCloudIndex);

    if(data == nullptr)
        return nullptr;

    return new CT_Ellipse(data);
}
