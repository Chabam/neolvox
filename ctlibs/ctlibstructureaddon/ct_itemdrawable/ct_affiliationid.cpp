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

#include "ct_itemdrawable/ct_affiliationid.h"

size_t CT_AffiliationID::LASTID = 0;

CT_DEFAULT_IA_INIT(CT_AffiliationID)

CT_AffiliationID::CT_AffiliationID() : SuperClass()
{
    _value = LASTID++;
}

CT_AffiliationID::CT_AffiliationID(size_t value) : SuperClass(),
    _value(value)
{
}

void CT_AffiliationID::setValue(size_t value)
{
    _value = value;
}

size_t CT_AffiliationID::getValue() const
{
    return _value;
}

QString CT_AffiliationID::getValueAsString() const
{
    return QString().number(_value);
}

double CT_AffiliationID::getDoubleValue()
{
    return _value;
}
