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

#ifndef CT_ABSTRACTSTEP_H
#define CT_ABSTRACTSTEP_H

#include "ct_step/abstract/ct_virtualabstractstep.h"

#include "ct_handle/ct_handleinresultgroup.h"
#include "ct_handle/ct_handleinresultgroupcopy.h"
#include "ct_handle/ct_handleinstdzeroormoregroup.h"
#include "ct_handle/ct_handleinstdgroup.h"
#include "ct_handle/ct_handleinsingularitem.h"
#include "ct_handle/ct_handleinstditemattribute.h"
#include "ct_handle/ct_handleinstditemattributet.h"

#include "ct_handle/ct_handleoutresultgroup.h"
#include "ct_handle/ct_handleoutstdgroup.h"
#include "ct_handle/ct_handleoutsingularitem.h"
#include "ct_handle/ct_handleoutstditemattribute.h"
#include "ct_handle/ct_handleoutstditemattributewrapper.h"

/**
 * @brief Represent a step that do a process
 *
 * You must redefine in your step at least methods :
 * -------------------------------------------------
 *
 * CT_VirtualAbstractStep* createNewInstance()                                  // create a empty copy of your object
 * void declareInputModels(CT_StepInModelStructureManager& manager)             // declare INPUTS
 * void declareOutputModels(CT_StepOutModelStructureManager& manager)           // declare OUTPUTS
 * void compute()                                                               // your process !
 *
 ************* REDEFINE METHODS BELOW IF : ************
 *
 * You want to inform user what do your step :
 * -----------------------------------------------------------------------------
 *
 * QString description() const                                                  // a brief description of your step
 * QString detailledDescription() const                                         // a detailled description of your step
 *
 * You want to add standards configuration dialog :
 * ------------------------------------------------
 *
 * void fillPreInputConfigurationDialog(...)                                    // fill the standard pre input configuration dialog
 * void fillPostInputConfigurationDialog(...)                                   // fill the standard post input configuration dialog
 *
 * You want to use your own configuration dialog :
 * ------------------------------------------------
 *
 * bool preInputConfigure()                                                     // show your pre input configuration dialog and return true if user don't cancel it
 * void savePreSettings(SettingsWriterInterface& writer) const                  // save your pre input settings. Don't forget to call the method of the SuperClass if you redefine it !
 * bool restorePreSettings(SettingsReaderInterface& reader)                     // restore your pre input settings. Don't forget to call the method of the SuperClass if you redefine it !
 *
 * bool postInputConfigure()                                                    // show your post input configuration dialog and return true if user don't cancel it
 * void savePostSettings(SettingsWriterInterface& writer) const                 // save your post input settings. Don't forget to call the method of the SuperClass if you redefine it !
 * bool restorePostSettings(SettingsReaderInterface& reader)                    // restore your post input settings. Don't forget to call the method of the SuperClass if you redefine it !
 *
 * You want to use manual mode :
 * -----------------------------
 *
 * Don't forget to call "setManual(true)" in your constructor
 * Call "requestManualMode()" in your compute to request tha manual mode
 *
 * void initManualMode()                                                        // called when manual mode is activate for the first time
 * void useManualMode(bool quit = false)                                        // called every time the manual mode is activated or exited
 *
 * You want to use debug mode :
 * ----------------------------
 *
 * Don't forget to call "setDebuggable(true)" in your constructor
 * Call "waitForAckIfInDebugMode()" in your compute to set a break point
 *
 * void preWaitForAckIfInDebugMode()                                            // Called from gui before step go in debug mode
 * void postWaitForAckIfInDebugMode()                                           // Called from gui after user click next button in debug mode
 */
using CT_AbstractStep = CT_VirtualAbstractStep;

#endif // CT_ABSTRACTSTEP_H
