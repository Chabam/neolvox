#include "lvox2_stepexportcomputedgrids.h"

#include "ct_itemdrawable/ct_grid3d.h"
#include "ct_itemdrawable/tools/iterator/ct_groupiterator.h"
#include "ct_result/ct_resultgroup.h"
#include "ct_result/model/inModel/ct_inresultmodelgroup.h"
#include "ct_view/ct_stepconfigurabledialog.h"
#include "ct_model/tools/ct_modelsearchhelper.h"
#include "ctlibio/readers/ct_reader_asciigrid3d.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "tools/lvox_grid3dexporter.h"
#include "qdir.h"

// Alias for indexing models
#define DEF_in_res "result"
#define DEF_in_grp "group"
#define DEF_hits "hits"
#define DEF_theo "theoretical"
#define DEF_before "before"
#define DEF_density "density"
#define DEF_attr_ni "isNi"
#define DEF_attr_nt "isNt"
#define DEF_attr_nb "isNb"
#define DEF_attr_density "isDensity"

// Constructor : initialization of parameters
LVOX2_StepExportComputedGrids::LVOX2_StepExportComputedGrids(CT_StepInitializeData &dataInit) : CT_AbstractStep(dataInit)
{
}

// Step description (tooltip of contextual menu)
QString LVOX2_StepExportComputedGrids::getStepDescription() const
{
    return tr("2a - Export computed grids");
}

// Step detailled description
QString LVOX2_StepExportComputedGrids::getStepDetailledDescription() const
{
    return tr("Export all the computed grids into a folder");
}

// Step URL
QString LVOX2_StepExportComputedGrids::getStepURL() const
{
    //return tr("STEP URL HERE");
    return CT_AbstractStep::getStepURL(); //by default URL of the plugin
}

// Step copy method
CT_VirtualAbstractStep* LVOX2_StepExportComputedGrids::createNewInstance(CT_StepInitializeData &dataInit)
{
    return new LVOX2_StepExportComputedGrids(dataInit);
}

//////////////////// PROTECTED METHODS //////////////////

