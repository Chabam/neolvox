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

#include "ct_checkbox.h"
#include "settingsinterfaces.h"

CT_CheckBox::CT_CheckBox(QString text, bool &value, QString description)
{
    _data._value = &value;
    _data._text = text;
    _description = description;

    _checkBoxCreated = nullptr;
}

void CT_CheckBox::saveSettings(SettingsWriterInterface &writer) const
{
    writer.setCallerDescription(this, _data._text);
    writer.addParameter(this, "Checked", getValue(), _description);
}

bool CT_CheckBox::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;

    if(reader.parameter(this, "Checked", value)) {
        setWidgetValue(value);
        updateValue();
    }

    return true;
}

QWidget* CT_CheckBox::createWidget(QWidget &parent)
{
    if(_checkBoxCreated == nullptr)
    {
        _checkBoxCreated = new QCheckBox(_data._text, &parent);
        setWidgetValue(*(_data._value));
    }

    return _checkBoxCreated;
}

void CT_CheckBox::updateValue()
{
    *(_data._value) = _checkBoxCreated->isChecked();
}

bool CT_CheckBox::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _checkBoxCreated->isChecked());
}

QVariant CT_CheckBox::getValue() const
{
    return QVariant((*_data._value));
}

bool CT_CheckBox::setWidgetValue(QVariant val)
{
    if(val.toBool() != _checkBoxCreated->isChecked())
    {
        _checkBoxCreated->setChecked(val.toBool());
        emit valueChanged(_checkBoxCreated->isChecked());
    }

    return true;
}

void CT_CheckBox::setEnabled(bool enabled)
{
    _checkBoxCreated->setEnabled(enabled);
}

void CT_CheckBox::setChecked(bool checked)
{
    setWidgetValue(checked);
}
