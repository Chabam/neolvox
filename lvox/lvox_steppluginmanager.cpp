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

#include "lvox_steppluginmanager.h"

#include "tools/ct_readerstools.h"

#include "ct_exporter/ct_standardexporterseparator.h"
#include "ct_reader/ct_standardreaderseparator.h"

#include "step/lvox3_stepcomputelvoxgrids.h"
#include "step/lvox3_steploadfiles.h"
#include "step/lvox3_stepcomputeprofiles.h"
#include "step/lvox3_stepinterpolatedistance.h"
#include "step/lvox3_stepmergegrids.h"
#include "step/lvox3_stepextractcirculargrid.h"
#include "step/lvox3_stepcomputepad.h"
#include "step/lvox3_stepgridnormalisation.h"
#include "step/lvox3_stepgridnormalisationraster.h"
#include "step/lvox3_stepcomputenormalizedprofiles.h"
#include "tools/exporter/lvox3_grid3dexporterdefs.h"
#include "tools/reader/lvox3_readerdefs.h"


#include "step/lvox3_stepmakeshootingpatternformls.h"

//#include "step/lvox3_stepexportcrownstats.h"
//#include "step/lvox3_stepflatgrid.h"
//#include "step/lvox3_stepfiltervoxelatdtm.h"
//#include "step/lvox3_stepunifyscene.h"
//#include "step/lvox3_stepaddgridtonext.h"
//#include "step/lvox3_stepcomputesky.h"
//#include "step/lvox3_stepgenericcomputegrids.h"

//#ifdef USE_PCL
//#include "step/lvox3_stepcreatemesh.h"
//#endif

#include "ct_global/ct_context.h"
#include "ct_categories/ct_stdcategory.h"
#include "tools/lvox3_utils.h"

#include <QMutex>

LVOX_StepPluginManager::LVOX_StepPluginManager() : CT_AbstractStepPlugin()
{
    PS_CATEGORY_MANAGER->registerCategory(new CT_StdCategory(DEF_LVOX_GRD_NI, QList<QString>() << CT_AbstractCategory::DATA_VALUE, "Ni"));
    PS_CATEGORY_MANAGER->registerCategory(new CT_StdCategory(DEF_LVOX_GRD_NB, QList<QString>() << CT_AbstractCategory::DATA_VALUE, "Nb"));
    PS_CATEGORY_MANAGER->registerCategory(new CT_StdCategory(DEF_LVOX_GRD_NT, QList<QString>() << CT_AbstractCategory::DATA_VALUE, "Nt"));
    PS_CATEGORY_MANAGER->registerCategory(new CT_StdCategory(DEF_LVOX_GRD_NTA, QList<QString>() << CT_AbstractCategory::DATA_VALUE, "Nta"));
    PS_CATEGORY_MANAGER->registerCategory(new CT_StdCategory(DEF_LVOX_GRD_DENSITY, QList<QString>() << CT_AbstractCategory::DATA_VALUE, "Density"));
}

LVOX_StepPluginManager::~LVOX_StepPluginManager()
{
}

QString LVOX_StepPluginManager::getPluginRISCitation() const
{
    return "TY  - COMP\n"
           "TI  - Plugin LVOX for Computree\n"
           "AU  - Fournier, Richard\n"
           "AU  - Nguyen, Van Tho\n"
           "AU  - Krebs, Michael\n"
           "AU  - Ravaglia, Joris\n"
           "AU  - Piboule, Alexandre\n"
           "PB  - University of Sherbrooke\n"
           "PY  - 2022\n"
           "UR  - https://computree.onf.fr/?page_id=1348\n"
           "ER  - \n";
}



bool LVOX_StepPluginManager::loadGenericsStep()
{
    addNewVoxelsStep<LVOX3_StepLoadFiles>("LVOX3");
    addNewVoxelsStep<LVOX3_StepMakeShootingPatternForMLS>("LVOX3");
    addNewVoxelsStep<LVOX3_StepComputeLvoxGrids>("LVOX3");
    addNewVoxelsStep<LVOX3_StepComputePAD>("LVOX3");
    addNewVoxelsStep<LVOX3_StepMergeGrids>("LVOX3");
    addNewVoxelsStep<LVOX3_StepInterpolateDistance>("LVOX3");
    addNewVoxelsStep<LVOX3_StepComputeProfiles>("LVOX3");
    addNewVoxelsStep<LVOX3_StepExtractCircularGrid>("LVOX3");
    //addNewVoxelsStep<LVOX3_StepGridNormalisation>("LVOX3");
    addNewVoxelsStep<LVOX3_StepGridNormalisationRaster>("LVOX3");
    addNewVoxelsStep<LVOX3_StepComputeNormalizedProfiles>("LVOX3");


//    addNewVoxelsStep<LVOX3_StepGenericComputeGrids>(QObject::tr("LVOX3"));
//    addNewVoxelsStep<LVOX3_StepAddGridToNext>(QObject::tr("LVOX3"));

    //Unify step is a legacy method to merge point clouds that is more performant in the toolkit plugin
    //addNewVoxelsStep<LVOX3_StepUnifyScene>(QObject::tr("LVOX3"));
    //addNewVoxelsStep<LVOX3_StepFilterVoxelAtDTM>(CT_StepsMenu::LP_Filter);
//    #ifdef USE_PCL
//      addNewVoxelsStep<LVOX3_StepCreateMesh>(QObject::tr("Statistiques"));
//    #endif
//    addNewVoxelsStep<LVOX3_StepExportCrownStats>(QObject::tr("Statistiques"));
    //addNewVoxelsStep<LVOX3_StepFlatGrid>(QObject::tr("Statistiques"));

    return true;
}

bool LVOX_StepPluginManager::loadOpenFileStep()
{

    return true;
}

bool LVOX_StepPluginManager::loadCanBeAddedFirstStep()
{
    return true;
}

bool LVOX_StepPluginManager::loadActions()
{
    return true;
}

bool LVOX_StepPluginManager::loadExporters()
{
    //Re-implementation of GRID3D export into GRID3DLVOX for grids with three resolutions
    CT_StandardExporterSeparator *sep = addNewSeparator(new CT_StandardExporterSeparator("Exporters LVOX"));

    sep->addExporter(new LVOX3_Grid3DExporter(CT_StepsMenu::LP_Voxels));
    return true;
}

bool LVOX_StepPluginManager::loadReaders()
{
    //Re-implementation of GRID3D loader into GRID3DLVOX for grids with three resolutions
    CT_StandardReaderSeparator *sep = addNewSeparator(new CT_StandardReaderSeparator("Readers LVOX"));
    sep->addReader(new LVOX3_Reader_AsciiGrid3D(CT_StepsMenu::LP_Voxels));
    return true;
}

bool LVOX_StepPluginManager::loadAfterAllPluginsLoaded()
{
    initReadersCollection();
    return true;
}

void LVOX_StepPluginManager::initReadersCollection()
{
    CT_ReadersTools::initAvailableReaders(m_readersOfAllPlugins, nullptr);
}

CT_AbstractReader* LVOX_StepPluginManager::readerAvailableByUniqueName(const QString &readerClassName) const
{
    for(CT_AbstractReader* reader : m_readersOfAllPlugins) {
        if(reader->uniqueName() == readerClassName)
            return reader;
    }

    return nullptr;
}

const QList<CT_AbstractReader *> &LVOX_StepPluginManager::readersAvailable() const
{
    return m_readersOfAllPlugins;
}

