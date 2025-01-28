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

#ifndef CT_VIRTUALGRID2D_HPP
#define CT_VIRTUALGRID2D_HPP

#include "ct_itemdrawable/abstract/ct_virtualgrid2d.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <typeinfo>
#include <limits>

#include "qdebug.h"

template< typename DataT>
CT_VirtualGrid2D<DataT>::CT_VirtualGrid2D() : CT_AbstractGrid2D()
{
    _minCoordinates(0) = 0;
    _minCoordinates(1) = 0;
    _minCoordinates(2) = 0;
    _res = 1;
    _dimCol = 0;
    _dimLin = 0;
    _maxCoordinates(0) = 0;
    _maxCoordinates(1) = 0;
    _maxCoordinates(2) = 0;

    _minColCoord = 0;
    _minLinCoord = 0;

    _level = 0;
}

template< typename DataT>
CT_VirtualGrid2D<DataT>::CT_VirtualGrid2D(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result) : CT_AbstractGrid2D(model, result)
{
}

template< typename DataT>
CT_VirtualGrid2D<DataT>::CT_VirtualGrid2D(const QString &modelName, const CT_AbstractResult *result) : CT_AbstractGrid2D(modelName, result)
{
}


template< typename DataT>
CT_VirtualGrid2D<DataT>::~CT_VirtualGrid2D()
{
    _data.clear();
}

template< typename DataT>
void CT_VirtualGrid2D<DataT>::initGridWithValue(const DataT val)
{
    for (size_t  i = 0 ; i < nCells() ; i++)
    {
        _data[i] = val;
    }
    _dataMin = val;
    _dataMax = val;
}

template< typename DataT>
void CT_VirtualGrid2D<DataT>::computeMinMax()
{
    size_t ncells = nCells();
    _dataMin = _data[0];
    _dataMax = _data[0];

    for (size_t i = 1 ; i < ncells ; i++)
    {
        if (_data[i] != NA())
        {
            if (_dataMax==NA() || _data[i] > _dataMax) {_dataMax = _data[i];}
            if (_dataMin==NA() || _data[i] < _dataMin) {_dataMin = _data[i];}
        }
    }
}

template< typename DataT>
QString CT_VirtualGrid2D<DataT>::getType() const
{
    return staticGetType();
}

template< typename DataT>
QString CT_VirtualGrid2D<DataT>::staticGetType()
{
    return CT_AbstractGrid2D::staticGetType() + "/CT_VirtualGrid2D<" + CT_TypeInfo::name<DataT>() + ">";
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    _data[index] = value;

    return true;
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setValue(const size_t col, const size_t lin, const DataT value)
{
    size_t resultIndex;
    if (!index(col, lin, resultIndex)) {return false;}
    return setValueAtIndex(resultIndex, value);
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setValueAtCoords(const double x, const double y, const DataT value)
{
    size_t resultIndex;
    if (!indexAtCoords(x, y, resultIndex)) {return false;}
    return setValueAtIndex(resultIndex, value);
}


template< typename DataT>
DataT CT_VirtualGrid2D<DataT>::valueAtIndex(const size_t index) const
{
    if (index >= nCells()) {return NA();}
    return _data[index];
}

template< typename DataT>
DataT CT_VirtualGrid2D<DataT>::value(const size_t col, const size_t lin) const
{
    size_t resultIndex;
    if (!index(col, lin, resultIndex)) {return NA();}
    return valueAtIndex(resultIndex);
}

template< typename DataT>
DataT CT_VirtualGrid2D<DataT>::dataFromArray(const size_t &x, const size_t &y) const
{
    size_t resultIndex;
    if (!index(x, y, resultIndex)) {return NA();}
    return valueAtIndex(resultIndex);
}

template< typename DataT>
DataT CT_VirtualGrid2D<DataT>::dataFromArray(const size_t &index) const
{
    return _data[index];
}

template< typename DataT>
DataT CT_VirtualGrid2D<DataT>::valueAtCoords(const double x, const double y) const
{
    size_t index;
    if (!indexAtCoords(x, y, index)) {return NA();}
    return valueAtIndex(index);
}


template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setMaxValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}
    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value > currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setMaxValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return setMaxValueAtIndex(indice, value);
}


template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setMinValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}
    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value < currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::setMinValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return setMinValueAtIndex(indice, value);
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::addValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}
    DataT currentValue = _data[index];

    if (currentValue == NA())
    {
        return setValueAtIndex(index, value);
    } else if (value != NA())
    {
        return setValueAtIndex(index, value + currentValue);
    }

    return false;
}

template< typename DataT>
double CT_VirtualGrid2D<DataT>::ratioValueAtIndex(const size_t index) const
{
    if (_dataMax <= _dataMin) {return 1;}
    DataT value = valueAtIndex(index);
    if (value == NA()) {return -1;}
    return (double) (((double)(value - _dataMin))/((double)(_dataMax - _dataMin)));
}

template< typename DataT>
double CT_VirtualGrid2D<DataT>::valueAtIndexAsDouble(const size_t index) const
{
    DataT value = valueAtIndex(index);
    if (value == NA()) {return std::numeric_limits<DataT>::quiet_NaN();}
    return value;
}

template< typename DataT>
QString CT_VirtualGrid2D<DataT>::valueAtIndexAsString(const size_t index) const
{
    DataT value = valueAtIndex(index);
    return QString::number(value);
}

template< typename DataT>
QString CT_VirtualGrid2D<DataT>::NAAsString() const
{
    return QString::number(NA());
}

template< typename DataT>
bool CT_VirtualGrid2D<DataT>::addValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return addValueAtIndex(indice, value);
}

template< typename DataT>
QList<DataT> CT_VirtualGrid2D<DataT>::neighboursValues(const size_t col, const size_t lin, const size_t distance, const bool keepNAs, const CenterMode centermode) const
{
    QList<DataT> liste;

    if (distance < 1) {return liste;}
    if (col >= _dimCol) {return liste;}
    if (lin >= _dimLin) {return liste;}

    size_t firstcol = col-distance;
    size_t lastcol = col+distance;
    size_t firstlin = lin-distance;
    size_t lastlin = lin+distance;

    if (firstcol >= _dimCol) {firstcol = 0;}
    if (lastcol >= _dimCol) {lastcol = _dimCol - 1;}
    if (firstlin >= _dimLin) {firstlin = 0;}
    if (lastlin >= _dimLin) {lastlin = _dimLin - 1;}

    for (size_t xx = firstcol ; xx <= lastcol ; xx++)
    {
        for (size_t yy = firstlin ; yy <= lastlin ; yy++)
        {
            DataT value = this->value(xx, yy);
            if ((xx == col) && (yy == lin)) {
                if (centermode == CM_KeepCenter) {
                    if ((value != NA()) || keepNAs) {liste.append(value);}
                } else if (centermode == CM_NAasCenter) {
                    liste.append(NA());
                }
            } else {
                if ((value != NA()) || keepNAs) {liste.append(value);}
            }
        }
    }

    return liste;
}

#endif // CT_VIRTUALGRID2D_HPP
