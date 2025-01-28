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

#ifndef CT_FILECHOICEBUTTON_H
#define CT_FILECHOICEBUTTON_H



#include "ct_widgetwithvaluereferenceinterface.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QObject>
#include <QScrollArea>

class CTLIBWIDGET_EXPORT CT_FileChoiceButton : public QObject, public CT_WidgetWithValueReferenceInterface
{
    Q_OBJECT

public:
    enum NeededFileType {
        OneExistingFile,
        OneOrMoreExistingFiles,
        OneNewFile,
        OneExistingFolder
    };

private:


    struct FileChoiceButtonData
    {
        QString  _buttonLabel;
        QString  _fileFilter;
        CT_FileChoiceButton::NeededFileType _fileType;
        QStringList  *_value;
    };

public:


    CT_FileChoiceButton(QString btlab, CT_FileChoiceButton::NeededFileType filetype, QString fileFilter, QStringList &value, QString description = "");

    QString type() const override { return metaObject()->className(); }

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    QWidget* createWidget(QWidget &parent) override;

    void updateValue() override;

    bool isValueAndWidgetValueDifferent() const override;

    QVariant getValue() const override;

    bool setWidgetValue(QVariant val);

    QVariant getValueAsString() const;
    bool setWidgetValueAsString(QVariant val);

protected:

    FileChoiceButtonData   _data;
    QWidget                 *_widgetCreated;
    QScrollArea             *_scrollArea;
    QVBoxLayout             *_verticalLayout;
    QPushButton             *_pushButtonCreated;
    QLabel                  *_labelCreated;
    QString                 _description;

public slots:
    void setFormat(QString format);

private slots:
    void chooseFile();

signals:
    void valueModified(QString text);

};

#endif // CT_FILECHOICEBUTTON_H
