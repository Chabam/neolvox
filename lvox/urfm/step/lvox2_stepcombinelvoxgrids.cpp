#include "lvox2_stepcombinelvoxgrids.h"

#include "ct_view/ct_stepconfigurabledialog.h"
#include "ct_result/model/inModel/ct_inresultmodelgroup.h"
#include "ct_result/model/outModel/ct_outresultmodelgroup.h"
#include "ct_turn/inTurn/tools/ct_inturnmanager.h"
#include "ct_model/tools/ct_modelsearchhelper.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

// Inclusion of standard result class
#include "ct_result/ct_resultgroup.h"

// Inclusion of used ItemDrawable classes
#include "ct_itemdrawable/ct_grid3d.h"
#include "qdebug.h"

// MNT
#include "ct_itemdrawable/ct_image2d.h"


// Alias for indexing in models
#define DEF_resultIn_grids "rgrids"
#define DEF_groupIn_grids "grids"
#define DEF_itemIn_hits "hits"
#define DEF_itemIn_theoretical "theoretical"
#define DEF_itemIn_before "before"
#define DEF_itemIn_density "density"
#define DEF_itemIn_deltaTheoritical "deltaTheoritical"

#define DEF_inATTisNi "isNi"
#define DEF_inATTisNt "isNt"
#define DEF_inATTisNb "isNb"
#define DEF_inATTisDensity "isDensity"
// optional MNT
#define DEF_SearchInMNTResult        "rmnt"
#define DEF_SearchInMNTGroup         "gmnt"
#define DEF_SearchInMNT              "mnt"

// Alias for indexing out models
#define DEF_resultOut_grids "rgrids"
#define DEF_groupOut_grids "grids"
#define DEF_itemOut_hits "hits"
#define DEF_itemOut_theoretical "theoretical"
#define DEF_itemOut_before "before"
#define DEF_itemOut_density "density"
#define DEF_itemOut_scanId "scanId"
#define DEF_itemOut_deltaT "deltaT"

#include <math.h>

// Constructor : initialization of parameters
LVOX2_StepCombineLvoxGrids::LVOX2_StepCombineLvoxGrids(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
    _mode = maxNt_Nb;
    _effectiveRayThresh = 10;
    _UseOnlyNotEmptyCellsIf_maxNt_Nb_div_Nt_selected = true;
}

// Step description (tooltip of contextual menu)
QString LVOX2_StepCombineLvoxGrids::getStepDescription() const
{
    return tr("3bis- Combiner les grilles LVOX des différents points de vues");
}

// Step copy method
CT_VirtualAbstractStep* LVOX2_StepCombineLvoxGrids::createNewInstance(CT_StepInitializeData &dataInit)
{
    return new LVOX2_StepCombineLvoxGrids(dataInit);
}

//////////////////// PROTECTED METHODS //////////////////

