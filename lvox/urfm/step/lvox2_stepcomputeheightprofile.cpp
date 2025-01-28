#include "lvox2_stepcomputeheightprofile.h"

#ifdef USE_OPENCV

#include "ct_view/ct_stepconfigurabledialog.h"
#include "ct_result/model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_result/model/outModel/ct_outresultmodelgroupcopy.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_result/model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"
#include "ct_itemdrawable/ct_image2d.h"

// Inclusion of standard result class
#include "ct_result/ct_resultgroup.h"

// Inclusion of used ItemDrawable classes
#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/ct_profile.h"

// Alias for indexing in models
#define DEF_resultIn_grids "rgrids"
#define DEF_groupIn_grids "grids"
#define DEF_itemIn_grid "density"

#define DEF_SearchInMNTResult        "rmnt"
#define DEF_SearchInMNTGroup         "gmnt"
#define DEF_SearchInMNT              "mnt"

#include <math.h>

// Constructor : initialization of parameters
LVOX2_StepComputeHeightProfile::LVOX2_StepComputeHeightProfile(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
    _min = 0;
    _max = 10;
    _step = 10;
    _limStata1 = 1.0;
}

// Step description (tooltip of contextual menu)
QString LVOX2_StepComputeHeightProfile::getStepDescription() const
{
    return tr("Créer un Profil Vertical de phytovolume");
}

// Step copy method
CT_VirtualAbstractStep* LVOX2_StepComputeHeightProfile::createNewInstance(CT_StepInitializeData &dataInit)
{
    return new LVOX2_StepComputeHeightProfile(dataInit);
}

//////////////////// PROTECTED METHODS //////////////////

// Creation and affiliation of IN models
void LVOX2_StepComputeHeightProfile::createInResultModelListProtected()
{
    CT_InResultModelGroupToCopy *resultModel = createNewInResultModelForCopy(DEF_resultIn_grids, tr("Grilles"));
    resultModel->setZeroOrMoreRootGroup();
    resultModel->addGroupModel("", DEF_groupIn_grids);
    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_grid, CT_AbstractGrid3D::staticGetType(), tr("Grille 3D"));

    // MNT
    CT_InResultModelGroup* resultModelMNT = createNewInResultModel(DEF_SearchInMNTResult, tr("MNT (Raster)"), "", true);
    resultModelMNT->setZeroOrMoreRootGroup();
    resultModelMNT->addGroupModel("", DEF_SearchInMNTGroup);
    resultModelMNT->addItemModel(DEF_SearchInMNTGroup, DEF_SearchInMNT, CT_Image2D<float>::staticGetType(), tr("Modèle Numérique de Terrain"));
    resultModelMNT->setMinimumNumberOfPossibilityThatMustBeSelectedForOneTurn(0);
}

// Creation and affiliation of OUT models
void LVOX2_StepComputeHeightProfile::createOutResultModelListProtected()
{
    CT_OutResultModelGroupToCopyPossibilities *resultModel = createNewOutResultModelToCopy(DEF_resultIn_grids);

    if (resultModel != NULL)
    {
        resultModel->addGroupModel(DEF_groupIn_grids, _outProfileGrp_ModelName);
        resultModel->addItemModel(_outProfileGrp_ModelName, _outProfile_ModelName, new CT_Profile<double>(), tr("Profil"));
        resultModel->addItemAttributeModel(_outProfile_ModelName, _outProfileThreshold_ModelName,
                                           new CT_StdItemAttributeT<double>(CT_AbstractCategory::DATA_NUMBER),
                                           tr("Threshold"));
        resultModel->addItemAttributeModel(_outProfile_ModelName, _outProfileStrata1_ModelName,
                                           new CT_StdItemAttributeT<double>(CT_AbstractCategory::DATA_NUMBER),
                                           tr("Strata 1 Vol"));
    }
}

