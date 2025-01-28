/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_spinbox.h"

#include "settingsinterfaces.h"

CT_SpinBox::CT_SpinBox(int min, int max, int &value, QString description)
{
    _data._min = min;
    _data._max = max;
    _data._value = &value;
    _description = description;

    _spinBoxCreated = nullptr;
}

void CT_SpinBox::saveSettings(SettingsWriterInterface &writer) const
{
    const int pID = writer.addParameter(this, "Value", getValue(), _description);

    writer.addParameterInfo(this, pID, "min", _data._min);
    writer.addParameterInfo(this, pID, "max", _data._max);
}

bool CT_SpinBox::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;

    if(reader.parameter(this, "Value", value)) {
        setWidgetValue(value);
        updateValue();
    }

    return true;
}

QWidget* CT_SpinBox::createWidget(QWidget &parent)
{
    if(_spinBoxCreated == nullptr)
    {
        _spinBoxCreated = new QSpinBox(&parent);

        _spinBoxCreated->setMinimum(_data._min);
        _spinBoxCreated->setMaximum(_data._max);
        _spinBoxCreated->setValue((*_data._value));
    }

    return _spinBoxCreated;
}

void CT_SpinBox::updateValue()
{
    *(_data._value) = _spinBoxCreated->value();
}

bool CT_SpinBox::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _spinBoxCreated->value());
}

QVariant CT_SpinBox::getValue() const
{
    return QVariant((*_data._value));
}

bool CT_SpinBox::setWidgetValue(QVariant val)
{
    bool ok;

    int value = val.toInt(&ok);

    if(ok)
    {
        _spinBoxCreated->setValue(value);
    }

    return ok;
}
