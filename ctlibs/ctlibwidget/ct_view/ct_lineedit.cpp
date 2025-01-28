/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre Piboule (ONF)

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

#include "ct_lineedit.h"

#include "settingsinterfaces.h"

CT_LineEdit::CT_LineEdit(QString &value, QString description)
{
    _data._value = &value;
    _description = description;

    _lineEditCreated = nullptr;
}

void CT_LineEdit::saveSettings(SettingsWriterInterface &writer) const
{
    writer.addParameter(this, "Text", getValue());
}

bool CT_LineEdit::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;
    if(reader.parameter(this, "Text", value)) {
        setWidgetValue(value);
        updateValue();
    }

    return true;
}

QWidget* CT_LineEdit::createWidget(QWidget &parent)
{
    if(_lineEditCreated == nullptr)
    {
        _lineEditCreated = new QLineEdit(*(_data._value), &parent);
    }

    return _lineEditCreated;
}

void CT_LineEdit::updateValue()
{
    *(_data._value) = _lineEditCreated->text();
}

bool CT_LineEdit::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _lineEditCreated->text());
}

QVariant CT_LineEdit::getValue() const
{
    return QVariant((*_data._value));
}

bool CT_LineEdit::setWidgetValue(QVariant val)
{
    _lineEditCreated->setText(val.toString());

    return true;
}
