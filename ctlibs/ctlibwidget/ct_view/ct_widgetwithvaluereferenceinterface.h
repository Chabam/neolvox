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



#ifndef CT_WIDGETWITHVALUEREFERENCEINTERFACE_H
#define CT_WIDGETWITHVALUEREFERENCEINTERFACE_H

#include "ctlibwidget_global.h"

class SettingsNodeGroup;
class SettingsWriterInterface;
class SettingsReaderInterface;

#include <QWidget>
#include <QVariant>

class CTLIBWIDGET_EXPORT CT_WidgetWithValueReferenceInterface
{
public:    

    virtual ~CT_WidgetWithValueReferenceInterface() {}

    /**
     * @brief Must return the classname
     */
    virtual QString type() const = 0;

    /**
     * @brief Save all settings of this widget that must be
     */
    virtual void saveSettings(SettingsWriterInterface& writer) const = 0;

    /**
     * @brief Restore all settings that must be
     */
    virtual bool restoreSettings(SettingsReaderInterface& reader) = 0;

    /**
     * @brief Must create the widget to add it to the dialog or in another widget
     */
    virtual QWidget* createWidget(QWidget &parent) = 0;

    /**
     * @brief Must use the gui widget to update the value of the internal object
     */
    virtual void updateValue() = 0;

    /**
     * @brief return toUpdateValue
     */
    virtual QVariant getValue() const = 0;

    /**
     * @brief Must return true if the value of the internal object is different than the value of
     *        the gui widget
     */
    virtual bool isValueAndWidgetValueDifferent() const = 0;
};

#endif // CT_WIDGETWITHVALUEREFERENCEINTERFACE_H
