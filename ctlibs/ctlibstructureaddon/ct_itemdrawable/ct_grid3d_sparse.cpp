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

#include "ct_grid3d_sparse.h"

CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV(CT_Grid3D_Sparse)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
CT_Grid3D_Sparse<bool>::CT_Grid3D_Sparse() : SuperClass()
{
    _NAdata = false;
    _initData = false;
    _dataMax = false;
    _dataMin = true;

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template<>
CT_Grid3D_Sparse<bool>::CT_Grid3D_Sparse(const CT_Grid3D_Sparse<bool>& other) : SuperClass(other)
{
    _NAdata = other._NAdata;
    _initData = other._initData;
    _dataMax = other._dataMax;
    _dataMin = other._dataMin;

    if (other._data.size() > 0)
    {
        _data.insert(other._data.begin(), other._data.end());
    }

    _colorMap = other._colorMap;
    _defaultColor = other._defaultColor;
}

template<>
CT_Grid3D_Sparse<bool>::CT_Grid3D_Sparse(double xmin,
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
    _initData = initValue;
    _dataMax = initValue;
    _dataMin = initValue;

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template<>
CT_Grid3D_Sparse<bool>::CT_Grid3D_Sparse(double xmin,
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
    _initData = initValue;
    _dataMax = initValue;
    _dataMin = initValue;

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template<>
void CT_Grid3D_Sparse<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double CT_Grid3D_Sparse<bool>::ratioValueAtIndex(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double CT_Grid3D_Sparse<bool>::valueAtIndexAsDouble(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool CT_Grid3D_Sparse<bool>::setMaxValueAtIndex(const size_t index, const bool value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Grid3D_Sparse<bool>::setMinValueAtIndex(const size_t index, const bool value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Grid3D_Sparse<bool>::addValueAtIndex(const size_t index, const bool value)
{
    bool currentValue = valueAtIndex(index);
    return setValueAtIndex(index, value + currentValue);
}

template<>
QList<bool> CT_Grid3D_Sparse<bool>::neighboursValues(const int colx, const int liny, const int levz, const int distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs)

    QList<bool> liste;

    if (distance < 1) {return liste;}
    if (colx >= _dimx) {return liste;}
    if (liny >= _dimy) {return liste;}
    if (levz >= _dimz) {return liste;}

    if (distance < 1) {return liste;}
    if (colx < 0)     {return liste;}
    if (liny < 0)     {return liste;}
    if (levz < 0)     {return liste;}
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
QString CT_Grid3D_Sparse<bool>::valueAtIndexAsString(const size_t index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}
