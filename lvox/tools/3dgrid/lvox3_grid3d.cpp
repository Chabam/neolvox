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

#include "lvox3_grid3d.h"

CT_DEFAULT_IA_INIT_TEMPLATED_VALUES(LVOX3_Grid3D)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
void LVOX3_Grid3D<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double LVOX3_Grid3D<bool>::ratioValueAtIndex(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double LVOX3_Grid3D<bool>::valueAtIndexAsDouble(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool LVOX3_Grid3D<bool>::setMaxValueAtIndex(const size_t index, const bool value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool LVOX3_Grid3D<bool>::setMinValueAtIndex(const size_t index, const bool value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool LVOX3_Grid3D<bool>::addValueAtIndex(const size_t index, const bool value)
{
    bool currentValue = this->valueAtIndex(index);
    //adding bools ?
    return setValueAtIndex(index, value + currentValue);
}

template<>
QList<bool> LVOX3_Grid3D<bool>::neighboursValues(const size_t colx, const size_t liny, const size_t levz, const size_t distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs);

    QList<bool> liste;

    if (distance < 1) {return liste;}

    size_t firstColx = colx-distance;
    size_t lastColx = colx+distance;
    size_t firstLiny = liny-distance;
    size_t lastLiny = liny+distance;
    size_t firstLinz = levz-distance;
    size_t lastLinz = levz+distance;

    for (size_t xx = firstColx ; xx <= lastColx ; xx++)
    {
        for (size_t yy = firstLiny ; yy <= lastLiny ; yy++)
        {
            for (size_t zz = firstLinz ; zz <= lastLinz ; zz++)
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
QString LVOX3_Grid3D<bool>::valueAtIndexAsString(const size_t index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}

template<>
QString LVOX3_Grid3D<unsigned long>::valueAtIndexAsString(const size_t index) const
{
    return QString::number(valueAtIndex(index), 'g', 50);
}

template<>
QString LVOX3_Grid3D<unsigned long>::NAAsString() const
{
    return QString::number(NA(), 'g', 50);
}

template<>
QString LVOX3_Grid3D<long int>::valueAtIndexAsString(const size_t index) const
{
    return QString::number(valueAtIndex(index), 'g', 50);
}

template<>
QString LVOX3_Grid3D<long int>::NAAsString() const
{
    return QString::number(NA(), 'g', 50);
}





