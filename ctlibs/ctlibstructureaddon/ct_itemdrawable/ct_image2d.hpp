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

#include "ct_itemdrawable/ct_image2d.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardimage2ddrawmanager.h"

#include <math.h>
#include <typeinfo>
#include <limits>

template< typename DataT>
const CT_StandardImage2DDrawManager<DataT> CT_Image2D<DataT>::IMAGE2D_DRAW_MANAGER;

template< typename DataT>
CT_Image2D<DataT>::CT_Image2D() : SuperClass()
{
    this->setBaseDrawManager(&IMAGE2D_DRAW_MANAGER);
}

template< typename DataT>
CT_Image2D<DataT>::CT_Image2D(double xmin,
                              double ymin,
                              int dimx,
                              int dimy,
                              double resolution,
                              double zlevel,
                              DataT na,
                              DataT initValue) : SuperClass()
{
    _res = resolution;
    _dimx = dimx;
    _dimy = dimy;

    this->setBoundingBox(xmin,
                         ymin,
                         zlevel,
                         xmin + this->_res * this->_dimx,
                         ymin + this->_res * this->_dimy,
                         zlevel);

    this->_minXCoord = xmin;
    this->_minYCoord = ymin;

    this->_level = zlevel;
    this->_NAdata = na;

    this->_data = cv::Mat_<DataT>(this->_dimy, this->_dimx);

    initGridWithValue(initValue);

    this->setBaseDrawManager(&IMAGE2D_DRAW_MANAGER);
}

template<typename DataT>
CT_Image2D<DataT>::CT_Image2D(const CT_Image2D<DataT>& other) : SuperClass(other)
{
    _res = other._res;
    _dimx = other._dimx;
    _dimy = other._dimy;

    Eigen::Vector3d min, max;
    other.boundingBox(min, max);
    this->setBoundingBox(min, max);

    _minXCoord = other._minXCoord;
    _minYCoord = other._minYCoord;
    _level = other._level;
    _NAdata = other._NAdata;
    _dataMax = other._dataMax;
    _dataMin = other._dataMin;

    _data = cv::Mat_<DataT>(_dimy, _dimx);

    for (int xx = 0 ; xx < _dimx ; xx++)
    {
        for (int yy = 0 ; yy < _dimy ; yy++)
        {
            setValue(xx, yy, other.value(xx,yy));
        }
    }

    this->setBaseDrawManager(&IMAGE2D_DRAW_MANAGER);
}

template< typename DataT>
CT_Image2D<DataT>::~CT_Image2D()
{
    _data.release();
}

template< typename DataT>
CT_Image2D<DataT>* CT_Image2D<DataT>::createImage2DFromXYCoords(double xmin,
                                                                   double ymin,
                                                                   double xmax,
                                                                   double ymax,
                                                                   double resolution,
                                                                   double zlevel,
                                                                   DataT na,
                                                                   DataT initValue)
{
    int dimx = int(std::ceil((xmax - xmin)/resolution));
    int dimy = int(std::ceil((ymax - ymin)/resolution));

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= (xmin + dimx * resolution))
    {
        dimx++;
    }

    while (ymax >= (ymin + dimy * resolution))
    {
        dimy++;
    }

    CT_Image2D<DataT>* grid = new CT_Image2D<DataT>(xmin, ymin, dimx, dimy, resolution, zlevel, na, initValue);

    return grid;
}

template< typename DataT>
void CT_Image2D<DataT>::initGridWithValue(const DataT val)
{
    _data.setTo(cv::Scalar::all(val));
    _dataMin = val;
    _dataMax = val;
}

template< typename DataT>
void CT_Image2D<DataT>::computeMinMax()
{   
    _dataMin = NA();
    _dataMax = NA();

    cv::MatIterator_<DataT> it, end;
    for(it = _data.begin(), end = _data.end() ; it != end ; ++it)
    {
        DataT val = *it;

        if (val != NA())
        {
            if (_dataMax == NA() || val > _dataMax) {_dataMax = val;}
            if (_dataMin == NA() || val < _dataMin) {_dataMin = val;}
        }
    }
}

template< typename DataT>
void CT_Image2D<DataT>::setMinMax(DataT min, DataT max)
{
    _dataMin = min;
    _dataMax = max;
}

template< typename DataT>
bool CT_Image2D<DataT>::setValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    int y, x;
    indexToGrid(index, x, y);

    return setValue(x, y, value);
}

template< typename DataT>
bool CT_Image2D<DataT>::setValue(const int x, const int y, const DataT value)
{
    if ((x < 0) || (x >= _dimx) || (y < 0) || (y >= _dimy)) {return false;}
    _data(y, x) = value;
    return true;
}


