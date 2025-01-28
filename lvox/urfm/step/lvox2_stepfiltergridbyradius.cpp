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


#include "lvox2_stepfiltergridbyradius.h"

#include "ct_result/model/inModel/ct_inresultmodelgroup.h"
#include "ct_result/model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_result/model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"

// Inclusion of actions methods
#include "ct_tools/model/ct_outmodelcopyactionaddmodelitemingroup.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_iterator/ct_resultgroupiterator.h"
#include "ct_iterator/ct_resultitemiterator.h"

// Inclusion of standard result class
#include "ct_result/ct_resultgroup.h"

// Inclusion of used ItemDrawable classes
#include "ct_itemdrawable/ct_grid3d.h"

#include "ct_view/ct_stepconfigurabledialog.h"


#include <math.h>

#define DEF_SearchInResult "r"
#define DEF_SearchInGroup   "gr"
#define DEF_SearchInGrid   "grid"

LVOX2_StepFilterGridByRadius::LVOX2_StepFilterGridByRadius(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
    _centerX = 0;
    _centerY = 0;
    _radius = 6.0;
}

QString LVOX2_StepFilterGridByRadius::getStepDescription() const
{
    // Gives the descrption to print in the GUI
    return tr("Filter une grille 3D par un rayon");
}

// Step description (tooltip of contextual menu)
QString LVOX2_StepFilterGridByRadius::getStepDetailledDescription() const
{
    return tr("");
}

CT_VirtualAbstractStep* LVOX2_StepFilterGridByRadius::createNewInstance(CT_StepInitializeData &dataInit)
{
    // Creates an instance of this step
    return new LVOX2_StepFilterGridByRadius(dataInit);
}

void LVOX2_StepFilterGridByRadius::createInResultModelListProtected()
{
    CT_InResultModelGroupToCopy *resultModel = createNewInResultModelForCopy(DEF_SearchInResult, tr("Grille"), "", true);

    resultModel->setZeroOrMoreRootGroup();
    resultModel->addGroupModel("", DEF_SearchInGroup);
    resultModel->addItemModel(DEF_SearchInGroup, DEF_SearchInGrid, CT_AbstractGrid3D::staticGetType(), tr("Grille"), "", CT_InAbstractModel::C_ChooseMultipleIfMultiple);
}

void LVOX2_StepFilterGridByRadius::createOutResultModelListProtected()
{
    CT_OutResultModelGroupToCopyPossibilities *res = createNewOutResultModelToCopy(DEF_SearchInResult);

    if(res != NULL)
    {
        res->addItemModel(DEF_SearchInGroup, _outGrid_ModelName, new CT_Grid3D<double>(), tr("Grille filtrée"));
    }
}

void LVOX2_StepFilterGridByRadius::createPostConfigurationDialog()
{
    CT_StepConfigurableDialog *configDialog = newStandardPostConfigurationDialog();

    configDialog->addDouble(tr("X centre"),"", -1e+10, 1e+10, 4, _centerX);
    configDialog->addDouble(tr("Y centre"),"", -1e+10, 1e+10, 4, _centerY);
    configDialog->addDouble(tr("Rayon"),"", -1e+10, 1e+10, 4, _radius);
}

void LVOX2_StepFilterGridByRadius::compute()
{
    // Gets the out result
    CT_ResultGroup* outResult = getOutResultList().first();

    CT_ResultGroupIterator itOut(outResult, this, DEF_SearchInGroup);
    // iterate over all groups
    while(itOut.hasNext())
    {
        CT_StandardItemGroup *group = (CT_StandardItemGroup*)itOut.next();
        const CT_AbstractGrid3D* inGrid = (CT_AbstractGrid3D*)group->firstItemByINModelName(this, DEF_SearchInGrid);
         if (inGrid!=NULL)
        {
            bool ok;
            double NAval = inGrid->NAAsString().toDouble(&ok);
            if (!ok) {NAval = -std::numeric_limits<double>::max();}

            // Declaring the output grids
            //CT_Grid3D<double>* outGrid = new CT_Grid3D<double>(_outGrid_ModelName.completeName(), outResult, inGrid->minX(), inGrid->minY(), inGrid->minZ(), inGrid->xdim(), inGrid->ydim(), inGrid->zdim(), inGrid->resolution(), NAval, 0);
            CT_Grid3D<double>* outGrid = new CT_Grid3D<double>(_outGrid_ModelName.completeName(), outResult, inGrid->minX(), inGrid->minY(), inGrid->minZ(), inGrid->xdim(), inGrid->ydim(), inGrid->zdim(), inGrid->resolution(), -1, 0);

            //CT_Grid3D<float>*   density = new CT_Grid3D<float>(_density_ModelName.completeName(), outResult, hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(), hitGrid->ydim(), hitGrid->zdim(), _res, -1, 0);
            //density->addItemAttribute(new CT_StdItemAttributeT<bool>(_DensityFlag_ModelName.completeName(), "LVOX_GRD_DENSITY", outResult, true));

            outGrid->computeMinMax();
           // qDebug() << "Filtering grid by radius:" << outGrid->dataMin() << outGrid->dataMax();

            // loop on z levels
            for (size_t zz = 0 ;  zz < inGrid->zdim() ; zz++)
            {
                for (size_t yy = 0 ;  yy < inGrid->ydim() ; yy++)
                {
                    for (size_t xx = 0 ;  xx < inGrid->xdim() ; xx++)
                    {
                        size_t index;
                        inGrid->index(xx, yy, zz, index);
                        double value = inGrid->valueAtIndexAsDouble(index);
                          //qDebug() << "Filtering grid by radius=" << value;
                          double dist2 = pow(inGrid->getCellCenterX(xx) - _centerX, 2.0) + pow(inGrid->getCellCenterY(yy) - _centerY,2.0);
                        if (dist2 < pow(_radius, 2.0))
                        {
                            outGrid->setValueAtIndex(index, value);
                        } else {
                            outGrid->setValueAtIndex(index, 0.f);
                        }
                    }
                }
            }

            outGrid->computeMinMax();
            qDebug() << "Filtering grid by radius:" << outGrid->dataMin() << outGrid->dataMax();
            group->addItemDrawable(outGrid);
        }
    }

    setProgress(99);
}


