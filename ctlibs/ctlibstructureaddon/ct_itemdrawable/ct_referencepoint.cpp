/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#include "ct_referencepoint.h"
#include "ct_log/ct_logmanager.h"

const CT_StandardReferencePointDrawManager CT_ReferencePoint::REFERENCEPOINT_DRAW_MANAGER;
int CT_ReferencePoint::_currentRefId = 0;

CT_TYPE_IMPL_INIT_MACRO(CT_ReferencePoint)

CT_ReferencePoint::CT_ReferencePoint() : SuperClass(),
    _refId(-1) // referencePoint non valide
{
    setBaseDrawManager(&REFERENCEPOINT_DRAW_MANAGER);
}

CT_ReferencePoint::CT_ReferencePoint(double x, double y, double z, double buffer) : SuperClass(),
    _refId(_currentRefId++)
{
    SuperClass::setCenterX(x);
    SuperClass::setCenterY(y);
    SuperClass::setCenterZ(z);
    SuperClass::setBoundingBox(x, y, z, x, y, z);

    setXYBuffer(buffer);

    setBaseDrawManager(&REFERENCEPOINT_DRAW_MANAGER);
}

void CT_ReferencePoint::setXYBuffer(double buffer)
{
    _xyBuffer = buffer;
}

void CT_ReferencePoint::setCenterX(double)
{
    PS_LOG->addDebugMessage(LogInterface::itemdrawable, tr("setCenterX n'a aucun aucun effet dans le cas d'un CT_ReferencePoint"));
}

void CT_ReferencePoint::setCenterY(double)
{
    PS_LOG->addDebugMessage(LogInterface::itemdrawable, tr("setCenterY n'a aucun aucun effet dans le cas d'un CT_ReferencePoint"));
}

void CT_ReferencePoint::setCenterZ(double)
{
    PS_LOG->addDebugMessage(LogInterface::itemdrawable, tr("setCenterZ n'a aucun aucun effet dans le cas d'un CT_ReferencePoint"));
}

void CT_ReferencePoint::setCenterCoordinate(const Eigen::Vector3d&)
{
    PS_LOG->addDebugMessage(LogInterface::itemdrawable, tr("setCenterCoordinate n'a aucun aucun effet dans le cas d'un CT_ReferencePoint"));
}

void CT_ReferencePoint::setBoundingBox(const Eigen::Vector3d&, const Eigen::Vector3d&)
{
    PS_LOG->addDebugMessage(LogInterface::itemdrawable, tr("setBoundingBox n'a aucun aucun effet dans le cas d'un CT_ReferencePoint"));
}
