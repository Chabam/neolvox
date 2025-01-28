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

#include "ct_itemdrawable/ct_profile.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardprofiledrawmanager.h"

#include <math.h>
#include <typeinfo>
#include <limits>

template< typename DataT>
const CT_StandardProfileDrawManager<DataT> CT_Profile<DataT>::ABSPROFILE_DRAW_MANAGER;

template< typename DataT>
CT_Profile<DataT>::CT_Profile() : SuperClass()
{
    _res = 1;
    _dim = 0;
    _NAdata = -1;
    _dataMax = -1;
    _dataMin = -1;

    setBaseDrawManager(&ABSPROFILE_DRAW_MANAGER);
}



template< typename DataT>
CT_Profile<DataT>::CT_Profile(double xmin,
                              double ymin,
                              double zmin,
                              double xdir,
                              double ydir,
                              double zdir,
                              size_t dim,
                              double resolution,
                              DataT na,
                              DataT initValue) : SuperClass()
{
    _direction(0) = xdir;
    _direction(1) = ydir;
    _direction(2) = zdir;

    _direction.normalize();

    _res = resolution;
    _dim = dim;
    _maxLength = _dim * _res;

    _NAdata = na;

    setBoundingBox(xmin,
                   ymin,
                   zmin,
                   xmin + _res * _dim * _direction.x(),
                   ymin + _res * _dim * _direction.y(),
                   zmin + _res * _dim * _direction.z());

    _data.resize(nCells());

    initGridWithValue(initValue);

    setBaseDrawManager(&ABSPROFILE_DRAW_MANAGER);
}

template< typename DataT>
CT_Profile<DataT>* CT_Profile<DataT>::createProfileFromSegment(double xmin,
                                                               double ymin,
                                                               double zmin,
                                                               double xmax,
                                                               double ymax,
                                                               double zmax,
                                                               double resolution,
                                                               DataT na,
                                                               DataT initValue)
{
    double xdir = xmax - xmin;
    double ydir = ymax - ymin;
    double zdir = zmax - zmin;

    double length = sqrt(xdir*xdir + ydir*ydir + zdir*zdir);

    int dim = ceil(length/resolution);

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (length >= dim*resolution)
    {
        dim++;
    }

    CT_Profile<DataT>* grid = new CT_Profile(xmin, ymin, zmin, xdir, ydir, zdir, dim, resolution, na, initValue);

    return grid;
}

template< typename DataT>
CT_Profile<DataT>::~CT_Profile()
{
    _data.clear();
}

template< typename DataT>
void CT_Profile<DataT>::initGridWithValue(const DataT &val)
{
    for (size_t  i = 0 ; i < nCells() ; i++)
    {
        _data[i] = val;
    }
    _dataMin = val;
    _dataMax = val;
}


template< typename DataT>
void CT_Profile<DataT>::setValueAtIndexFromDouble(const size_t &index, const double &value)
{
    setValueAtIndex(index, (DataT) value);
}

