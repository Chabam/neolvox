#include "lvox2_steppreparepointcloud.h"


// Utilise le depot
#include "ct_global/ct_context.h"

#include "ct_view/ct_stepconfigurabledialog.h"
#include "ct_result/model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_result/model/outModel/ct_outresultmodelgroupcopy.h"
#include "ct_result/model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"
#include "ct_iterator/ct_pointiterator.h"

// Inclusion of standard result class
#include "ct_result/ct_resultgroup.h"

// Inclusion of used ItemDrawable classes
#include "ct_itemdrawable/ct_scene.h"

#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"

// Alias for indexing in models
#define DEF_SearchInResult "inputResult"
#define DEF_SearchInGroup "inputSceneGrp"
#define DEF_itemIn_scene "inputScene"

// Constructor : initialization of parameters
LVOX2_StepPreparePointCloud::LVOX2_StepPreparePointCloud(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
}

// Step description (tooltip of contextual menu)
QString LVOX2_StepPreparePointCloud::getStepDescription() const
{
    return tr("Préparer la scène de points");
}

// Step copy method
CT_VirtualAbstractStep* LVOX2_StepPreparePointCloud::createNewInstance(CT_StepInitializeData &dataInit)
{
    return new LVOX2_StepPreparePointCloud(dataInit);
}

//////////////////// PROTECTED METHODS //////////////////

// Creation and affiliation of IN models
void LVOX2_StepPreparePointCloud::createInResultModelListProtected()
{
    CT_InResultModelGroupToCopy *resultInModel = createNewInResultModelForCopy(DEF_SearchInResult, tr("Scene"));
    resultInModel->setZeroOrMoreRootGroup();
    resultInModel->addGroupModel("", DEF_SearchInGroup);
    resultInModel->addItemModel(DEF_SearchInGroup, DEF_itemIn_scene, CT_Scene::staticGetType(),tr("Scene"));
}

// Creation and affiliation of OUT models
void LVOX2_StepPreparePointCloud::createOutResultModelListProtected()
{
    CT_OutResultModelGroupToCopyPossibilities *res = createNewOutResultModelToCopy(DEF_SearchInResult);

    if(res != NULL)
    {
        res->addItemModel(DEF_SearchInGroup, _outFlagAttribute_ModelName, new CT_PointsAttributesScalarTemplated<int>(), tr("Flag"));
    }
}

// Semi-automatic creation of step parameters DialogBox
void LVOX2_StepPreparePointCloud::createPostConfigurationDialog()
{
    // No dialog box
}

void LVOX2_StepPreparePointCloud::compute()
{
    CT_ResultGroup* outResult = getOutResultList().first();

    CT_ResultGroupIterator itOut(outResult, this, DEF_SearchInGroup);
    // iterate over all groups
    while(itOut.hasNext())
    {
        CT_StandardItemGroup *group = (CT_StandardItemGroup*)itOut.next();
        const CT_Scene* scene = (const CT_Scene*)group->firstItemByINModelName(this, DEF_itemIn_scene);

        if(scene != NULL)
        {
            const CT_AbstractPointCloudIndex* pointCloudIndex = scene->getPointCloudIndex();
            size_t nbPoints = pointCloudIndex->size();
            CT_StandardCloudStdVectorT<int> *attributes = new CT_StandardCloudStdVectorT<int>(nbPoints);
            int minVal = std::numeric_limits<int>::max();
            int maxVal = -std::numeric_limits<int>::max();

            size_t cpt = 0;
            CT_PointIterator itP(pointCloudIndex);
            while(itP.hasNext() && (!isStopped()))
            {
                const CT_Point &point = itP.next().currentPoint();

                int flagVal = 0;
                if (point(0) > 0)
                {
                    flagVal = 1;
                }

                if (flagVal < minVal) {minVal = flagVal;}
                if (flagVal > maxVal) {maxVal = flagVal;}

                (*attributes)[cpt++] = flagVal;
            }

            CT_PointsAttributesScalarTemplated<int> *attributesItem = new CT_PointsAttributesScalarTemplated<int>(_outFlagAttribute_ModelName.completeName(),
                                                                                                               outResult,
                                                                                                               scene->getPointCloudIndexRegistered(),
                                                                                                               attributes,
                                                                                                               minVal,
                                                                                                               maxVal);
            group->addItemDrawable(attributesItem);
        }
    }
}
