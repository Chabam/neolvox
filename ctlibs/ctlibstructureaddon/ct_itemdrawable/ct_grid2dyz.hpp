/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
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

#ifndef CT_GRID2DYZ_HPP
#define CT_GRID2DYZ_HPP

#include "ct_itemdrawable/ct_grid2dyz.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid2dyzdrawmanager.h"
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <typeinfo>

#include "qdebug.h"

template< typename DataT>
const CT_StandardGrid2DYZDrawManager<DataT> CT_Grid2DYZ<DataT>::ABSGRID2DYZ_DRAW_MANAGER;

template< typename DataT>
CT_Grid2DYZ<DataT>::CT_Grid2DYZ() : CT_VirtualGrid2D<DataT>()
{
    this->setBaseDrawManager(&ABSGRID2DYZ_DRAW_MANAGER);
}

template< typename DataT>
CT_Grid2DYZ<DataT>::CT_Grid2DYZ(const CT_OutAbstractSingularItemModel *model,
                                const CT_AbstractResult *result,
                                double ymin,
                                double zmin,
                                size_t dimy,
                                size_t dimz,
                                double resolution,
                                double xlevel,
                                DataT na,
                                DataT initValue) : CT_VirtualGrid2D<DataT>(model, result)
{
    this->_minCoordinates(0) = xlevel;
    this->_minCoordinates(1) = ymin;
    this->_minCoordinates(2) = zmin;
    this->_res = resolution;
    this->_dimCol = dimy;
    this->_dimLin = dimz;
    this->_maxCoordinates(0) = xlevel;
    this->_maxCoordinates(1) = this->minY() + this->_res * this->_dimCol;
    this->_maxCoordinates(2) = this->minZ() + this->_res * this->_dimLin;

    this->_minColCoord = ymin;
    this->_minLinCoord = zmin;

    this->_level = xlevel;
    this->_NAdata = na;

    this->setCenterX (xlevel);
    this->setCenterY (this->minY() + (this->maxY() - this->minY())/2.0);
    this->setCenterZ (this->minZ() + (this->maxZ() - this->minZ())/2.0);

    this->_data.resize(this->nCells());
    this->initGridWithValue(initValue);

    this->setBaseDrawManager(&ABSGRID2DYZ_DRAW_MANAGER);
}

template< typename DataT>
CT_Grid2DYZ<DataT>::CT_Grid2DYZ(const QString &modelName,
                                const CT_AbstractResult *result,
                                double ymin,
                                double zmin,
                                size_t dimy,
                                size_t dimz,
                                double resolution,
                                double xlevel,
                                DataT na,
                                DataT initValue) : CT_VirtualGrid2D<DataT>(modelName, result)
{
    this->_minCoordinates(0) = xlevel;
    this->_minCoordinates(1) = ymin;
    this->_minCoordinates(2) = zmin;
    this->_res = resolution;
    this->_dimCol = dimy;
    this->_dimLin = dimz;
    this->_maxCoordinates(0) = xlevel;
    this->_maxCoordinates(1) = this->minY() + this->_res * this->_dimCol;
    this->_maxCoordinates(2) = this->minZ() + this->_res * this->_dimLin;

    this->_minColCoord = ymin;
    this->_minLinCoord = zmin;

    this->_level = xlevel;
    this->_NAdata = na;

    this->setCenterX (xlevel);
    this->setCenterY (this->minY() + (this->maxY() - this->minY())/2.0);
    this->setCenterZ (this->minZ() + (this->maxZ() - this->minZ())/2.0);

    this->_data.resize(this->nCells());
    this->initGridWithValue(initValue);

    this->setBaseDrawManager(&ABSGRID2DYZ_DRAW_MANAGER);
}

template< typename DataT>
CT_Grid2DYZ<DataT>* CT_Grid2DYZ<DataT>::createGrid2DYZFromYZCoords(const CT_OutAbstractSingularItemModel *model,
                                                                   const CT_AbstractResult *result,
                                                                   double ymin,
                                                                   double zmin,
                                                                   double ymax,
                                                                   double zmax,
                                                                   double resolution,
                                                                   double xlevel,
                                                                   DataT na,
                                                                   DataT initValue)
{
    size_t dimy = ceil((ymax - ymin)/resolution);
    size_t dimz = ceil((zmax - zmin)/resolution);

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (ymax >= (ymin + dimy * resolution))
    {
        dimy++;
    }

    while (zmax >= (zmin + dimz * resolution))
    {
        dimz++;
    }

    return new CT_Grid2DYZ<DataT>(model, result, ymin, zmin, dimy, dimz, resolution, xlevel, na, initValue);
}

