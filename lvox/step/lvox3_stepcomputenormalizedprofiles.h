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

#ifndef LVOX3_STEPCOMPUTENORMALIZEDPROFILES_H
#define LVOX3_STEPCOMPUTENORMALIZEDPROFILES_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "tools/3dgrid/lvox_grid3d.h"
#include "ct_itemdrawable/ct_profile.h"
#include "ct_itemdrawable/ct_image2d.h"

class LVOX3_StepComputeNormalizedProfiles : public CT_AbstractStep
{
    Q_OBJECT
    using SuperClass = CT_AbstractStep;

public:

    LVOX3_StepComputeNormalizedProfiles();

    QString description() const override;
    QString detailledDescription() const override;
    QString inputDescription() const override;
    QString outputDescription() const override;
    QString detailsDescription() const override;

    virtual CT_VirtualAbstractStep* createNewInstance() const final;

protected:

    virtual void declareInputModels(CT_StepInModelStructureManager& manager) final;

    virtual void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    virtual void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog) final;

    virtual void compute() final;


private:



    double          _res;
    bool            _ignoreLowCells;

    CT_HandleInResultGroupCopy<>                                                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                                                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                                                               _inGroup;
    CT_HandleInSingularItem<CT_AbstractGrid3D>                                       _inGrid;

    CT_HandleInResultGroup<0,1>                                                         _inResultDTM;
    CT_HandleInStdZeroOrMoreGroup                                                       _inZeroOrMoreRootGroupDTM;
    CT_HandleInStdGroup<>                                                               _inGroupDTM;
    CT_HandleInSingularItem<CT_Image2D<float> >                                         _inDTM;

    CT_HandleOutSingularItem<CT_Profile<double>>                                        _outProfile;


};

#endif // LVOX3_STEPCOMPUTENORMALIZEDPROFILES_H

