/****************************************************************************

 Copyright (C) 2012-2012 Université de Sherbrooke, Québec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA

 This file is part of Computree version 2.0.

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Computree.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include "lvox_pluginentry.h"
#include "lvox_steppluginmanager.h"

LVOX_PluginEntry::LVOX_PluginEntry()
{
    _stepPluginManager = new LVOX_StepPluginManager();
}

LVOX_PluginEntry::~LVOX_PluginEntry()
{
    delete _stepPluginManager;
}

QString LVOX_PluginEntry::getVersion() const
{
    return "3.0";
}

CT_AbstractStepPlugin* LVOX_PluginEntry::getPlugin() const
{
    return _stepPluginManager;
}


#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(plug_lvoxv3, LVOX_PluginEntry)
#endif

