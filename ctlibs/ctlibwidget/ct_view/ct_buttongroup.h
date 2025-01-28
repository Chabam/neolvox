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

#ifndef CT_BUTTONGROUP_H
#define CT_BUTTONGROUP_H

#include "ct_radiobutton.h"

#include <QButtonGroup>

class CTLIBWIDGET_EXPORT CT_ButtonGroup : public QObject, public CT_WidgetWithValueReferenceInterface
{
    Q_OBJECT
private:

    struct ButtonGroupData
    {
        QWidget                         *_parent;
        QMap<int, CT_RadioButton*>      _rbList;
        int                             *_value;
    };

public:
    CT_ButtonGroup(QWidget &parent, int &value);
    virtual ~CT_ButtonGroup();

    QString type() const override { return metaObject()->className(); }

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    QWidget *createWidget(QWidget &parent) override;

    void updateValue() override;

    bool isValueAndWidgetValueDifferent() const override;

    QVariant getValue() const override;

    bool setWidgetValue(QVariant val);

    bool addRadioButton(CT_RadioButton *button);
    int& getValueReference() const;

private:

    ButtonGroupData _data;
    QButtonGroup    *_bGCreated;
};

#endif // CT_BUTTONGROUP_H