// Creation and affiliation of IN models
void LVOX2_StepCombineLvoxGrids::createInResultModelListProtected()
{
    CT_InResultModelGroup *resultModel = createNewInResultModel(DEF_resultIn_grids, tr("Grilles"));

    resultModel->setZeroOrMoreRootGroup();
    resultModel->addGroupModel("", DEF_groupIn_grids);

    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_hits, CT_Grid3D<int>::staticGetType(), tr("hits"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    resultModel->addItemAttributeModel(DEF_itemIn_hits, DEF_inATTisNi, QList<QString>() << "LVOX_GRD_NI", CT_AbstractCategory::ANY, tr("isNi"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);


    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_theoretical, CT_Grid3D<int>::staticGetType(), tr("theoretical"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    resultModel->addItemAttributeModel(DEF_itemIn_theoretical, DEF_inATTisNt, QList<QString>() << "LVOX_GRD_NT", CT_AbstractCategory::ANY, tr("isNt"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_before, CT_Grid3D<int>::staticGetType(), tr("before"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    resultModel->addItemAttributeModel(DEF_itemIn_before, DEF_inATTisNb, QList<QString>() << "LVOX_GRD_NB", CT_AbstractCategory::ANY, tr("isNb"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_density, CT_Grid3D<float>::staticGetType(), tr("density"));
    resultModel->addItemAttributeModel(DEF_itemIn_density, DEF_inATTisDensity, QList<QString>() << "LVOX_GRD_DENSITY", CT_AbstractCategory::ANY, tr("isDensity"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    resultModel->addItemModel(DEF_groupIn_grids, DEF_itemIn_deltaTheoritical, CT_Grid3D<float>::staticGetType(), tr("delta th."), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    // MNT
    CT_InResultModelGroup* resultModelMNT = createNewInResultModel(DEF_SearchInMNTResult, tr("MNT (Raster)"), "", true);
    resultModelMNT->setZeroOrMoreRootGroup();
    resultModelMNT->addGroupModel("", DEF_SearchInMNTGroup);
    resultModelMNT->addItemModel(DEF_SearchInMNTGroup, DEF_SearchInMNT, CT_Image2D<float>::staticGetType(), tr("Modèle Numérique de Terrain"));
    resultModelMNT->setMinimumNumberOfPossibilityThatMustBeSelectedForOneTurn(0);
}

// Creation and affiliation of OUT models
void LVOX2_StepCombineLvoxGrids::createOutResultModelListProtected()
{
    QString modeString;
    if (_mode == maxDensity)    {modeString = "max(density)";}
    else if (_mode == maxNt_Nb) {modeString = "max(nt-nb)";}
    else if (_mode == maxNt_Nb_div_Nt) {modeString = "max(nt-nb)/nt";}
    else if (_mode == maxNi)    {modeString = "max(ni)";}
    else if (_mode == sumNiSumNtNb)    {modeString = "sum(ni)/sum(nt-nb)";}

    CT_OutResultModelGroup *resultModel = createNewOutResultModel(DEF_resultOut_grids, tr("Grilles combinées"));

    resultModel->setRootGroup(DEF_groupOut_grids, new CT_StandardItemGroup(), tr("Grilles"));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_hits, new CT_Grid3D<int>(), tr("hits"));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_theoretical, new CT_Grid3D<int>(), tr("theoretical"));
    resultModel->addItemAttributeModel(DEF_itemOut_theoretical, DEF_inATTisNt, new CT_StdItemAttributeT<bool>("LVOX_GRD_NT"), tr("isNt"));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_before, new CT_Grid3D<int>(), tr("before"));
    resultModel->addItemAttributeModel(DEF_itemOut_before, DEF_inATTisNb, new CT_StdItemAttributeT<bool>("LVOX_GRD_NB"), tr("isNb"));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_density, new CT_Grid3D<float>(), tr("density - %1").arg(modeString));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_deltaT, new CT_Grid3D<float>(), tr("delta theoretical"));
    resultModel->addItemModel(DEF_groupOut_grids, DEF_itemOut_scanId, new CT_Grid3D<int>(), tr("scanId"));

    setMaximumTurn(1);
}

// Semi-automatic creation of step parameters DialogBox
void LVOX2_StepCombineLvoxGrids::createPostConfigurationDialog()
{
    CT_InTurnManager *man = getInTurnManager();
    man->getTurnIndexManager()->resetTurnIndex();
    man->createSearchModelListForCurrentTurn();

    // Gets in models to test the presence of optional grids
    CT_InResultModelGroup* resultIn_model = (CT_InResultModelGroup*) man->getInResultModel(DEF_resultIn_grids);
    CT_OutAbstractResultModel *resultOut_model = (CT_OutAbstractResultModel*) resultIn_model->getPossibilitiesSavedSelected().first()->outModel();

    //CT_ResultGroup* resultIn_grids = getInputResults().first();

    CT_InAbstractSingularItemModel *itemInModel_hits = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_hits, resultOut_model, this);
    CT_InAbstractSingularItemModel *itemInModel_theoretical = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_theoretical, resultOut_model, this);
    CT_InAbstractSingularItemModel *itemInModel_before = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_before, resultOut_model, this);

    CT_StepConfigurableDialog *configDialog = newStandardPostConfigurationDialog();


    CT_ButtonGroup &bg_mode = configDialog->addButtonGroup(_mode);

    configDialog->addText(tr("Mode de combinaison"), tr("des grilles de densité :"),"");
    configDialog->addExcludeValue(tr("max (densité)"), "", "", bg_mode, maxDensity);

    if (itemInModel_theoretical->isAtLeastOnePossibilitySelected() && itemInModel_before->isAtLeastOnePossibilitySelected())
    {
        configDialog->addExcludeValue(tr("max (nt - nb)"), "", "", bg_mode, maxNt_Nb);
    } else {
        configDialog->addText(tr("max (nt - nb)"), tr("Non disponible : "), tr("grille(s) nt/nb manquante(s)"));
    }

    if (itemInModel_theoretical->isAtLeastOnePossibilitySelected() && itemInModel_before->isAtLeastOnePossibilitySelected())
    {
        configDialog->addExcludeValue(tr("max (nt - nb)/nt"), "", "", bg_mode, maxNt_Nb_div_Nt);
        configDialog->addBool(tr("    -> Ignorer les cas à densité nulle"),"", "", _UseOnlyNotEmptyCellsIf_maxNt_Nb_div_Nt_selected);
    } else {
        configDialog->addText(tr("max (nt - nb)/nt"), tr("Non disponible : "), tr("grille(s) nt/nb manquante(s)"));
    }

    if (itemInModel_hits->isAtLeastOnePossibilitySelected())
    {
        configDialog->addExcludeValue(tr("max (ni)"), "", "", bg_mode, maxNi);
    } else {
        configDialog->addText(tr("max (ni)"), tr("Non disponible : "), tr("grille ni manquante"));
    }

    if (itemInModel_hits->isAtLeastOnePossibilitySelected() && itemInModel_theoretical->isAtLeastOnePossibilitySelected() && itemInModel_before->isAtLeastOnePossibilitySelected())
    {
        configDialog->addExcludeValue(tr("sum(ni) / sum(nt - nb)"), "", "", bg_mode, sumNiSumNtNb);
        configDialog->addInt(tr("    -> (nt - nb) minimum"),"",-100000,100000, _effectiveRayThresh );
    } else {
        configDialog->addText(tr("sum(ni) / sum(nt - nb)"), tr("Non disponible : "), tr("grille(s) ni/nt/nb manquante(s)"));
    }
}

void LVOX2_StepCombineLvoxGrids::compute()
{
    // Modéles pour récupérer les MNT
    qDebug() << "Trying to read MNT";
    CT_Image2D<float>* dtm = NULL;
    if (getInputResults().size() > 1)
    {
         qDebug() << "  There is one";
        CT_ResultGroup* inMNTResult = getInputResults().at(1);

        CT_ResultItemIterator itMNT(inMNTResult, this, DEF_SearchInMNT);
        // on demande si on peut commencer à parcourir les groupes
        if(itMNT.hasNext())
        {
            dtm = (CT_Image2D<float>*)itMNT.next();
            qDebug() << "  It has been correctly red";
        }
    }

    CT_ResultGroup* resultIn_grids = getInputResults().first();

    CT_InAbstractSingularItemModel *itemInModel_hits = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_hits, resultIn_grids, this);
    CT_InAbstractSingularItemModel *itemInModel_theoretical = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_theoretical, resultIn_grids, this);
    CT_InAbstractSingularItemModel *itemInModel_before = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_before, resultIn_grids, this);
    CT_InAbstractSingularItemModel *itemInModel_deltaTheo = (CT_InAbstractSingularItemModel*)PS_MODELS->searchModel(DEF_itemIn_deltaTheoritical, resultIn_grids, this);

    bool use_ni = itemInModel_hits->isAtLeastOnePossibilitySelected();
    bool use_nt = itemInModel_theoretical->isAtLeastOnePossibilitySelected();
    bool use_nb = itemInModel_before->isAtLeastOnePossibilitySelected();
    bool use_deltaT = itemInModel_deltaTheo->isAtLeastOnePossibilitySelected();

    // coherence control between models and _mode parameter
    if (_mode == maxNt_Nb && (!use_nt || !use_nb)) {qDebug() << "Configuration non prévue !"; return;}
    if (_mode == maxNt_Nb_div_Nt && (!use_nt || !use_nb)) {qDebug() << "Configuration non prévue !"; return;}
    if (_mode == maxNi && !use_ni) {qDebug() << "Configuration non prévue !"; return;}
    if (_mode == sumNiSumNtNb && (!use_nt || !use_nb || !use_ni)) {qDebug() << "Configuration non prévue !"; return;}

    double xmin, ymin, zmin, res, NAd;
    size_t xdim, ydim, zdim;
    int NAi, NAt, NAb, NAdelta;
    bool firstGrid = true;

    QVector<const CT_Grid3D<int>*> InGrids_hits; // vector contenant toutes les grilles
    QVector<const CT_Grid3D<int>*> InGrids_theoretical;
    QVector<const CT_Grid3D<int>*> InGrids_before;
    QVector<const CT_Grid3D<float>*> InGrids_density;
    QVector<const CT_Grid3D<float>*> InGrids_deltaT;

    // Iterating on groups situated at the root of the result (corresponding to DEF_groupIn_grids)
    // => Create grids lists, and retrieve grids characteristics
    CT_ResultGroupIterator itGrp(resultIn_grids, this, DEF_groupIn_grids);
    while (itGrp.hasNext() && !isStopped())
    {
        CT_AbstractItemGroup *groupIn_grids = (CT_AbstractItemGroup*) itGrp.next();

        // grids for current scan
        const CT_Grid3D<int>* itemIn_hits;
        const CT_Grid3D<int>* itemIn_theoretical;
        const CT_Grid3D<int>* itemIn_before;
        const CT_Grid3D<float>* itemIn_density;
        const CT_Grid3D<float>* itemIn_deltaT;

        if (use_ni) {itemIn_hits = (const CT_Grid3D<int>*) groupIn_grids->firstItemByINModelName(this, DEF_itemOut_hits);}
        if (use_nt) {itemIn_theoretical = (const CT_Grid3D<int>*) groupIn_grids->firstItemByINModelName(this, DEF_itemIn_theoretical);}
        if (use_nb) {itemIn_before = (const CT_Grid3D<int>*) groupIn_grids->firstItemByINModelName(this, DEF_itemIn_before);}
        if (use_deltaT) {itemIn_deltaT = (const CT_Grid3D<float>*) groupIn_grids->firstItemByINModelName(this, DEF_itemIn_deltaTheoritical);}
        itemIn_density = (const CT_Grid3D<float>*) groupIn_grids->firstItemByINModelName(this, DEF_itemIn_density);

        // test la présence de la grille de densité
        if (itemIn_density!=NULL)
        {
            if (use_ni && itemIn_hits==NULL) {qDebug() << tr("Grille ni manquante !");return; }
            if (use_nt && itemIn_theoretical==NULL) {qDebug() << tr("Grille nt manquante !");return; }
            if (use_nb && itemIn_before==NULL) {qDebug() << tr("Grille nb manquante !");return; }
            if (use_deltaT && itemIn_deltaT==NULL) {qDebug() << tr("Grille delta theorique manquante !");return; }

            if (firstGrid)
            {
                firstGrid = false;
                xmin = itemIn_density->minX();
                ymin = itemIn_density->minY();
                zmin = itemIn_density->minZ();
                xdim = itemIn_density->xdim();
                ydim = itemIn_density->ydim();
                zdim = itemIn_density->zdim();
                res = itemIn_density->resolution();
                NAd = itemIn_density->NA();
                if (use_ni) {NAi = itemIn_hits->NA();}
                if (use_nt) {NAt = itemIn_theoretical->NA();}
                if (use_nb) {NAb = itemIn_before->NA();}
                if (use_deltaT) {NAdelta = itemIn_deltaT->NA();}
            } else {
//                if (xmin != itemIn_density->minX()) {qDebug() << "xmin non homogène !"; return; }
//                if (ymin != itemIn_density->minY()) {qDebug() << "ymin non homogène !"; return; }
//                if (zmin != itemIn_density->minZ()) {qDebug() << "zmin non homogène !"; return; }
//                if (xdim != itemIn_density->xdim()) {qDebug() << "xdim non homogène !"; return; }
//                if (ydim != itemIn_density->ydim()) {qDebug() << "ydim non homogène !"; return; }
//                if (zdim != itemIn_density->zdim()) {qDebug() << "zdim non homogène !"; return; }
//                if (res != itemIn_density->resolution()) {qDebug() << "resolution non homogène !"; return; }
//                if (NAd != itemIn_density->NA()) {qDebug() << "NAd non homogène !"; return; }
//                if (NAi != itemIn_hits->NA()) {qDebug() << "NAi non homogène !"; return; }
//                if (NAt != itemIn_theoretical->NA()) {qDebug() << "NAt non homogène !"; return; }
//                if (NAb != itemIn_before->NA()) {qDebug() << "NAb non homogène !"; return; }
//                if (NAdelta != itemIn_deltaT->NA()) {qDebug() << "NAb non homogène !"; return; }
            }
            //qDebug() << "   end firstgrid";
            if (use_ni) {InGrids_hits.append(itemIn_hits);}
            if (use_nt) {InGrids_theoretical.append(itemIn_theoretical);}
            if (use_nb) {InGrids_before.append(itemIn_before);}
            if (use_deltaT) {InGrids_deltaT.append(itemIn_deltaT);}
            InGrids_density.append(itemIn_density);
        } else
        {
            qDebug() << "Grille de densité manquante ! Le traitement continue avec les autres.";
        }
    }
    if (InGrids_density.size() <=0) {qDebug() << "Aucune Grille !"; return;}
    qDebug() << "   All grid collected";

    // Create combined out grids, considering optional input grids
    QList<CT_ResultGroup*> outResultList = getOutResultList();
    CT_ResultGroup* resultOut_grids = outResultList.at(0);

    CT_StandardItemGroup* groupOut_grids = new CT_StandardItemGroup(DEF_groupOut_grids, resultOut_grids);

    CT_Grid3D<int>* itemOut_hits = NULL;
    if (use_ni)
    {
        itemOut_hits = new CT_Grid3D<int>(DEF_itemOut_hits, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, NAi, NAi);
        groupOut_grids->addItemDrawable(itemOut_hits);
    }

    CT_Grid3D<int>* itemOut_theoretical = NULL;
    if (use_nt)
    {
        itemOut_theoretical = new CT_Grid3D<int>(DEF_itemOut_theoretical, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, NAt, NAt);
        groupOut_grids->addItemDrawable(itemOut_theoretical);
    }

    CT_Grid3D<int>* itemOut_before = NULL;
    if (use_nb)
    {
        itemOut_before = new CT_Grid3D<int>(DEF_itemOut_before, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, NAb, NAb);
        groupOut_grids->addItemDrawable(itemOut_before);
    }

    CT_Grid3D<float>* itemOut_deltaT = NULL;
    if (use_deltaT)
    {
        itemOut_deltaT = new CT_Grid3D<float>(DEF_itemOut_deltaT, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, NAdelta, NAdelta);
        groupOut_grids->addItemDrawable(itemOut_deltaT);
    }

    CT_Grid3D<float>* itemOut_density = new CT_Grid3D<float>(DEF_itemOut_density, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, NAd, NAd);
    groupOut_grids->addItemDrawable(itemOut_density);

    CT_Grid3D<int>* itemOut_scanId = new CT_Grid3D<int>(DEF_itemOut_scanId, resultOut_grids, xmin, ymin, zmin, xdim, ydim, zdim, res, -1, -1);
    groupOut_grids->addItemDrawable(itemOut_scanId);

    resultOut_grids->addGroup(groupOut_grids);


    const CT_Grid3D<float>* in_d = NULL;
    const CT_Grid3D<int>* in_ni = NULL;
    const CT_Grid3D<int>* in_nt = NULL;
    const CT_Grid3D<int>* in_nb = NULL;
    const CT_Grid3D<float>* in_deltaT = NULL;
    qDebug() << "  Computing combined grids...";

    // Compute combined grids values
    // Init with first grids
    in_d = InGrids_density.at(0);
    if (use_ni) {in_ni = InGrids_hits.at(0);}
    if (use_nt) {in_nt = InGrids_theoretical.at(0);}
    if (use_nb) {in_nb = InGrids_before.at(0);}
    if (use_deltaT) {in_deltaT = InGrids_deltaT.at(0);}

    for (size_t xx = 0 ; xx < xdim ; xx++)
    {
        for (size_t yy = 0 ; yy < ydim ; yy++)
        {
            for (size_t zz = 0 ; zz < zdim ; zz++)
            {
                size_t index;
                in_d->index(xx, yy, zz, index);
                itemOut_density->setValueAtIndex(index, in_d->valueAtIndex(index));
                if (use_ni) {itemOut_hits->setValueAtIndex(index, in_ni->valueAtIndex(index));}
                if (use_nt) {itemOut_theoretical->setValueAtIndex(index, in_nt->valueAtIndex(index));}
                if (use_nb) {itemOut_before->setValueAtIndex(index, in_nb->valueAtIndex(index));}
                if (use_deltaT) {itemOut_deltaT->setValueAtIndex(index, in_deltaT->valueAtIndex(index)*(float)in_nt->valueAtIndex(index));}
                itemOut_scanId->setValueAtIndex(index, 0);
            }
        }
    }
    qDebug() << "  Initialization with first grid done. Starting other grids...";
    // Compare with others grids
    for (int i = 1 ; i < InGrids_density.size() ; i++)
    {
        //qDebug() << "i=" << i;
        in_d = InGrids_density.at(i);
        if (use_ni) {in_ni = InGrids_hits.at(i);}
        if (use_nt) {in_nt = InGrids_theoretical.at(i);}
        if (use_nb) {in_nb = InGrids_before.at(i);}
        if (use_deltaT) {in_deltaT = InGrids_deltaT.at(i);}
        //qDebug() << "i=" << i;
        for (size_t xx = 0 ; xx < xdim ; xx++)
        {
            for (size_t yy = 0 ; yy < ydim ; yy++)
            {
                for (size_t zz = 0 ; zz < zdim ; zz++)
                {
                    size_t index;
                    in_d->index(xx, yy, zz, index);
                    bool replace = false;

                    if (_mode == maxDensity) {
                        replace = (in_d->valueAtIndex(index) > itemOut_density->valueAtIndex(index));
                    } else if (_mode == maxNt_Nb) {
                        //qDebug() << "before replace";
                        replace = ((in_nt->valueAtIndex(index) - in_nb->valueAtIndex(index)) > (itemOut_theoretical->valueAtIndex(index) - itemOut_before->valueAtIndex(index)));
                    } else if (_mode == maxNt_Nb_div_Nt) {
                        float inVal = 0;
                        float outVal = 0;

                        if (in_nt->valueAtIndex(index) > 0) {inVal = (float)(in_nt->valueAtIndex(index) - in_nb->valueAtIndex(index))/(float)in_nt->valueAtIndex(index);}
                        if (itemOut_theoretical->valueAtIndex(index) > 0) {outVal = (float)(itemOut_theoretical->valueAtIndex(index) - itemOut_before->valueAtIndex(index))/(float)itemOut_theoretical->valueAtIndex(index);}

                        replace = (inVal > outVal);

                        if (_UseOnlyNotEmptyCellsIf_maxNt_Nb_div_Nt_selected && (in_d->valueAtIndex(index) <= 0)) {replace = false;}

                    } else if (_mode == maxNi) {
                        replace = (in_ni->valueAtIndex(index) > itemOut_hits->valueAtIndex(index));
                    } else if (_mode == sumNiSumNtNb) {
                        //qDebug() << "i=" <<i;
                        replace = false;
                        //qDebug() << "ni=" <<in_ni->valueAtIndex(index);
                        //qDebug() << "nt=" <<in_nt->valueAtIndex(index);
                        //qDebug() << "nb=" <<in_nb->valueAtIndex(index);
                        //qDebug() << use_deltaT;
                        //qDebug() << "ndelta=" <<in_deltaT->valueAtIndex(index)*(float)in_nt->valueAtIndex(index);

                        itemOut_hits->addValueAtIndex(index, in_ni->valueAtIndex(index));
                        itemOut_theoretical->addValueAtIndex(index, in_nt->valueAtIndex(index));
                        itemOut_before->addValueAtIndex(index, in_nb->valueAtIndex(index));
                        if (use_deltaT) {itemOut_deltaT->addValueAtIndex(index, in_deltaT->valueAtIndex(index)*(float)in_nt->valueAtIndex(index));}

                        if (i == (InGrids_density.size() - 1))
                        {
                            int ni = itemOut_hits->valueAtIndex(index);
                            int nt = itemOut_theoretical->valueAtIndex(index);
                            int nb = itemOut_before->valueAtIndex(index);
                            //qDebug() << "ni,nt,nb";
                            int ntMnb = nt - nb;

                            // Avoid division by 0
                            if (ntMnb == 0 )
                            {
                                itemOut_density->setValueAtIndex(index, -1);
                                if (use_deltaT) {itemOut_deltaT->setValueAtIndex(index, -1);}
                            }
                            // If there is an error (nb > nt)
                            else if (ntMnb < 0 )
                            {
                                itemOut_density->setValueAtIndex(index, -2);
                                if (use_deltaT) {itemOut_deltaT->setValueAtIndex(index, -2);}
                            }
                            // If there is not enough information
                            else if (ntMnb < _effectiveRayThresh )
                            {
                                itemOut_density->setValueAtIndex(index, -3);
                                if (use_deltaT) {itemOut_deltaT->setValueAtIndex(index, -3);}
                            }
                            // Normal case
                            else
                            {
                                float density = (float) ni / ((float) ntMnb);
                                itemOut_density->setValueAtIndex(index, density);
                                if (use_deltaT) {
                                    //TODO
                                    qDebug() << "use_deltaT not implemented";
                                    exit(1);
                                }
                            }

                            if (ni > (nt - nb))
                            {
                                itemOut_density->setValueAtIndex(i, 1);
                            }

                        }
                    }

                    if (replace)
                    {
                        itemOut_density->setValueAtIndex(index, in_d->valueAtIndex(index));

                        if (use_ni) {itemOut_hits->setValueAtIndex(index, in_ni->valueAtIndex(index));}
                        if (use_nt) {itemOut_theoretical->setValueAtIndex(index, in_nt->valueAtIndex(index));}
                        if (use_nb) {itemOut_before->setValueAtIndex(index, in_nb->valueAtIndex(index));}
                        if (use_deltaT) {itemOut_deltaT->setValueAtIndex(index, in_deltaT->valueAtIndex(index) * (float)in_nt->valueAtIndex(index));}
                        itemOut_scanId->setValueAtIndex(index, i);

                    }
                   // qDebug() << "vox="<< xx << yy << zz;
                }
            }
        }

    }
     if (use_deltaT)
    {
        qDebug() << " Building delta grid...";
        for (size_t xx = 0 ; xx < xdim ; xx++)
        {
            for (size_t yy = 0 ; yy < ydim ; yy++)
            {
                for (size_t zz = 0 ; zz < zdim ; zz++)
                {
                    size_t index;
                    itemOut_deltaT->index(xx, yy, zz, index);
                    itemOut_deltaT->setValueAtIndex(index, itemOut_deltaT->valueAtIndex(index) / (float)itemOut_theoretical->valueAtIndex(index));
                }
            }
        }
    }
     //int invisible=0;
     double thres =0.1;
    if (dtm!=NULL) { // put an average density in unseen vox above ground
        // loop on z levels
        qDebug() << " Filling hidden voxels...";

        //    for (size_t zz = 0 ;  zz < itemOut_density->zdim() ; zz++)
        //    {
            for (size_t yy = 0 ;  yy < itemOut_density->ydim() ; yy++)
            {
                for (size_t xx = 0 ;  xx < itemOut_density->xdim() ; xx++)
                {
                     bool belowVeg = false;
                     double zDTM = dtm->valueAtCoords(itemOut_density->getCellCenterX(xx), itemOut_density->getCellCenterY(yy));
                    for (size_t zz = 0 ;  zz < itemOut_density->zdim() ; zz++)
                        //size_t zz = itemOut_density->zdim()-1 ;  zz >= 0 ; zz--)
                    {
                        size_t zz2 =  itemOut_density->zdim()-1 - zz;
                        if (xx==0&&yy==0) {
                            //qDebug() << xx <<yy<<zz2; //<<itemOut_density->zdim()<<itemOut_density->xdim();
                        }
                        //exit(1);
                        size_t index;
                        itemOut_density->index(xx, yy, zz2, index);
                        double value = itemOut_density ->valueAtIndexAsDouble(index);
                        if (belowVeg&&itemOut_density->getCellCenterZ(zz2) >= zDTM) {
                            if (value<=thres) {
                                bool viewed = (itemOut_theoretical ->valueAtIndex(index)-itemOut_before ->valueAtIndex(index)>=100);
                                if (!viewed) {
                                   // qDebug() <<"not viewed"<< xx <<yy<<zz2;
                                    itemOut_density->setValueAtIndexFromDouble(index, 1.1f);
                                }
                            }
                        }
                        if (!belowVeg && value >= thres) {
                            belowVeg = true;
                         //   qDebug() <<"voxel not empty below " <<xx <<yy<<zz2;
                          //    qDebug() << "     "     <<itemOut_density->getCellCenterZ(zz2-1) << zDTM;
                        }




//                    bool viewed = (itemOut_theoretical ->valueAtIndex(index)-itemOut_before ->valueAtIndex(index)>=1);
//                    if (!viewed)
//                    {
//                        invisible++;
//                    }
//                   viewed = (itemOut_theoretical ->valueAtIndex(index)-itemOut_before ->valueAtIndex(index)>=10);
//                    double value = itemOut_density ->valueAtIndexAsDouble(index);
//                    double zDTM = dtm->valueAtCoords(itemOut_density->getCellCenterX(xx), itemOut_density->getCellCenterY(yy));
//                    if (value <= 0.f && itemOut_density->getCellCenterZ(zz) >= zDTM &&!viewed)
//                    {
//                        itemOut_density->setValueAtIndexFromDouble(index, 10.0f);
//                    }
                }
            }
        }
         //qDebug() << "Invisible points ="<<invisible;
    }

    if (itemOut_hits!=NULL) {itemOut_hits->computeMinMax();}
    if (itemOut_theoretical!=NULL) {itemOut_theoretical->computeMinMax();}
    if (itemOut_before!=NULL) {itemOut_before->computeMinMax();}
    if (itemOut_density!=NULL) {itemOut_density->computeMinMax();}
    if (itemOut_deltaT!=NULL) {itemOut_deltaT->computeMinMax();}
    if (itemOut_scanId!=NULL) {itemOut_scanId->computeMinMax();}
    qDebug() << " Combination terminated";
}
