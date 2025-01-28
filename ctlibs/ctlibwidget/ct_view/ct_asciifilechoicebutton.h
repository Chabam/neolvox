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

#ifndef CT_ASCIIFILECHOICEBUTTON_H
#define CT_ASCIIFILECHOICEBUTTON_H



#include "ct_widgetwithvaluereferenceinterface.h"
#include "ct_view/tools/ct_textfileconfigurationdialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>
#include <QScrollArea>

class CTLIBWIDGET_EXPORT CT_AsciiFileChoiceButton : public QObject, public CT_WidgetWithValueReferenceInterface
{
    Q_OBJECT

public:


    CT_AsciiFileChoiceButton(QString btlab, QString fileFilter, bool autoDetect, const QStringList &neededFields, QString &fileName, bool &header, QString &separator, QString &decimal, QLocale &locale, int &skip, QMap<QString, int> &columns, QString description = "");
    CT_AsciiFileChoiceButton(QString btlab, QString fileFilter, bool autoDetect, QList<CT_TextFileConfigurationFields> &neededFields, QString &fileName, bool &header, QString &separator, QString &decimal, QLocale &locale, int &skip, QMap<QString, int> &columns, QString description = "");

    ~CT_AsciiFileChoiceButton();

    QString type() const override { return metaObject()->className(); }

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    QWidget* createWidget(QWidget &parent) override;

    void updateValue() override;

    bool isValueAndWidgetValueDifferent() const override;

    QVariant getValue() const override;

    bool setWidgetValue(QVariant val);


protected:

    QString     _buttonLabel;
    QString     _fileFilter;
    bool        _autoDetect;

    QString             *_fileName;
    bool                *_header;
    QString             *_separator;
    QString             *_decimal;
    QLocale             *_locale;
    int                 *_skip;
    QMap<QString, int>  *_columns;

    QString             _columnsAsString;

    CT_TextFileConfigurationDialog  *_dialog;
    QWidget                         *_widgetCreated;
    QScrollArea                     *_scrollArea;
    QVBoxLayout                     *_verticalLayout;
    QPushButton                     *_pushButtonCreated;
    QLabel                          *_labelCreated;
    QString                         _description;

public slots:
    void setFormat(QString format);

private slots:
    void chooseFile();

signals:
    void fileChanged();

};

#endif // CT_ASCIIFILECHOICEBUTTON_H
