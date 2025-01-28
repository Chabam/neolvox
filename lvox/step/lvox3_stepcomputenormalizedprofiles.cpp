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

#include "lvox3_stepcomputenormalizedprofiles.h"

#include <QFileInfo>

LVOX3_StepComputeNormalizedProfiles::LVOX3_StepComputeNormalizedProfiles() : SuperClass()
{
    _res = 0.5;
    _ignoreLowCells = true;
}

QString LVOX3_StepComputeNormalizedProfiles::description() const
{
    // Gives the descrption to print in the GUI
    return tr("Créer profil vertical normalisé (en hauteur)");
}

QString LVOX3_StepComputeNormalizedProfiles::detailledDescription() const
{
    return tr("Cette étape génère un profil vertical, en hauteur (prise en compte d'un MNT).");
}

QString LVOX3_StepComputeNormalizedProfiles::inputDescription() const
{
    return SuperClass::inputDescription() + tr("<br><br>");
}

QString LVOX3_StepComputeNormalizedProfiles::outputDescription() const
{
    return SuperClass::outputDescription() + tr("<br><br>");
}

QString LVOX3_StepComputeNormalizedProfiles::detailsDescription() const
{
    return tr("");
}

CT_VirtualAbstractStep* LVOX3_StepComputeNormalizedProfiles::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepComputeNormalizedProfiles();
}

void LVOX3_StepComputeNormalizedProfiles::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, tr("Grille voxel"));
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inGrid, tr("Grille voxel"));

    manager.addResult(_inResultDTM, tr("MNT (optionnel)"), "", true);
    manager.setZeroOrMoreRootGroup(_inResultDTM, _inZeroOrMoreRootGroupDTM);
    manager.addGroup(_inZeroOrMoreRootGroupDTM, _inGroupDTM);
    manager.addItem(_inGroupDTM, _inDTM, tr("MNT"));

}

void LVOX3_StepComputeNormalizedProfiles::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _outProfile, tr("ProfilH"));
}

void LVOX3_StepComputeNormalizedProfiles::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog)
{
    postInputConfigDialog->addDouble(tr("Résolution du profil"),tr("mètres"),0.0001,10000,2, _res );
    postInputConfigDialog->addBool(tr("Ignorer les cellules intersectant le sol"), "", "", _ignoreLowCells);

}

void LVOX3_StepComputeNormalizedProfiles::compute()
{

    const CT_Image2D<float>* mnt = nullptr;
    for (const CT_Image2D<float>* imageIn : _inDTM.iterateInputs(_inResultDTM))
    {
            mnt = imageIn;
    }

    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        for (const LVOX3_AbstractGrid3D* inGrid : group->singularItems(_inGrid))
        {
            if (isStopped()) {return;}

            int dim = (inGrid->maxZ() - inGrid->minZ()) / _res + 1;

            // Creating the output profile
            CT_Profile<double>* proH = new CT_Profile<double>(inGrid->minX(), inGrid->minY(), 0, 0, 0, 1, dim, _res, -1, 0);
            CT_Profile<double>* proN = new CT_Profile<double>(inGrid->minX(), inGrid->minY(), 0, 0, 0, 1, dim, _res, -1, 0);
            group->addSingularItem(_outProfile, proH);


            for (size_t c = 0 ; c < inGrid->nCells() ; c++)
            {
                if (!inGrid->isNA(c))
                {
                    Eigen::Vector3d centerCell;
                    inGrid->getCellCenterCoordinates(c, centerCell);
                    double val = inGrid->valueAtIndexAsDouble(c);

                    double zMNT = 0;
                    if (mnt != nullptr)
                    {
                        zMNT = mnt->valueAtCoords(centerCell(0), centerCell(1));
                    }

                    double h = centerCell(2) - zMNT;

                    if (h > 0 || (_ignoreLowCells && h > (_res/2.0)))
                    {
                        proH->addValueForXYZ(centerCell(0), centerCell(1), h, val) ;
                        proN->addValueForXYZ(centerCell(0), centerCell(1), h, 1.0) ;
                    }
                }
            }

            for (size_t i = 0 ; i < proN->getDim() ; i++)
            {
                double n = proN->valueAtIndex(i);
                if (n > 0)
                {
                    proH->setValueAtIndex(i, proH->valueAtIndex(i) / n);
                } else {
                    proH->setValueAtIndex(i, 0);
                }
            }

            proH->computeMinMax();
            delete proN;

        }
    }

    setProgress(99.0f);
}
