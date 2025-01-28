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

#include "ct_abstractshape.h"

CT_TYPE_IMPL_INIT_MACRO(CT_AbstractShape)

CT_AbstractShape::CT_AbstractShape() : SuperClass(),
    _data(nullptr)
{
}

CT_AbstractShape::CT_AbstractShape(CT_ShapeData* data) : SuperClass(),
    _data(data)
{
    Q_ASSERT(_data != nullptr);
    if (_data == nullptr) {qDebug() << "CT_AbstractShape::CT_AbstractShape" << ", " << "_data == nullptr";}

    setCenterCoordinate(data->getCenter());

    Eigen::Vector3d min, max;
    data->getBoundingBox(min, max);
    setBoundingBox(min, max);
}

CT_AbstractShape::CT_AbstractShape(const CT_AbstractShape& other) : SuperClass(other),
    _data((other._data != nullptr) ? other._data->copy() : nullptr)
{
}

void CT_AbstractShape::setCenterX(double x)
{
    _data->setCenterX(x);
    SuperClass::setCenterX(x);
}

void CT_AbstractShape::setCenterY(double y)
{
    _data->setCenterY(y);
    SuperClass::setCenterY(y);
}

void CT_AbstractShape::setCenterZ(double z)
{
    _data->setCenterZ(z);
    SuperClass::setCenterZ(z);
}

void CT_AbstractShape::setCenterCoordinate(const Eigen::Vector3d& center)
{
    _data->setCenter(center);
    SuperClass::setCenterCoordinate(center);
}

double CT_AbstractShape::centerX() const
{
    return _data->getCenter()(0);
}

double CT_AbstractShape::centerY() const
{
    return _data->getCenter()(1);
}

double CT_AbstractShape::centerZ() const
{
    return _data->getCenter()(2);
}

const CT_ShapeData* CT_AbstractShape::getPointerData() const
{
    return _data;
}

const CT_ShapeData& CT_AbstractShape::getData() const
{
    return *_data;
}

const Eigen::Vector3d& CT_AbstractShape::getCenter() const
{
    return _data->getCenter();
}

const Eigen::Vector3d& CT_AbstractShape::getDirection() const
{
    return _data->getDirection();
}

double CT_AbstractShape::getDirectionX() const
{
    return _data->getDirection().x();
}

double CT_AbstractShape::getDirectionY() const
{
    return _data->getDirection().y();
}

double CT_AbstractShape::getDirectionZ() const
{
    return _data->getDirection().z();
}

CT_ShapeData* CT_AbstractShape::getDataNotConst() const
{
    return _data;
}
