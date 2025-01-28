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

#include "ct_asciifilechoicebutton.h"

#include "settingsinterfaces.h"

#include <QFileDialog>
#include <QDebug>

CT_AsciiFileChoiceButton::CT_AsciiFileChoiceButton(QString btlab,
                                                   QString fileFilter,
                                                   bool autoDetect,
                                                   const QStringList &neededFields,
                                                   QString &fileName,
                                                   bool &header,
                                                   QString &separator,
                                                   QString &decimal,
                                                   QLocale &locale,
                                                   int & skip,
                                                   QMap<QString, int> &columns,
                                                   QString description)
{
    _buttonLabel = btlab;
    _fileFilter = fileFilter;
    _autoDetect = autoDetect;
    _columnsAsString = "";

    _fileName = &fileName;
    _header = &header;
    _separator = &separator;
    _decimal = &decimal;
    _locale = &locale;
    _skip = &skip;
    _columns = &columns;

    _description = description;

    _widgetCreated = nullptr;
    _scrollArea = nullptr;
    _verticalLayout = nullptr;
    _pushButtonCreated = nullptr;
    _labelCreated = nullptr;

    _dialog = new CT_TextFileConfigurationDialog(neededFields, nullptr, *_fileName, _autoDetect);
    _dialog->setFileExtensionAccepted(QList<FileFormat>() << FileFormat(fileFilter, ""));
}

CT_AsciiFileChoiceButton::CT_AsciiFileChoiceButton(QString btlab,
                                                   QString fileFilter,
                                                   bool autoDetect,
                                                   QList<CT_TextFileConfigurationFields> &neededFields,
                                                   QString &fileName,
                                                   bool &header,
                                                   QString &separator,
                                                   QString &decimal,
                                                   QLocale &locale,
                                                   int & skip,
                                                   QMap<QString, int> &columns,
                                                   QString description)
{
    _buttonLabel = btlab;
    _fileFilter = fileFilter;
    _autoDetect = autoDetect;
    _columnsAsString = "";

    _fileName = &fileName;
    _header = &header;
    _separator = &separator;
    _decimal = &decimal;
    _locale = &locale;
    _skip = &skip;
    _columns = &columns;

    _description = description;

    _widgetCreated = nullptr;
    _scrollArea = nullptr;
    _verticalLayout = nullptr;
    _pushButtonCreated = nullptr;
    _labelCreated = nullptr;

    _dialog = new CT_TextFileConfigurationDialog(neededFields, nullptr, *_fileName, _autoDetect);
    _dialog->setFileExtensionAccepted(QList<FileFormat>() << FileFormat(fileFilter, ""));
}

CT_AsciiFileChoiceButton::~CT_AsciiFileChoiceButton()
{
    delete _dialog;
}

void CT_AsciiFileChoiceButton::saveSettings(SettingsWriterInterface &writer) const
{
    const int epgSID = writer.createExtraParametersGroup(this, "PossibleValue", tr("Séparateurs de colonne possibles"));
    const int epgSDID = writer.createExtraParametersGroup(this, "PossibleValue", tr("Séparateurs décimal possibles"));

    writer.addParameterPath(this, "FileName", *_fileName);
    writer.addParameter(this, "HeaderPresent", *_header);
    writer.linkParameterAndExtraParametersGroup(this, writer.addParameter(this, "Separator", *_separator), epgSID);
    writer.linkParameterAndExtraParametersGroup(this, writer.addParameter(this, "DecimalSeparator", *_decimal), epgSDID);
    writer.addParameter(this, "NbLinesToSkip", *_skip);

    QMapIterator<QString, int> it(*_columns);
    int nbColumns = _columns->size();

    writer.addParameter(this, "NbColumns", nbColumns);

    while(it.hasNext()) {
        it.next();

        const int pID = writer.addParameter(this, "Column", it.key());
        writer.addParameterInfo(this, pID, "Position", it.value());
    }

    writer.addExtraParametersValueToGroup(this, QLocale(QLocale::French, QLocale::France).name(), epgSDID);
    writer.addExtraParametersValueToGroup(this, QLocale(QLocale::English, QLocale::UnitedKingdom).name(), epgSDID);

    const QStringList separators = CT_TextFileConfigurationDialog::separatorsAvailable();

    foreach (const QString& sep, separators) {
        writer.addExtraParametersValueToGroup(this, sep, epgSID);
    }
}

