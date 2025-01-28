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

#include "ct_profile.h"

CT_DEFAULT_IA_INIT_TEMPLATED_VALUES(CT_Profile)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
void CT_Profile<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double CT_Profile<bool>::ratioValueAtIndex(const size_t &index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double CT_Profile<bool>::valueAtIndexAsDouble(const size_t &index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool CT_Profile<bool>::setMaxValueAtIndex(const size_t &index, const bool &value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Profile<bool>::setMinValueAtIndex(const size_t &index, const bool &value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Profile<bool>::addValueAtIndex(const size_t &index, const bool &value)
{
    bool currentValue = _data[index];
    return setValueAtIndex(index, value + currentValue);
}

template<>
QList<bool> CT_Profile<bool>::neighboursValues(const size_t &index, const size_t &distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs);

    QList<bool> liste;

    if (distance < 1) {return liste;}
    if (index >= _dim) {return liste;}

    size_t firstCol = index-distance;
    size_t lastCol = index+distance;

    if (firstCol >= _dim) {firstCol = 0;}
    if (lastCol >= _dim) {lastCol = _dim - 1;}


    for (size_t ii = firstCol ; ii <= lastCol ; ii++)
    {

        bool val = valueAtIndex(ii);
        if (ii == index) {
            if (centermode == CM_KeepCenter) {liste.append(val);}
        } else {
            liste.append(val);
        }
    }

    return liste;
}

template<>
QString CT_Profile<bool>::valueAtIndexAsString(const size_t &index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}


