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

#include "ct_doublespinbox.h"
#include "settingsinterfaces.h"

CT_DoubleSpinBox::CT_DoubleSpinBox(double min,
                                   double max,
                                   int nDecimals,
                                   double &value,
                                   double multValue, QString description)
{
    _data._min = min;
    _data._max = max;
    _data._nDecimals = nDecimals;
    _data._value = &value;
    _description = description;

    if(multValue == 0)
    {
        multValue = 1;
    }

    _data._multValue = multValue;

    _doubleSpinBoxCreated = nullptr;
}

void CT_DoubleSpinBox::saveSettings(SettingsWriterInterface &writer) const
{
    const int pID = writer.addParameter(this, "DoubleValue", getValue(), _description);

    writer.addParameterInfo(this, pID, "min", _data._min);
    writer.addParameterInfo(this, pID, "max", _data._max);
    writer.addParameterInfo(this, pID, "nDecimal", _data._nDecimals);
}

bool CT_DoubleSpinBox::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;

    if(reader.parameter(this, "DoubleValue", value)) {
        setWidgetValue(value);
        updateValue();
    }

    return true;
}

QWidget* CT_DoubleSpinBox::createWidget(QWidget &parent)
{
    if(_doubleSpinBoxCreated == nullptr)
    {
        _doubleSpinBoxCreated = new QDoubleSpinBox(&parent);

        _doubleSpinBoxCreated->setMinimum(_data._min);
        _doubleSpinBoxCreated->setMaximum(_data._max);
        _doubleSpinBoxCreated->setDecimals(_data._nDecimals);
        _doubleSpinBoxCreated->setValue((*_data._value) * _data._multValue);
    }

    return _doubleSpinBoxCreated;
}

void CT_DoubleSpinBox::updateValue()
{
    *(_data._value) = (_doubleSpinBoxCreated->value() / _data._multValue);
}

bool CT_DoubleSpinBox::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _doubleSpinBoxCreated->value());
}

QVariant CT_DoubleSpinBox::getValue() const
{
    return QVariant((*_data._value));
}

bool CT_DoubleSpinBox::setWidgetValue(QVariant val)
{
    bool ok;

    double value = val.toDouble(&ok);

    if(ok)
    {
        _doubleSpinBoxCreated->setValue(value * _data._multValue);
    }

    return ok;
}
