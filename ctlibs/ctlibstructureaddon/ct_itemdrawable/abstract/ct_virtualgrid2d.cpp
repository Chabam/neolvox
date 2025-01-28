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

#include "ct_virtualgrid2d.h"

template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<bool>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<float>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<double>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<long>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<unsigned long>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<qint8>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<quint8>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<qint16>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<quint16>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<qint32>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<quint32>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<qint64>)
template<>
CT_DEFAULT_IA_INIT(CT_VirtualGrid2D<quint64>)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
void CT_VirtualGrid2D<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double CT_VirtualGrid2D<bool>::ratioValueAtIndex(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double CT_VirtualGrid2D<bool>::valueAtIndexAsDouble(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool CT_VirtualGrid2D<bool>::setMaxValueAtIndex(const size_t index, const bool value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_VirtualGrid2D<bool>::setMinValueAtIndex(const size_t index, const bool value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_VirtualGrid2D<bool>::addValueAtIndex(const size_t index, const bool value)
{
    bool currentValue = _data[index];
    return setValueAtIndex(index, value + currentValue);
}

template<>
QList<bool> CT_VirtualGrid2D<bool>::neighboursValues(const size_t colx, const size_t liny, const size_t distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs);

    QList<bool> liste;

    if (distance < 1) {return liste;}

    size_t firstColx = colx-distance;
    size_t lastColx = colx+distance;
    size_t firstLiny = liny-distance;
    size_t lastLiny = liny+distance;

    for (size_t xx = firstColx ; xx <= lastColx ; xx++)
    {
        for (size_t yy = firstLiny ; yy <= lastLiny ; yy++)
        {
            bool value = this->value(xx, yy);
            if ((xx == colx) && (yy == liny))
            {
                if (centermode == CM_KeepCenter) {liste.append(value);}
            } else {
                liste.append(value);
            }
        }
    }

    return liste;
}

template<>
QString CT_VirtualGrid2D<bool>::valueAtIndexAsString(const size_t index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}