bool CT_AsciiFileChoiceButton::restoreSettings(SettingsReaderInterface &reader)
{
    QVariant value;
    QString path;

    if(reader.parameterPath(this, "FileName", path))
        (*_fileName) = path;

    if(reader.parameter(this, "HeaderPresent", value))
        (*_header) = value.toBool();

    if(reader.parameter(this, "Separator", value))
        (*_separator) = value.toString();

    if(reader.parameter(this, "DecimalSeparator", value))
        (*_decimal) = value.toString();

    if(reader.parameter(this, "NbLinesToSkip", value))
        (*_skip) = value.toInt();

    int nbColumns = 0;
    if(reader.parameter(this, "NbColumns", value))
        (nbColumns) = value.toInt();

    for (int i = 0 ; i < nbColumns ; i++)
    {
        int pID = reader.parameter(this, "Column", value);

        const QString columnName = value.toString();

        if(reader.parameterInfo(this, pID, "Position", value, 0))
            _columns->insert(columnName, value.toInt());
    }

    _dialog->setHeader(*_header);
    _dialog->setSeparator(*_separator);
    _dialog->setDecimal(*_decimal);
    *_locale = QLocale(_dialog->getQLocaleName());

    _dialog->setNLinesToSkip(*_skip);
    _dialog->setFileNameWithPath(*_fileName);
    _dialog->setFieldColumnsSelected(*_columns);
    _labelCreated->setText(*_fileName);

    _columnsAsString = _dialog->getFieldColumnsSelectedAsString(*_columns);

    return true;
}

QWidget* CT_AsciiFileChoiceButton::createWidget(QWidget &parent)
{
    if(_pushButtonCreated == nullptr)
    {
        _widgetCreated = new QWidget(&parent);
        _pushButtonCreated = new QPushButton(_widgetCreated);
        _pushButtonCreated->setText(_buttonLabel);

        _scrollArea = new QScrollArea(_widgetCreated);
        _scrollArea->setFrameShape(QScrollArea::NoFrame);
        _scrollArea->setWidgetResizable(true);
        _scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);

        _labelCreated = new QLabel(_scrollArea);

        _labelCreated->setText(*_fileName);

        _verticalLayout = new QVBoxLayout(_widgetCreated);
        _verticalLayout->setSpacing(0);

        _verticalLayout->addWidget(_pushButtonCreated);
        _verticalLayout->addWidget(_scrollArea);
        _scrollArea->setWidget(_labelCreated);
        _labelCreated->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        connect(_pushButtonCreated, SIGNAL(clicked()),this,SLOT(chooseFile()));
    }

    return _widgetCreated;
}

void CT_AsciiFileChoiceButton::updateValue()
{       
    *_fileName = _labelCreated->text();
}

bool CT_AsciiFileChoiceButton::isValueAndWidgetValueDifferent() const
{
    return (*_fileName != _labelCreated->text());
}

QVariant CT_AsciiFileChoiceButton::getValue() const
{
    return QVariant(*_fileName);
}

bool CT_AsciiFileChoiceButton::setWidgetValue(QVariant val)
{
    _labelCreated->setText(val.toString());

    return true;
}

void CT_AsciiFileChoiceButton::chooseFile()
{

    if (!QFile(*_fileName).exists())
    {
        QString s = QFileDialog::getOpenFileName(_widgetCreated, tr("Choisir un fichier"), "", _fileFilter);
        if (QFile(s).exists())
        {
            *_fileName = s;
            _labelCreated->setText(s);
        }   else {
            _labelCreated->setText("");
            *_fileName = "";
        }
    }

    _dialog->setHeader(*_header);
    _dialog->setSeparator(*_separator);
    _dialog->setDecimal(*_decimal);
    _dialog->setNLinesToSkip(*_skip);
    _dialog->setFileNameWithPath(*_fileName);

    if (!_columnsAsString.isEmpty())
    {
        _dialog->setFieldColumnsSelectedFromString(_columnsAsString);
    }

    if (_dialog->exec() == QDialog::Accepted)
    {
        *_fileName = _dialog->getFileNameWithPath();
        *_header = _dialog->hasHeader();
        *_separator = _dialog->getSeparator();
        *_decimal = QLocale(_dialog->getQLocaleName()).decimalPoint();
        *_locale = QLocale(_dialog->getQLocaleName());
        *_skip = _dialog->getNlinesToSkip();
        _columns->clear();
        *_columns = _dialog->getNeededFieldColumns();
        _columnsAsString = _dialog->getFieldColumnsSelectedAsString(*_columns);

        _labelCreated->setText(*_fileName);
    }

    emit fileChanged();
}

void CT_AsciiFileChoiceButton::setFormat(QString format)
{
        _fileFilter = format;
}

