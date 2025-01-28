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

#include "ct_image2d.h"

CT_DEFAULT_IA_INIT_TEMPLATED_OPENCV(CT_Image2D)

/////////////////////////////////////////////////////////////////////
/// Specialisations for bool type ///////////////////////////////////
/////////////////////////////////////////////////////////////////////

template<>
void CT_Image2D<bool>::computeMinMax()
{
    _dataMin = false;
    _dataMax = true;
}

template<>
double CT_Image2D<bool>::ratioValueAtIndex(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
double CT_Image2D<bool>::valueAtIndexAsDouble(const size_t index) const
{
    bool value = valueAtIndex(index);
    if (value) {return 1;}
    return 0;
}

template<>
bool CT_Image2D<bool>::setMaxValueAtIndex(const size_t index, const bool value)
{
    if (value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Image2D<bool>::setMinValueAtIndex(const size_t index, const bool value)
{
    if (!value) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template<>
bool CT_Image2D<bool>::addValueAtIndex(const size_t index, const bool value)
{
    if (value) {setValueAtIndex(index, true);}
    return true;
}

template<>
QList<bool> CT_Image2D<bool>::neighboursValues(const int x, const int y, const int distance, const bool keepNAs, const CenterMode centermode) const
{
    Q_UNUSED(keepNAs);

    QList<bool> liste;

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
            bool value = this->value(xx, yy);
            if ((xx == x) && (yy == y))
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
QString CT_Image2D<bool>::valueAtIndexAsString(const size_t index) const
{
    if (valueAtIndex(index)) {return "T";}
    return "F";
}


