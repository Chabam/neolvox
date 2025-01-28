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



#ifndef CT_DOUBLESPINBOX_H
#define CT_DOUBLESPINBOX_H

#include "ct_widgetwithvaluereferenceinterface.h"

#include <QDoubleSpinBox>

class CTLIBWIDGET_EXPORT CT_DoubleSpinBox : public QObject, public CT_WidgetWithValueReferenceInterface
{
    Q_OBJECT

private:

    struct DoubleSpinBoxData
    {
        double  _min;
        double  _max;
        int     _nDecimals;
        double  *_value;
        double  _multValue;
    };

public:
    CT_DoubleSpinBox(double min,
                     double max,
                     int nDecimals,
                     double &value,
                     double multValue = 1, QString description = "");

    QString type() const override { return metaObject()->className(); }

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    QWidget* createWidget(QWidget &parent) override;

    void updateValue() override;

    bool isValueAndWidgetValueDifferent() const override;

    QVariant getValue() const override;

    double getMultValue() const {return _data._multValue;}
    int getnDecimals() const {return _data._nDecimals;}

    bool setWidgetValue(QVariant val);

protected:

    DoubleSpinBoxData   _data;
    QDoubleSpinBox      *_doubleSpinBoxCreated;
    QString         _description;
};

#endif // CT_DOUBLESPINBOX_H