template< typename DataT>
bool CT_Image2D<DataT>::setValueAtCoords(const double x, const double y, const DataT value)
{
    size_t resultIndex;
    if (!indexAtCoords(x, y, resultIndex)) {return false;}
    return setValueAtIndex(resultIndex, value);
}


template< typename DataT>
DataT CT_Image2D<DataT>::valueAtIndex(const size_t index) const
{
    if (index >= nCells()) {return NA();}

    int y, x;
    indexToGrid(index, x, y);

    return value(x, y);
}

template< typename DataT>
DataT CT_Image2D<DataT>::value(const int x, const int y) const
{
    if ((x < 0) || (x >= _dimx) || (y < 0) || (y >= _dimy)) {return NA();}
    return _data(y, x);
}

template< typename DataT>
DataT CT_Image2D<DataT>::valueAtCoords(const double x, const double y) const
{
    size_t index;
    if (!indexAtCoords(x, y, index)) {return NA();}
    return valueAtIndex(index);
}


template< typename DataT>
bool CT_Image2D<DataT>::setMaxValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    int y, x;
    if (indexToGrid(index, x, y))
    {
        DataT currentValue = _data(y, x);

        if ((currentValue == NA()) || (value > currentValue)) {
            return setValueAtIndex(index, value);
        }
    }

    return false;
}

template< typename DataT>
bool CT_Image2D<DataT>::setMaxValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return setMaxValueAtIndex(indice, value);
}


template< typename DataT>
bool CT_Image2D<DataT>::setMinValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    int y, x;
    if (indexToGrid(index, x, y))
    {
        DataT currentValue = _data(y, x);

        if ((currentValue == NA()) || (value < currentValue)) {
            return setValueAtIndex(index, value);
        }
    }

    return false;
}

template< typename DataT>
bool CT_Image2D<DataT>::setMinValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return setMinValueAtIndex(indice, value);
}

template< typename DataT>
bool CT_Image2D<DataT>::addValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    int y, x;
    if (indexToGrid(index, x, y))
    {
        DataT currentValue = _data(y, x);

        if (currentValue == NA())
        {
            return setValueAtIndex(index, value);
        } else if (value != NA())
        {
            return setValueAtIndex(index, value + currentValue);
        }
    }
    return false;
}

template< typename DataT>
double CT_Image2D<DataT>::ratioValueAtIndex(const size_t index) const
{
    if (_dataMax <= _dataMin) {return 1;}
    DataT value = valueAtIndex(index);
    if (value == NA()) {return -1;}
    return double((value - _dataMin)/(_dataMax - _dataMin));
}

template< typename DataT>
double CT_Image2D<DataT>::valueAtIndexAsDouble(const size_t index) const
{
    DataT value = valueAtIndex(index);
    if (value == NA()) {return NAAsDouble();}
    return value;
}

template< typename DataT>
QString CT_Image2D<DataT>::valueAtIndexAsString(const size_t index) const
{
    DataT value = valueAtIndex(index);
    return QVariant(value).toString();
}

template< typename DataT>
QString CT_Image2D<DataT>::NAAsString() const
{
    return QVariant(NA()).toString();
}

template< typename DataT>
double CT_Image2D<DataT>::NAAsDouble() const
{
    return NA();
}

template< typename DataT>
double CT_Image2D<DataT>::minValueAsDouble() const
{
    return dataMin();
}

template< typename DataT>
double CT_Image2D<DataT>::maxValueAsDouble() const
{
    return dataMax();
}


template< typename DataT>
bool CT_Image2D<DataT>::addValueAtCoords(const double x, const double y, const DataT value)
{
    size_t indice;
    if (!indexAtCoords(x, y, indice)) {return false;}

    return addValueAtIndex(indice, value);
}

template< typename DataT>
QList<DataT> CT_Image2D<DataT>::neighboursValues(const int x, const int y, const int distance, const bool keepNAs, const CenterMode centermode) const
{
    QList<DataT> liste;

    if (distance < 1) {return liste;}
    if (x < 0)        {return liste;}
    if (y < 0)        {return liste;}
    if (x >= _dimx)   {return liste;}
    if (y >= _dimy)   {return liste;}

    int firstcol = x - distance;
    int lastcol  = x + distance;
    int firstlin = y - distance;
    int lastlin  = y + distance;

    if (firstcol < 0)      {firstcol = 0;}
    if (lastcol >= _dimx)  {lastcol = _dimx - 1;}
    if (firstlin < 0)      {firstlin = 0;}
    if (lastlin >= _dimy)  {lastlin = _dimy - 1;}

    for (int xx = firstcol ; xx <= lastcol ; xx++)
    {
        for (int yy = firstlin ; yy <= lastlin ; yy++)
        {
            DataT value = this->value(xx, yy);
            if ((xx == x) && (yy == y)) {
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