// Semi-automatic creation of step parameters DialogBox
void LVOX2_StepComputeHeightProfile::createPostConfigurationDialog()
{
    CT_StepConfigurableDialog *configDialog = newStandardPostConfigurationDialog();

    configDialog->addDouble(tr("Seuil Minimum"), "", -99999, 99999, 2, _min);
    configDialog->addDouble(tr("Seuil Maximum"), "", -99999, 99999, 2, _max);
    configDialog->addDouble(tr("Pas du seuil"), "", -99999, 99999, 2,  _step);
    configDialog->addDouble(tr("Limite haute de la strate 1"), "", 0, 99999, 2, _limStata1);
}

void LVOX2_StepComputeHeightProfile::compute()
{

    // Modéles pour récupérer les MNT
    CT_Image2D<float>* dtm = NULL;
    if (getInputResults().size() > 1)
    {
        CT_ResultGroup* inMNTResult = getOutResultList().at(1);

        CT_ResultItemIterator itMNT(inMNTResult, this, DEF_SearchInMNT);
        // on demande si on peut commencer à parcourir les groupes
        if(itMNT.hasNext())
        {
            dtm = (CT_Image2D<float>*)itMNT.next();
        }
    }

    CT_ResultGroup *outResult = getOutResultList().first();

    CT_ResultGroupIterator itGrp(outResult, this, DEF_groupIn_grids);
    while (itGrp.hasNext() && !isStopped())
    {
        CT_StandardItemGroup *groupIn_grids = (CT_StandardItemGroup*) itGrp.next();

        const CT_AbstractGrid3D* inGrid = (const CT_AbstractGrid3D*) groupIn_grids->firstItemByINModelName(this, DEF_itemIn_grid);

        if (inGrid!=NULL)
        {
            bool ok;
            double NAval = inGrid->NAAsString().toDouble(&ok);
            if (!ok) {NAval = -std::numeric_limits<double>::max();}

            double zMinProfile = 0;
            if (dtm == NULL) {zMinProfile = inGrid->minZ();}

            for (double threshold = _min ; threshold <= _max ; threshold += _step)
            {
                CT_Profile<double>* outProfile = new CT_Profile<double>(_outProfile_ModelName.completeName(),
                                                                        outResult,
                                                                        (inGrid->maxX() + inGrid->minX()) / 2.0,
                                                                        (inGrid->maxY() + inGrid->minY()) / 2.0,
                                                                        zMinProfile,
                                                                        0,
                                                                        0,
                                                                        1,
                                                                        inGrid->zdim(),
                                                                        inGrid->resolution(),
                                                                        NAval,
                                                                        0.0);

                double voxelVol = pow(inGrid->resolution(), 3);

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

                            double zDTM = inGrid->minZ();

                            if (dtm != NULL)
                            {
                                zDTM = dtm->valueAtCoords(inGrid->getCellCenterX(xx), inGrid->getCellCenterY(yy));
                            }

                            if (value > threshold && zDTM != NAval)
                            {
                                outProfile->addValueAtIndex(inGrid->getCellCenterZ(zz) - zDTM, voxelVol);
                            }
                        }
                    }
                }

                outProfile->computeMinMax();
                CT_StandardItemGroup* grpProfile = new CT_StandardItemGroup(_outProfileGrp_ModelName.completeName(), outResult);
                groupIn_grids->addGroup(grpProfile);
                grpProfile->addItemDrawable(outProfile);

                double strata1Vol = 0;
                for (double h = outProfile->resolution() / 2.0 ; h < _limStata1; h += outProfile->resolution())
                {
                    double val = outProfile->valueAtLength(h);
                    if (val != outProfile->NA())
                    {
                        strata1Vol += val;
                    }
                }

                outProfile->addItemAttribute(new CT_StdItemAttributeT<double>(_outProfileThreshold_ModelName.completeName(), CT_AbstractCategory::DATA_NUMBER, outResult, threshold));
                outProfile->addItemAttribute(new CT_StdItemAttributeT<double>(_outProfileStrata1_ModelName.completeName(), CT_AbstractCategory::DATA_NUMBER, outResult, strata1Vol));

                outProfile->setDisplayableName(QString("Thres%1").arg(threshold));
            }
        }
    }


}

#endif
