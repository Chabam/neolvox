/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Metiers (ENSAM), Cluny, France.
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

#include "ct_grid3d.h"

CT_DEFAULT_IA_INIT_TEMPLATED_VALUES(CT_Grid3D)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
CT_Grid3D<bool>::CT_Grid3D() : SuperClass()
{
    _NAdata = false;
    _dataMax = true;
    _dataMin = false;

    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template<>
CT_Grid3D<bool>::CT_Grid3D(const CT_Grid3D<bool>& other) : SuperClass(other)
{
    _NAdata = other._NAdata;
    _dataMax = other._dataMax;
    _dataMin = other._dataMin;

    _data.resize(other._data.size());
    for (size_t i = 0 ; i < other._data.size() ; i++)
    {
        _data[i] = other._data[i];
    }

    _colorMap = other._colorMap;
    _defaultColor = other._defaultColor;
}


template<>
CT_Grid3D<bool>::CT_Grid3D(double xmin,
                            double ymin,
                            double zmin,
                            int dimx,
                            int dimy,
                            int dimz,
                            double resolution,
                            bool na,
                            bool initValue) : SuperClass(xmin, ymin, zmin, dimx, dimy, dimz, resolution)
{

    _NAdata = na;
    _dataMax = true;
    _dataMin = false;

    _data.resize(nCells());
    initGridWithValue(initValue);

    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template<>
CT_Grid3D<bool>::CT_Grid3D(double xmin,
                            double ymin,
                            double zmin,
                            double xmax,
                            double ymax,
                            double zmax,
                            double resolution,
                            bool na,
                            bool initValue) : SuperClass(xmin, ymin, zmin, xmax, ymax, zmax, resolution)
{

    _NAdata = na;
    _dataMax = true;
    _dataMin = false;

    _data.resize(nCells());
    initGridWithValue(initValue);

    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template<>
void CT_Grid3D<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double CT_Grid3D<bool>::ratioValueAtIndex(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double CT_Grid3D<bool>::valueAtIndexAsDouble(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool CT_Grid3D<bool>::setMaxValueAtIndex(const size_t index, const bool value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Grid3D<bool>::setMinValueAtIndex(const size_t index, const bool value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Grid3D<bool>::addValueAtIndex(const size_t index, const bool value)
{
    bool currentValue = _data[index];
    return setValueAtIndex(index, value + currentValue);
}

template<>
QList<bool> CT_Grid3D<bool>::neighboursValues(const int colx, const int liny, const int levz, const int distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs);

    QList<bool> liste;

    if (distance < 1)  {return liste;}

    if (distance < 1)  {return liste;}
    if (colx < 0)      {return liste;}
    if (liny < 0)      {return liste;}
    if (levz < 0)      {return liste;}
    if (colx >= _dimx) {return liste;}
    if (liny >= _dimy) {return liste;}
    if (levz >= _dimz) {return liste;}

    int firstColx = colx - distance;
    int lastColx = colx + distance;
    int firstLiny = liny - distance;
    int lastLiny = liny + distance;
    int firstLinz = levz - distance;
    int lastLinz = levz + distance;

    if (firstColx < 0)     {firstColx = 0;}
    if (lastColx >= _dimx) {lastColx = _dimx - 1;}
    if (firstLiny < 0)     {firstLiny = 0;}
    if (lastLiny >= _dimy) {lastLiny = _dimy - 1;}
    if (firstLinz <0)      {firstLinz = 0;}
    if (lastLinz >= _dimz) {lastLinz = _dimz - 1;}

    for (int xx = firstColx ; xx <= lastColx ; xx++)
    {
        for (int yy = firstLiny ; yy <= lastLiny ; yy++)
        {
            for (int zz = firstLinz ; zz <= lastLinz ; zz++)
            {
                bool value = this->value(xx, yy, zz);

                if ((xx == colx) && (yy == liny) && (zz == levz))
                {
                    if (centermode == CM_KeepCenter) {liste.append(value);}
                } else {
                    liste.append(value);
                }
            }
        }
    }

    return liste;
}

template<>
QString CT_Grid3D<bool>::valueAtIndexAsString(const size_t index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}

template<>
QString CT_Grid3D<unsigned long>::valueAtIndexAsString(const size_t index) const
{
    return QString::number(valueAtIndex(index), 'g', 50);
}

template<>
QString CT_Grid3D<unsigned long>::NAAsString() const
{
    return QString::number(NA(), 'g', 50);
}

template<>
QString CT_Grid3D<long int>::valueAtIndexAsString(const size_t index) const
{
    return QString::number(valueAtIndex(index), 'g', 50);
}

template<>
QString CT_Grid3D<long int>::NAAsString() const
{
    return QString::number(NA(), 'g', 50);
}



