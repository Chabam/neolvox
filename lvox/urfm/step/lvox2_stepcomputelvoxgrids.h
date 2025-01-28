/****************************************************************************

 Copyright (C) 2012-2012 Universite de Sherbrooke, Quebec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA
 Adapted by Alexandre Piboule for Computree 2.0

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


#ifndef LVOX2_STEPCOMPUTELVOXGRIDS_H
#define LVOX2_STEPCOMPUTELVOXGRIDS_H

/** \file LVOX2_StepComputeLvoxGrids.h
    \author Ravaglia J. - mailto : joris.ravaglia@gmail.com
    \version 0.1
*/

#include "ct_step/abstract/ct_abstractstep.h"                    // The step inherits from ct_abstractstep

// Inclusion of auto-indexation system
#include "ct_tools/model/ct_autorenamemodels.h"
#include "ct_tools/ct_monitoredqthread.h"


/** \class  LVOX2_StepComputeLvoxGrids
    \brief
*/
class LVOX2_StepComputeLvoxGrids : public CT_AbstractStep
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
    LVOX2_StepComputeLvoxGrids(CT_StepInitializeData &dataInit);

    /*!
    *  \brief Gives a description of the step
    *  This method will be used in the GUI management : this description will be shown to the user when the mouse is over the step in the step menu.
    *
    *  \warning Inherited from CT_AbstractStep, this method is not pure virtual (could be not defined here).
    */
    virtual QString getStepDescription() const;

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

public slots:
    void updateProgress();

private:

    // Declaration of autoRenames Variables (groups or items addes to In models copies)
    CT_AutoRenameModels _deltaout_ModelName;
    CT_AutoRenameModels _deltain_ModelName;
    CT_AutoRenameModels _deltabef_ModelName;
    CT_AutoRenameModels _deltatheo_ModelName;
    CT_AutoRenameModels _deltaactu_ModelName;

    CT_AutoRenameModels _bef_ModelName;
    CT_AutoRenameModels _theo_ModelName;
    CT_AutoRenameModels _actu_ModelName;
    CT_AutoRenameModels _hits_ModelName;
    CT_AutoRenameModels _density_ModelName;

    CT_AutoRenameModels _NiFlag_ModelName;
    CT_AutoRenameModels _NbFlag_ModelName;
    CT_AutoRenameModels _NtFlag_ModelName;
    CT_AutoRenameModels _NtaFlag_ModelName;
    CT_AutoRenameModels _DensityFlag_ModelName;


    QList<CT_MonitoredQThread*>     _threadList;

//********************************************//
//              Attributes of LVox            //
//********************************************//

    double          _res;                   /*!< size of a voxel*/
    int             _effectiveRayThresh;    /*!< minimum number of effective ray (Nt-Nb) in the voxel to calculate a density*/
    bool            _computeDistances;
    int             _ntMode; // mode of computation of total number of beam (0 theoretical, 1 actual, or 2 both)
    int             _gridMode;
    double          _xBase;
    double          _yBase;
    double          _zBase;

    int          _xDim;
    int          _yDim;
    int          _zDim;
    bool        _cylindricFilter;// Ni are counted only inside a cylinder (IGN, RH)

};

#endif // LVOX2_STEPCOMPUTELVOXGRIDS_H
