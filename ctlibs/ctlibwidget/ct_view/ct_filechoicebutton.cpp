/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#include "ct_filechoicebutton.h"
#include "settingsinterfaces.h"

#include <QFileDialog>
#include <QString>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#define QT_SKIP_EMPTY_PARTS Qt::SkipEmptyParts
#else
#define QT_SKIP_EMPTY_PARTS QString::SkipEmptyParts
#endif

CT_FileChoiceButton::CT_FileChoiceButton(QString btlab, CT_FileChoiceButton::NeededFileType filetype, QString fileFilter, QStringList &value, QString description)
{
    _data._buttonLabel = btlab;
    _data._fileType = filetype;
    _data._fileFilter = fileFilter;
    _data._value = &value;
    _description = description;

    _widgetCreated = nullptr;
    _scrollArea = nullptr;
    _verticalLayout = nullptr;
    _pushButtonCreated = nullptr;
    _labelCreated = nullptr;
}

void CT_FileChoiceButton::saveSettings(SettingsWriterInterface &writer) const
{
    writer.setCallerDescription(this, _description);

    foreach(const QString& path, (*_data._value))
        writer.addParameterPath(this, "File", path);
}

bool CT_FileChoiceButton::restoreSettings(SettingsReaderInterface &reader)
{
    QString path;

    int n = reader.parameterPathCount(this, "File");

    while((n > 0) && reader.parameterPath(this, "File", path))
    {
        _data._value->append(path);
        --n;
    }

    setWidgetValue(*_data._value);

    return true;
}

QWidget* CT_FileChoiceButton::createWidget(QWidget &parent)
{
    if(_pushButtonCreated == nullptr)
    {
        _widgetCreated = new QWidget(&parent);
        _pushButtonCreated = new QPushButton(_widgetCreated);
        _pushButtonCreated->setText(QString(" %1").arg(_data._buttonLabel));
        _pushButtonCreated->setStyleSheet("text-align:left;");
        _pushButtonCreated->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

        _labelCreated = new QLabel(_widgetCreated);
        _labelCreated->setWordWrap(true);
        _labelCreated->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        _labelCreated->setMinimumWidth(600);

        _labelCreated->setText("");
        for (int i = 0 ; i < (*_data._value).size() ; ++i)
        {
            _labelCreated->setText(QString("%1\n%2").arg(_labelCreated->text()).arg((*_data._value).at(i)));        
            emit valueModified(_labelCreated->text());
        }

        _verticalLayout = new QVBoxLayout(_widgetCreated);
        _verticalLayout->setSpacing(0);

        _verticalLayout->addWidget(_pushButtonCreated);
        _verticalLayout->addWidget(_labelCreated);

        connect(_pushButtonCreated, SIGNAL(clicked()),this,SLOT(chooseFile()));

    }

    return _widgetCreated;
}

void CT_FileChoiceButton::updateValue()
{
    *(_data._value) = _labelCreated->text().split("\n", QT_SKIP_EMPTY_PARTS);
}

bool CT_FileChoiceButton::isValueAndWidgetValueDifferent() const
{
    return ((*_data._value) != _labelCreated->text().split("\n", QT_SKIP_EMPTY_PARTS));
}

QVariant CT_FileChoiceButton::getValue() const
{
    return QVariant((*_data._value));
}

QVariant CT_FileChoiceButton::getValueAsString() const
{
    return _labelCreated->text();
}

bool CT_FileChoiceButton::setWidgetValueAsString(QVariant val)
{
    QString value = val.toString();
    _labelCreated->setText(value);
    emit valueModified(_labelCreated->text());
    return true;
}

bool CT_FileChoiceButton::setWidgetValue(QVariant val)
{
    bool ok = true;

    QStringList value = val.toStringList();

    if(ok)
    {
        _labelCreated->setText("");
        for (int i = 0 ; i < value.size() ; ++i)
        {
            _labelCreated->setText(QString("%1\n%2").arg(_labelCreated->text()).arg(value.at(i)));
            emit valueModified(_labelCreated->text());
        }

    }

    return ok;
}

void CT_FileChoiceButton::chooseFile()
{
    _labelCreated->setText("");

    if (_data._fileType == CT_FileChoiceButton::OneExistingFile)
    {
        QString s = QFileDialog::getOpenFileName(_widgetCreated, tr("Choisir un fichier"), "", _data._fileFilter);
        if (QFile(s).exists())
        {
            _labelCreated->setText(s);
        }

    } else if (_data._fileType == CT_FileChoiceButton::OneOrMoreExistingFiles)
    {
        QStringList s = QFileDialog::getOpenFileNames(_widgetCreated, tr("Choisir un ou plusieurs fichiers"), "", _data._fileFilter);
        for (int i = 0 ; i < s.size() ; i++)
        {
            if (QFile(s.at(i)).exists())
            {
                _labelCreated->setText(QString("%1\n%2").arg(_labelCreated->text()).arg(s.at(i)));
            }
        }


    } else if (_data._fileType == CT_FileChoiceButton::OneExistingFolder)
    {
        QString s = QFileDialog::getExistingDirectory(_widgetCreated, tr("Choisir un répertoire"), _data._value->isEmpty() ? QString() : _data._value->first());
        if (QDir(s).exists())
        {
            _labelCreated->setText(s);
        }
    } else if (_data._fileType == CT_FileChoiceButton::OneNewFile)
    {
        QString s = QFileDialog::getSaveFileName(_widgetCreated, tr("Fichier à créer"), "", _data._fileFilter);
        _labelCreated->setText(s);
    }

    emit valueModified(_labelCreated->text());
}

void CT_FileChoiceButton::setFormat(QString format)
{
    _data._fileFilter = format;
}

