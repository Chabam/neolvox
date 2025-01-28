/****************************************************************************
 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                         All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre PIBOULE (ONF)

 This file is part of PluginONF library.

 PluginONF is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginONF is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginONF.  If not, see <http://www.gnu.org/licenses/lgpl.html>.
*****************************************************************************/


#ifndef LVOX2_STEPFILTERGRIDBYRADIUS_H
#define LVOX2_STEPFILTERGRIDBYRADIUS_H

#include "ct_step/abstract/ct_abstractstep.h"                    // The step inherits from ct_abstractstep

// Inclusion of auto-indexation system
#include "ct_tools/model/ct_autorenamemodels.h"
#include "ct_tools/ct_monitoredqthread.h"

#include <QMutex>


class LVOX2_StepFilterGridByRadius : public CT_AbstractStep
{
    /** \def    Q_OBJECT :
        \brief  Macro from the Qt environment. Useful to get the name of the step (among others)*/
    Q_OBJECT

public:
    /*!
    *  \brief Constructor
    *
    *  Constructor of the class
    *  \param dataInit : informations about the step : parent step, plugin manager managing this step and some settings from Qt (for the Q_OBJECT?).
    */
    LVOX2_StepFilterGridByRadius(CT_StepInitializeData &dataInit);

    /*!
    *  \brief Gives a description of the step
    *  This method will be used in the GUI management : this description will be shown to the user when the mouse is over the step in the step menu.
    *
    *  \warning Inherited from CT_AbstractStep, this method is not pure virtual (could be not defined here).
    */
    virtual QString getStepDescription() const;

    /**
     * @brief Inherit this method if you want to return your own detailled description.
     *
     *        By default return a empty string
     */
    virtual QString getStepDetailledDescription() const;

    /*!
    *  \brief Creates a new instance of this step
    *
    *  \param dataInit : informations about the step : parent step, plugin manager managing this step and some settings from Qt (for the Q_OBJECT?).
    *
    *  \warning Inherited from CT_AbstractStep, this method is pure virtual, each step MUST define its own method.
    */
    virtual CT_VirtualAbstractStep* createNewInstance(CT_StepInitializeData &dataInit);

protected:
    /*!
    *  \brief Creates the input of this step
    *
    *  This method defines what kind of input the step can accept and get them
    *
    *  \warning Inherited from CT_AbstractStep, this method is pure virtual, each step MUST define its own method.
    */
    virtual void createInResultModelListProtected();

    /*!
    *  \brief Creates the output of this step
    *
    *  This method defines what kind of results the step produces
    *
    *  \warning Inherited from CT_AbstractStep, this method is pure virtual, each step MUST define its own method.
    */
    virtual void createOutResultModelListProtected();

    /*!
    *  \brief Create a specific configuration dialog window for the step
    *
    *  This method creates a window for the user to set the different parameters of the step.
    *  The different parameters that can be set by the users and their properties (min, max, level of detail) are defined here :
    *       To add a parameter to this dialog window, just do a "_configDialog->addTYPEOFVALUE(...)"
    *
    *  \warning Inherited from CT_AbstractStep, this method is not pure virtual (could be not defined here).
    */
    virtual void createPostConfigurationDialog();

    /*!
    *  \brief Runs the step process
    *
    *  This method contains the effective calculus of the step (running the step is equivalent to launch this method).
    *  You might consider it as a "main" function for a step.
    *
    *  \warning Inherited from CT_AbstractStep, this method is pure virtual, each step MUST define its own method.
    */
    virtual void compute();

private:

    double  _centerX;
    double  _centerY;
    double  _radius;

    // Declaration of autoRenames Variables (groups or items addes to In models copies)
    CT_AutoRenameModels _outGrid_ModelName;

};

#endif // LVOX2_STEPFILTERGRIDBYRADIUS_H