// Creation and affiliation of IN models
void LVOX2_StepExportComputedGrids::createInResultModelListProtected()
{
    CT_InResultModelGroup* in_res = createNewInResultModel(DEF_in_res, tr("Grilles"));

    in_res->setZeroOrMoreRootGroup();
    in_res->addGroupModel("", DEF_in_grp);

    in_res->addItemModel(DEF_in_grp, DEF_hits, CT_Grid3D<int>::staticGetType(), tr("hits"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    in_res->addItemAttributeModel(DEF_hits, DEF_attr_ni, QList<QString>() << "LVOX_GRD_NI", CT_AbstractCategory::ANY, tr("isNi"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    in_res->addItemModel(DEF_in_grp, DEF_theo, CT_Grid3D<int>::staticGetType(), tr("theoretical"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    in_res->addItemAttributeModel(DEF_theo, DEF_attr_nt, QList<QString>() << "LVOX_GRD_NT", CT_AbstractCategory::ANY, tr("isNt"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    in_res->addItemModel(DEF_in_grp, DEF_before, CT_Grid3D<int>::staticGetType(), tr("before"), "",
                              CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    in_res->addItemAttributeModel(DEF_before, DEF_attr_nb, QList<QString>() << "LVOX_GRD_NB", CT_AbstractCategory::ANY, tr("isNb"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);

    in_res->addItemModel(DEF_in_grp, DEF_density, CT_Grid3D<float>::staticGetType(), tr("density"));
    in_res->addItemAttributeModel(DEF_density, DEF_attr_density, QList<QString>() << "LVOX_GRD_DENSITY", CT_AbstractCategory::ANY, tr("isDensity"), "", CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
}

// Creation and affiliation of OUT models
void LVOX2_StepExportComputedGrids::createOutResultModelListProtected()
{
    // No out result, just files on disk
}

// Semi-automatic creation of step parameters DialogBox
void LVOX2_StepExportComputedGrids::createPostConfigurationDialog()
{
    CT_StepConfigurableDialog* diag = newStandardPostConfigurationDialog();

    diag->addFileChoice(tr("Choisir la destination"), CT_FileChoiceButton::OneNewFile, "", _folder, "Choisissez le dossier de destination à créer", "", "");
}

//template<class T>
//CT_Grid3D<T>* readFile(QString filename, CT_OutAbstractSingularItemModel* model, CT_ResultGroup* result)
//{
//    CT_Reader_AsciiGrid3D reader{std::is_same<T, float>::value};
//    CT_Grid3D<T>* grid;

//    if(reader.setFilePath(filename))
//    {
//        reader.init();
//        if(reader.readFile())
//        {
//            grid = (CT_Grid3D<T>*) reader.takeFirstItemDrawableOfType(CT_Grid3D<T>::staticGetType(), result, model);
//            grid->computeMinMax();
//        }
//    }
//    return grid;
//}

void LVOX2_StepExportComputedGrids::compute()
{
    QDir().mkdir(_folder.first());

    // on récupère le modèle d'entrée à exporter
    CT_InAbstractModel* hits_mod{};
    CT_InAbstractModel* theo_mod{};
    CT_InAbstractModel* before_mod{};
    CT_InAbstractModel* density_mod{};

    // on récupère les résultats d'entrée classés par modèle d'entrée
    QList<CT_ResultGroup*> in_ress = getInputResultsForModel(DEF_in_res);
    QListIterator<CT_ResultGroup*> it(in_ress);

    QList<CT_AbstractItemDrawable*> hits_list;
    QList<CT_AbstractItemDrawable*> theo_list;
    QList<CT_AbstractItemDrawable*> before_list;
    QList<CT_AbstractItemDrawable*> density_list;

    while(it.hasNext())
    {
        CT_ResultGroup* in_res = it.next();
        hits_mod = (CT_InAbstractModel*)PS_MODELS->searchModel(DEF_hits, in_res, this);
        theo_mod = (CT_InAbstractModel*)PS_MODELS->searchModel(DEF_theo, in_res, this);
        before_mod = (CT_InAbstractModel*)PS_MODELS->searchModel(DEF_before, in_res, this);
        density_mod = (CT_InAbstractModel*)PS_MODELS->searchModel(DEF_density, in_res, this);

        // on recherche tous les ItemDrawable à exporter
        CT_ResultIterator it_hits(in_res, hits_mod);
        CT_ResultIterator it_theo(in_res, theo_mod);
        CT_ResultIterator it_before(in_res, before_mod);
        CT_ResultIterator it_density(in_res, density_mod);

        while(it_hits.hasNext())
            hits_list.append((CT_AbstractItemDrawable*)it_hits.next());
        while(it_theo.hasNext())
            theo_list.append((CT_AbstractItemDrawable*)it_theo.next());
        while(it_before.hasNext())
            before_list.append((CT_AbstractItemDrawable*)it_before.next());
        while(it_density.hasNext())
            density_list.append((CT_AbstractItemDrawable*)it_density.next());
    }

    // une fois la liste constituée
    if(!hits_list.isEmpty())
    {
        LVOX_Grid3DExporter exporter;
        exporter.init();
        exporter.setExportFilePath(_folder.first() + "/ni.GRD3D");

        // on la donne à l'exportateur
        if(!exporter.setItemDrawableToExport(hits_list))
        {
            PS_LOG->addErrorMessage(this, exporter.errorMessage());
            setErrorMessage(1, exporter.errorMessage());
            setErrorCode(1);
        }
        else
        {
            // et on exporte....
            exporter.exportToFile();
        }
    }
    if(!theo_list.isEmpty())
    {
        LVOX_Grid3DExporter exporter;
        exporter.init();
        exporter.setExportFilePath(_folder.first() + "/nt.GRD3D");

        // on la donne à l'exportateur
        if(!exporter.setItemDrawableToExport(theo_list))
        {
            PS_LOG->addErrorMessage(this, exporter.errorMessage());
            setErrorMessage(1, exporter.errorMessage());
            setErrorCode(1);
        }
        else
        {
            // et on exporte....
            exporter.exportToFile();
        }
    }
    if(!before_list.isEmpty())
    {
        LVOX_Grid3DExporter exporter;
        exporter.init();
        exporter.setExportFilePath(_folder.first() + "/nb.GRD3D");

        // on la donne à l'exportateur
        if(!exporter.setItemDrawableToExport(before_list))
        {
            PS_LOG->addErrorMessage(this, exporter.errorMessage());
            setErrorMessage(1, exporter.errorMessage());
            setErrorCode(1);
        }
        else
        {
            // et on exporte....
            exporter.exportToFile();
        }
    }
    if(!density_list.isEmpty())
    {
        LVOX_Grid3DExporter exporter;
        exporter.init();
        exporter.setExportFilePath(_folder.first() + "/density.GRD3D");

        // on la donne à l'exportateur
        if(!exporter.setItemDrawableToExport(density_list))
        {
            PS_LOG->addErrorMessage(this, exporter.errorMessage());
            setErrorMessage(1, exporter.errorMessage());
            setErrorCode(1);
        }
        else
        {
            // et on exporte....
            exporter.exportToFile();
        }
    }
}