template< typename DataT>
void CT_Profile<DataT>::computeMinMax()
{
    size_t ncells = nCells();
    if (ncells > 0) {
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
}

template< typename DataT>
bool CT_Profile<DataT>::setValueAtIndex(const size_t &index, const DataT &value)
{
    if (index >= nCells()) {return false;}
    _data[index] = value;

    return true;
}

template< typename DataT>
bool CT_Profile<DataT>::setValueAtLength(const double &l, const DataT &value)
{
    size_t index;
    if (indexForLength(l, index))
    {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::setValueForXYZ(const double &x, const double &y, const double &z, const DataT &value)
{
    size_t index;
    if (indexForXYZ(x, y, z, index))
    {
        return setValueAtIndex(index, value);
    }
    return false;
}


template< typename DataT>
DataT CT_Profile<DataT>::valueAtIndex(const size_t &index) const
{
    if (index >= nCells()) {return NA();}
    return _data[index];
}

template< typename DataT>
DataT CT_Profile<DataT>::valueAtLength(const double &l) const
{
    size_t index;
    if (indexForLength(l, index))
    {
        return valueAtIndex(index);
    }
    return NA();
}


template< typename DataT>
double CT_Profile<DataT>::ratioValueAtIndex(const size_t &index) const
{
    if (_dataMax <= _dataMin) {return 1;}
    DataT value = valueAtIndex(index);
    if (value == NA()) {return -1;}
    return (double) (((double)(value - _dataMin))/((double)(_dataMax - _dataMin)));
}

template< typename DataT>
double CT_Profile<DataT>::valueAtIndexAsDouble(const size_t &index) const
{
    DataT value = valueAtIndex(index);
    if (value == NA()) {return std::numeric_limits<DataT>::quiet_NaN();}
    return value;
}

template< typename DataT>
QString CT_Profile<DataT>::valueAtIndexAsString(const size_t &index) const
{
    DataT value = valueAtIndex(index);
    return QString::number(value);
}

template< typename DataT>
QString CT_Profile<DataT>::NAAsString() const
{
    return QString::number(NA());
}

template< typename DataT>
DataT CT_Profile<DataT>::valueForXYZ(const double &x, const double &y, const double &z) const
{
    size_t index;
    if (indexForXYZ(x, y, z, index))
    {
        return valueAtIndex(index);
    }
    return NA();
}


template< typename DataT>
bool CT_Profile<DataT>::setMaxValueAtIndex(const size_t &index, const DataT &value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value > currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::setMaxValueAtLength(const double &l, const DataT &value)
{
    size_t index;
    if (indexForLength(l, index))
    {
        return setMaxValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::setMaxValueForXYZ(const double &x, const double &y, const double &z, const DataT &value)
{
    size_t index;
    if (indexForXYZ(x, y, z, index))
    {
        return setMaxValueAtIndex(index, value);
    }
    return false;
}


template< typename DataT>
bool CT_Profile<DataT>::setMinValueAtIndex(const size_t &index, const DataT &value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value < currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::setMinValueAtLength(const double &l, const DataT &value)
{
    size_t index;
    if (indexForLength(l, index))
    {
        return setMinValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::setMinValueForXYZ(const double &x, const double &y, const double &z, const DataT &value)
{
    size_t index;
    if (indexForXYZ(x, y, z, index))
    {
        return setMinValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::addValueAtIndex(const size_t &index, const DataT &value)
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
bool CT_Profile<DataT>::addValueAtLength(const double &l, const DataT &value)
{
    size_t index;
    if (indexForLength(l, index))
    {
        return addValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool CT_Profile<DataT>::addValueForXYZ(const double &x, const double &y, const double &z, const DataT &value)
{
    size_t index;
    if (indexForXYZ(x, y, z, index))
    {
        return addValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
QList<DataT> CT_Profile<DataT>::neighboursValues(const size_t &index, const size_t &distance, const bool keepNAs, const CenterMode centermode) const
{
    QList<DataT> liste;

    if (distance < 1) {return liste;}
    if (index >= _dim) {return liste;}

    size_t firstCol = index-distance;
    size_t lastCol = index+distance;

    if (firstCol >= _dim) {firstCol = 0;}
    if (lastCol >= _dim) {lastCol = _dim - 1;}


    for (size_t ii = firstCol ; ii <= lastCol ; ii++)
    {

        DataT val = valueAtIndex(ii);
        if (ii == index) {
            if (centermode == CM_KeepCenter) {
                if ((val != NA()) || keepNAs) {liste.append(val);}
            } else if (centermode == CM_NAasCenter) {
                liste.append(NA());
            }
        } else {
            if ((val != NA()) || keepNAs) {liste.append(val);}
        }
    }

    return liste;
}

template< typename DataT>
double CT_Profile<DataT>::getOtsuThreshold(CT_Profile<DataT> *outProfileLow, CT_Profile<DataT> *outProfileHigh) {
    double total = 0;
    double sum = 0;

    for (int i = 0; i < static_cast<int>(nCells()); ++i)
    {
        sum += lengthForIndex(i) * valueAtIndex(i);
        total += valueAtIndex(i);
    }
    double sumB = 0;
    double wB = 0;
    double wF = 0;
    double mB;
    double mF;
    double max = 0.0;
    double between = 0.0;
    double threshold1 = 0.0;
    double threshold2 = 0.0;

    for (int i = 0; i < static_cast<int>(nCells()); ++i) {
        wB += valueAtIndex(i);
        if (wB == 0)
            continue;
        wF = total - wB;
        if (wF == 0)
            break;
        sumB += lengthForIndex(i) * valueAtIndex(i);
        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);
        if ( between >= max ) {
            threshold1 = lengthForIndex(i);
            if ( between > max ) {
                threshold2 = lengthForIndex(i);
            }
            max = between;
        }
    }
    double threshold = ( threshold1 + threshold2 ) / 2.0;

    // create filtered profiles
    if (outProfileLow != nullptr || outProfileHigh != nullptr)
    {
        for (int i = 0; i < static_cast<int>(nCells()); ++i)
        {
            double length = lengthForIndex(i);
            if (outProfileLow  != nullptr && length >= threshold)
            {
                outProfileLow->setValueAtIndex(i, 0);
            } else {
                outProfileLow->setValueAtIndex(i, valueAtIndex(i));
            }

            if (outProfileHigh != nullptr && length < threshold)
            {
                outProfileHigh->setValueAtIndex(i, 0);
            } else {
                outProfileHigh->setValueAtIndex(i, valueAtIndex(i));
            }
        }

        if (outProfileLow  != nullptr)  {outProfileLow->computeMinMax();}
        if (outProfileHigh  != nullptr) {outProfileHigh->computeMinMax();}
    }

    return threshold;
}

template< typename DataT>
void CT_Profile<DataT>::standardize()
{
    DataT sum = 0;
    for (int i = 0; i < nCells(); ++i)
    {
        DataT val = valueAtIndex(i);
        if (val != NA()) {sum += val;}
    }

    for (int i = 0; i < nCells(); ++i)
    {
        DataT val = valueAtIndex(i);
        if (val != NA()) {setValueAtIndex(i, (DataT) ((double)val / (double)sum));}
    }
}