template< typename DataT>
CT_Grid2DYZ<DataT>* CT_Grid2DYZ<DataT>::createGrid2DYZFromYZCoords(const QString &modelName,
                                                                   const CT_AbstractResult *result,
                                                                   double ymin,
                                                                   double zmin,
                                                                   double ymax,
                                                                   double zmax,
                                                                   double resolution,
                                                                   double xlevel,
                                                                   DataT na,
                                                                   DataT initValue)
{
    size_t dimy = ceil((ymax - ymin)/resolution);
    size_t dimz = ceil((zmax - zmin)/resolution);

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (ymax >= (ymin + dimy * resolution))
    {
        dimy++;
    }

    while (zmax >= (zmin + dimz * resolution))
    {
        dimz++;
    }

    return new CT_Grid2DYZ<DataT>(modelName, result, ymin, zmin, dimy, dimz, resolution, xlevel, na, initValue);
}

template< typename DataT>
CT_Grid2DYZ<DataT>::~CT_Grid2DYZ()
{

}

template< typename DataT>
void CT_Grid2DYZ<DataT>::initDrawManager(QString drawConfigurationName, bool mapMode, bool scale)
{
    this->setBaseDrawManager(new CT_StandardGrid2DYZDrawManager<DataT>(drawConfigurationName, mapMode, scale));
}

template< typename DataT>
QString CT_Grid2DYZ<DataT>::getType() const
{
    return staticGetType();
}

template< typename DataT>
QString CT_Grid2DYZ<DataT>::staticGetType()
{
    QString type = CT_AbstractGrid2D::staticGetType() + "/CT_Grid2DYZ<" + CT_TypeInfo::name<DataT>() + ">";
    CT_AbstractItemDrawable::addNameTypeCorresp(type, staticName());
    return type;
}

template< typename DataT>
QString CT_Grid2DYZ<DataT>::name() const
{
    return staticName();
}

template< typename DataT>
QString CT_Grid2DYZ<DataT>::staticName()
{
    return QString("CT_Grid2DYZ<") + CT_TypeInfo::name<DataT>() + QString(">");
}

template< typename DataT>
CT_AbstractItemDrawable* CT_Grid2DYZ<DataT>::copy(const CT_OutAbstractItemModel *model, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList)
{
    CT_Grid2DYZ<DataT>* cpy = new CT_Grid2DYZ<DataT>((const CT_OutAbstractSingularItemModel *)model, result, this->minY(), this->minZ(), this->_dimCol, this->_dimLin, this->_res, this->_level, this->_NAdata, this->_NAdata);
    cpy->setId(this->id());
    size_t size = this->nCells();
    for (size_t i = 0 ; i < size ; i++)
    {
        cpy->_data[i] = this->_data[i];
    }

    if (size >0)
    {
        cpy->computeMinMax();
    }
    cpy->setAlternativeDrawManager(this->getAlternativeDrawManager());

    return cpy;
}

template< typename DataT>
CT_AbstractItemDrawable* CT_Grid2DYZ<DataT>::copy(const QString &modelName, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList)
{
    CT_Grid2DYZ<DataT>* cpy = new CT_Grid2DYZ<DataT>(modelName, result, this->minY(), this->minZ(), this->_dimCol, this->_dimLin, this->_res, this->_level, this->_NAdata, this->_NAdata);
    cpy->setId(this->id());
    size_t size = this->nCells();
    for (size_t i = 0 ; i < size ; i++)
    {
        cpy->_data[i] = this->_data[i];
    }

    if (size >0)
    {
        cpy->computeMinMax();
    }
    cpy->setAlternativeDrawManager(this->getAlternativeDrawManager());

    return cpy;
}

#endif // CT_GRID2DYZ_HPP
