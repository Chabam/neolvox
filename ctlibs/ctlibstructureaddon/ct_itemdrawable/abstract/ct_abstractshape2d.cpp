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

#include "ct_abstractshape2d.h"

double CT_AbstractShape2D::Z_PLANE_FOR_2D_SHAPES = 0;

CT_DEFAULT_IA_INIT(CT_AbstractShape2D)

CT_AbstractShape2D::CT_AbstractShape2D() : SuperClass(),
    _data(nullptr),
    _zValue(0),
    _zValueDefined(false)
{
}

CT_AbstractShape2D::CT_AbstractShape2D(CT_Shape2DData* data) : SuperClass(),
    _data(data),
    _zValue(0),
    _zValueDefined(false)
{
    Q_ASSERT(_data != nullptr);
    if (_data == nullptr) {qDebug() << "CT_AbstractShape2D::CT_AbstractShape2D" << ", " << "_data == nullptr";}

    Eigen::Vector3d center(data->getCenter()(0), data->getCenter()(1), _zValue);
    setCenterCoordinate(center);

    Eigen::Vector3d min, max;
    _data->getBoundingBox(min, max);
    setBoundingBox(min, max);
}

CT_AbstractShape2D::CT_AbstractShape2D(const CT_AbstractShape2D& other) : SuperClass(other),
    _data((other._data != nullptr) ? other._data->copy() : nullptr),
    _zValue(other._zValue),
    _zValueDefined(other._zValueDefined)
{
}

CT_AbstractShape2D::~CT_AbstractShape2D()
{
    delete _data;
}

void CT_AbstractShape2D::setCenterX(double x)
{
    _data->setCenterX(x);
    SuperClass::setCenterX(x);
}

void CT_AbstractShape2D::setCenterY(double y)
{
    _data->setCenterY(y);
    SuperClass::setCenterY(y);
}

void CT_AbstractShape2D::setCenterZ(double z)
{
    setZValue(z);
}

void CT_AbstractShape2D::setCenterCoordinate(const Eigen::Vector3d& center)
{
    const Eigen::Vector3d center3D(center(0), center(1), _zValueDefined ? _zValue : Z_PLANE_FOR_2D_SHAPES);
    _data->setCenter(center3D);

    SuperClass::setCenterCoordinate(center3D);
}

void CT_AbstractShape2D::setZValue(double z)
{
    _zValue = z;
    _zValueDefined = true;

    SuperClass::setCenterZ(z);
}

double CT_AbstractShape2D::zValue() const
{
    return _zValue;
}

bool CT_AbstractShape2D::isZValueDefined() const
{
    return _zValueDefined;
}

const CT_Shape2DData* CT_AbstractShape2D::getPointerData() const
{
    return _data;
}

const CT_Shape2DData& CT_AbstractShape2D::getData() const
{
    return *_data;
}

const Eigen::Vector2d& CT_AbstractShape2D::getCenter() const
{
    return _data->getCenter();
}

void CT_AbstractShape2D::setBoundingBox(const Eigen::Vector3d& min, const Eigen::Vector3d& max)
{
    Eigen::Vector3d cmin = min;
    Eigen::Vector3d cmax = max;

    cmin(2) = _zValueDefined ? _zValue : Z_PLANE_FOR_2D_SHAPES;
    cmax(2) = cmin(2);

    SuperClass::setBoundingBox(cmin, cmax);
}

CT_Shape2DData* CT_AbstractShape2D::getDataNotConst() const
{
    return _data;
}
