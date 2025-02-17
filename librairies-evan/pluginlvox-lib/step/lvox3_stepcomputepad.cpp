#include "lvox3_stepcomputepad.h"

#include "datastructures/lvox3_implgrid3d.h"
#include "qdebug.h"
#include "tools/lvox3_logger.h"
#include "view/computepadconfiguration.h"
#include "ct_view/tools/ct_configurablewidgettodialog.h"

#include <math.h>

// Constructor : initialization of parameters
LVOX3_StepComputePAD::LVOX3_StepComputePAD() : CT_AbstractStep()
{
    _methodInfos.insert("CF", {"CF", "Contact Frequency", "", false});
    _methodInfos.insert("MLE", {"MLE", "Maximum Likelyhood Estimator", "", false});
    _methodInfos.insert("BCMLE", {"BCMLE", "Bias-Corrected Maximum Likelyhood Estimator", "", false});
    _methodInfos.insert("BL", {"BL", "Beer Lambert", "", false});
    _methodInfos.insert("BCBL", {"BCBL", "Bias-Corrected Beer-Lambert (Equal path)", "", false});
    _methodInfos.insert("BC2BL", {"BC2BL", "Bias-Corrected Beer-Lambert (Unequal path)", "", false});

    _EraseInfinity = false;
    _PADlimit = 100; // 100 m2/voxel
    _NThreshold = 5;
    _isRelativeThreshold = false;
    _grilleSup = false;
}

// Displayed step name
QString LVOX3_StepComputePAD::description() const
{
    return tr("3 - Estimation of the Plant Area Density");
}

// Step documentation
QString LVOX3_StepComputePAD::detailledDescription() const
{
    return QString() +
            tr("Step allowing the computation of the Plant Area Density (PAD) for a grid set.") + "<br>" +
            tr("The field  \"Attenuation coefficient estimators method\" allows choosing between multiple computation methods :") + "<br>"
                "<ul>"
                    "<li>" + tr("Contact Frequency") + " : " + "CF" + "</li>"
                    "<li>" + tr("Beer-Lambert") + " : BL</li>"
                    "<li>" + tr("Maximum Likelyhood Estimator") + " : MLE</li>"
                    "<li>" + tr("Bias-Corrected Maximum Likelyhood Estimator") + " : BCMLE</li>"
                    "<li>" + tr("Bias-Corrected Beer-Lambert (Equal path)") + " : BCBL</li>"
                    "<li>" + tr("Bias-Corrected Beer-Lambert (Unequal path)") + " : BC2BL</li>"
                "</ul>"
            "<em>" + tr("Note that Beer-Lambert as well as MLE are obsolete in term of precision but are left to allow baseline comparisons") + "</em><br><br>" +
            tr("Users can also set a threshold for the actual hits (N = Nt - Nb) in a voxel (ie. if a ") +

            tr("For more informations regarding the different methods used to compute the PAD, refer to the provided article.");

}

// Step copy method
CT_VirtualAbstractStep* LVOX3_StepComputePAD::createNewInstance() const
{
    return new LVOX3_StepComputePAD();
}

void LVOX3_StepComputePAD::savePostSettings(SettingsWriterInterface& writer) const
{
    CT_AbstractStep::savePostSettings(writer);

    writer.addParameter(this, "NThreshold", _NThreshold);
    writer.addParameter(this, "isRelativeThreshold", _isRelativeThreshold);
    writer.addParameter(this, "grilleSup", _grilleSup);

    for (const auto& e : _methodInfos)
    {
        writer.addParameter(this, e.shortName, e.isSelected);
    }

}

bool LVOX3_StepComputePAD::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    if(!reader.parameter(this,"NThreshold", value))
        return false;
    _NThreshold = value.toInt();

    if(!reader.parameter(this,"isRelativeThreshold", value))
        return false;
    _isRelativeThreshold = value.toBool();

    if(!reader.parameter(this,"grilleSup", value))
        return false;
    _isRelativeThreshold = value.toBool();

    for (const auto& e : qAsConst(_methodInfos))
    {
        reader.parameter(this, e.shortName, value);
        _methodInfos[e.shortName].isSelected = value.toBool();
        qDebug() << "Loading " << e.shortName << " with value " << value.toBool();
    }

    return true;
}

QString LVOX3_StepComputePAD::parametersDescription()
{
    return CT_AbstractStep::parametersDescription() +
            "<strong>" + tr("Step parameters:") + "</strong>"
            "<ul>"
                "<li>" + tr("Attenuation coefficient estimators method") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Contact Frequency") + " : " + "CF" + "</li>"
                        "<li>" + tr("Beer-Lambert") + " : BL</li>"
                        "<li>" + tr("Maximum Likelyhood Estimator") + " : MLE</li>"
                        "<li>" + tr("Bias-Corrected Maximum Likelyhood Estimator") + " : BCMLE</li>"
                        "<li>" + tr("Bias-Corrected Beer-Lambert (Equal path)") + " : BCBL</li>"
                        "<li>" + tr("Bias-Corrected Beer-Lambert (Unequal path)") + " : BC2BL</li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Compute estimator for N larger than") + ": " + tr("integer") + " <em>" + tr("default") + "</em> = 5</li>" +
                "<li>" + tr("Use relative threshold") + ": " + tr("boolean") + "<em>" + tr("default") + "</em> = " + tr("false") + "</li>" +
                "<li>" + tr("Create grids corresponding to summations") + ": " + tr("boolean") + "<em>" + tr("default") + "</em> = " + tr("false") + "</li>" +
            "</ul>";
}

QString LVOX3_StepComputePAD::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Group [Group]:" +
            indent("...") +
            indent("One or many of the follwing, given the <em>\""+ tr("Attenuation coefficient estimators method") +"\"</em> selected :")+
            indent(indent("LVOX3_Grid3D<float> [PAD-ContactFreq]")) +
            indent(indent("LVOX3_Grid3D<float> [PAD-MLE]")) +
            indent(indent("LVOX3_Grid3D<float> [PAD-BiasCorMLE]")) +
            indent(indent("LVOX3_Grid3D<float> [PAD-BeerLamb]")) +
            indent(indent("LVOX3_Grid3D<float> [PAD-BiasCorBeerLamb]")) +
            indent(indent("LVOX3_Grid3D<float> [PAD-UnequalPathBeerLamb]")) +
            indent("If <em>\""+ tr("Create grids corresponding to summations") +"\"</em> is checked, all of the following will be produced :") +
            indent(indent("LVOX3_Grid3D<float> [FreePath Sum]")) +
            indent(indent("LVOX3_Grid3D<float> [Effective FreePath Sum]")) +
            indent(indent("LVOX3_Grid3D<float> [Delta Sum]")) +
            indent(indent("LVOX3_Grid3D<float> [Effective Delta Sum]")) +
            indent(indent("LVOX3_Grid3D<float> [Effective Delta Square Sum]")) +
            indent(indent("LVOX3_Grid3D<float> [1_zleDelta Effective Sum]"));
}


QStringList LVOX3_StepComputePAD::getStepRISCitations() const
{
    return QStringList{
        "TY  - JOUR\n"
        "T1  - Estimation of vertical plant area density from single return terrestrial laser scanning point clouds acquired in forest environments\n"
        "AU  - Nguyen, Van-Tho\n"
        "AU  - Fournier, Richard A.\n"
        "AU  - Côté, Jean-François\n"
        "AU  - Pimont, François\n"
        "JO  - Remote Sensing of Environment\n"
        "VL  - 279\n"
        "SP  - 113115\n"
        "PY  - 2022\n"
        "DA  - 2022/09/15\n"
        "SN  - 0034-4257\n"
        "DO  - https://doi.org/10.1016/j.rse.2022.113115\n"
        "UR  - https://www.sciencedirect.com/science/article/pii/S0034425722002292\n"


    };
}


//////////////////// PROTECTED METHODS //////////////////

// Creation and affiliation of IN models
void LVOX3_StepComputePAD::declareInputModels(CT_StepInModelStructureManager &manager)
{
    manager.addResult(_inResult, "SGrids");
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inDeltaHits, "Hits");
    manager.addItem(_inGroup, _inDeltaTheo, "Theoretical");
    manager.addItem(_inGroup, _inDeltaBefore, "Before");
    manager.addItem(_inGroup, _inHitRef, "RefHits");

}

// Creation and affiliation of OUT models
void LVOX3_StepComputePAD::declareOutputModels(CT_StepOutModelStructureManager &manager)
{    
    manager.addResultCopy(_inResult);
    if (_methodInfos["CF"].isSelected)
        manager.addItem(_inGroup, _out_PADCF, "PAD-ContactFreq");
    if (_methodInfos["MLE"].isSelected)
        manager.addItem(_inGroup, _out_PADMLE, "PAD-MLE");
    if (_methodInfos["BCMLE"].isSelected)
        manager.addItem(_inGroup, _out_PADBCMLE, "PAD-BiasCorMLE");
    if (_methodInfos["BL"].isSelected)
        manager.addItem(_inGroup, _out_PADBL, "PAD-BeerLamb");
    if (_methodInfos["BCBL"].isSelected)
        manager.addItem(_inGroup, _out_PADBCBL, "PAD-BiasCorBeerLamb");
    if (_methodInfos["BC2BL"].isSelected)
        manager.addItem(_inGroup, _out_PADBC2BL, "PAD-UnequalPathBeerLamb");

    if (_grilleSup)
    {
        manager.addItem(_inGroup, _out_FreeSum, "FreePath Sum");
        manager.addItem(_inGroup, _out_FreeEffSum, "Effective FreePath Sum");
        manager.addItem(_inGroup, _out_DeltaSum, "Delta Sum");
        manager.addItem(_inGroup, _out_DeltaEffSum, "Effective Delta Sum");
        manager.addItem(_inGroup, _out_DeltaEffSquareSum, "Effective Delta Square Sum");
        manager.addItem(_inGroup, _out_1ZleDeltaEffSum, "1_zleDelta Effective Sum");
    }
}


bool LVOX3_StepComputePAD::postInputConfigure()
{
    ComputePadConfiguration c;
    for (const auto& e : qAsConst(_methodInfos))
        c.setPADComputationMethod(e.shortName, e.longName, e.isSelected);

    c.setNThreshold(_NThreshold);
    c.setUseRelativeThreshold(_isRelativeThreshold);
    c.setCreateSummationGrids(_grilleSup);

    if(CT_ConfigurableWidgetToDialog::exec(&c) == QDialog::Rejected)
        return false;

    _isRelativeThreshold = c.isRelativeThreshold();
    _NThreshold = c.getThreshold();
    _grilleSup = c.createSummationsGrids();
    _methodInfos["CF"].isSelected = c.isMethodSelected("CF");
    _methodInfos["MLE"].isSelected = c.isMethodSelected("MLE");
    _methodInfos["BCMLE"].isSelected = c.isMethodSelected("BCMLE");
    _methodInfos["BL"].isSelected = c.isMethodSelected("BL");
    _methodInfos["BCBL"].isSelected = c.isMethodSelected("BCBL");
    _methodInfos["BC2BL"].isSelected = c.isMethodSelected("BC2BL");

    return true;
}

void LVOX3_StepComputePAD::compute()
{
    std::stringstream fmt;
    lvox::ComputePAD::Settings settings;
    settings.ErrorValue = -1;
    settings.NThreshold = _NThreshold;
    settings.IsRelativeThreshold = _isRelativeThreshold;

    qDebug() << "Started Compute PAD";

//    // Extract grid settings, should be the same for every grids
//    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
//    {
//        const pluginlvox::Grid3Di* g = group->singularItem(_inDeltaHits);
//        const pluginlvox::Grid3Di* h = group->singularItem(_inDeltaBefore);
//        const pluginlvox::Grid3Di* i = group->singularItem(_inDeltaTheo);
//        const CT_Grid3D<int>* j = group->singularItem(_inHitRef);


//        fmt << g->valueAtIndex(3000) << " "
//            << h->valueAtIndex(3000) << " "
//            << i->valueAtIndex(3000) << " "
//            << j->valueAtIndex(3000);
//        qDebug().noquote() << QString::fromStdString(fmt.str());
//        fmt.clear();
//        break;
//    }


    std::vector<std::pair<CT_StandardItemGroup*, lvox::ComputePAD::Input>> inputs;
    std::vector<LVOX3_AbstractGrid3D*> minMaxs;

    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        lvox::ComputePAD::Input input;
        const pluginlvox::Grid3Di* itemIn_deltaH = group->singularItem(_inDeltaHits);
        const pluginlvox::Grid3Di* itemIn_deltaT = group->singularItem(_inDeltaTheo);
        const pluginlvox::Grid3Di* itemIn_deltaB = group->singularItem(_inDeltaBefore);
        const CT_Grid3D<int>* j = group->singularItem(_inHitRef);

        LVOX3_ImplGrid3d<int>* hits = new LVOX3_ImplGrid3d<int>{ const_cast<pluginlvox::Grid3Di*>(itemIn_deltaH) };
        LVOX3_ImplGrid3d<int>* befo = new LVOX3_ImplGrid3d<int>{ const_cast<pluginlvox::Grid3Di*>(itemIn_deltaB) };
        LVOX3_ImplGrid3d<int>* theo = new LVOX3_ImplGrid3d<int>{ const_cast<pluginlvox::Grid3Di*>(itemIn_deltaT) };

        settings.Lambda1 = itemIn_deltaH->getLambda1();
        settings.GridNAValue = itemIn_deltaH->NA();
        settings.GridResolution = itemIn_deltaH->xresolution();

        input.TheoreticalGrid = theo;
        input.HitGrid = hits;
        input.BeforeGrid = befo;

        if (itemIn_deltaH != nullptr && itemIn_deltaT != nullptr && itemIn_deltaB != nullptr)
        {
            float xres = itemIn_deltaH->xresolution();
            float yres = itemIn_deltaH->yresolution();
            float zres = itemIn_deltaH->zresolution();

            size_t xdim = itemIn_deltaH->xdim();
            size_t ydim = itemIn_deltaH->ydim();
            size_t zdim = itemIn_deltaH->zdim();

            float NAd = itemIn_deltaH->NA();

            _lambda1 = itemIn_deltaT->getLambda1();

            fmt << xdim << " " << ydim << " " << zdim << " " << NAd << " " << _lambda1 << " "
                << itemIn_deltaH->valueAtIndex(3000) << " " << hits->getValueAtIndex(3000) << " "
                << itemIn_deltaT->valueAtIndex(3000) << " " << befo->getValueAtIndex(3000) << " "
                << itemIn_deltaB->valueAtIndex(3000) << " " << theo->getValueAtIndex(3000) << " "
                << j->valueAtIndex(3000) << " ";

            qDebug().noquote() << QString::fromStdString(fmt.str());
            // qDebug()<<"x array size theo before hit"<< itemIn_deltaT->xArraySize() << itemIn_deltaB->xArraySize() << itemIn_deltaH->xArraySize();
            // qDebug()<<"y array size theo before hit"<< itemIn_deltaT->yArraySize() << itemIn_deltaB->yArraySize() << itemIn_deltaH->yArraySize();
            // qDebug()<<"z array size theo before hit"<< itemIn_deltaT->zArraySize() << itemIn_deltaB->zArraySize() << itemIn_deltaH->zArraySize();
            // qDebug()<<"min z theo before hit"<< itemIn_deltaT->minZ() << itemIn_deltaB->minZ() << itemIn_deltaH->minZ();




            pluginlvox::Grid3Df* itemOut_PADCF = nullptr;
            pluginlvox::Grid3Df* itemOut_PADMLESimple = nullptr;
            pluginlvox::Grid3Df* itemOut_PADMLEBias = nullptr;
            pluginlvox::Grid3Df* itemOut_PADBLBasic = nullptr;
            pluginlvox::Grid3Df* itemOut_PADBLEqual = nullptr;
            pluginlvox::Grid3Df* itemOut_PADBLUnequal = nullptr;

            pluginlvox::Grid3Df* itemOut_deltaSum = nullptr;
            pluginlvox::Grid3Df* itemOut_deltaEffSum = nullptr;
            pluginlvox::Grid3Df* itemOut_zSum = nullptr;
            pluginlvox::Grid3Df* itemOut_zEffSum = nullptr;
            pluginlvox::Grid3Df* itemOut_deltaSquareSum = nullptr;
            pluginlvox::Grid3Df* itemOut_1_zleDelta_effSum = nullptr;
            // TODO : Implement the UI side : pluginlvox::Grid3Df* itemOut_1_zleDelta_idr = nullptr;


            if(_methodInfos["CF"].isSelected)
            {
                itemOut_PADCF = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADCF);
                LVOX3_ImplGrid3d<float>* padcf = new LVOX3_ImplGrid3d<float>(itemOut_PADCF);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::ContactFrequency, padcf));
                group->addSingularItem(_out_PADCF, itemOut_PADCF);
            }
            if(_methodInfos["MLE"].isSelected)
            {
                itemOut_PADMLESimple = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADMLESimple);
                LVOX3_ImplGrid3d<float>* padmle = new LVOX3_ImplGrid3d<float>(itemOut_PADMLESimple);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::MaximumLikelyhoodEstimator, padmle));
                group->addSingularItem(_out_PADMLE, itemOut_PADMLESimple);
            }
            if(_methodInfos["BCMLE"].isSelected)
            {
                itemOut_PADMLEBias = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADMLEBias);

                LVOX3_ImplGrid3d<float>* padbcmle = new LVOX3_ImplGrid3d<float>(itemOut_PADMLEBias);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::BiasCorrectedMaximumLikelyhoodEstimator, padbcmle));

                group->addSingularItem(_out_PADBCMLE, itemOut_PADMLEBias);

            }
            if(_methodInfos["BL"].isSelected)
            {
                qDebug() << "In BeerLamber grid creation";
                itemOut_PADBLBasic = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADBLBasic);

                LVOX3_ImplGrid3d<float>* padbl = new LVOX3_ImplGrid3d<float>(itemOut_PADBLBasic);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::BeerLamber, padbl));

                group->addSingularItem(_out_PADBL, itemOut_PADBLBasic);
            }
            if(_methodInfos["BCBL"].isSelected)
            {
                itemOut_PADBLEqual = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADBLBasic);

                LVOX3_ImplGrid3d<float>* padbcbl = new LVOX3_ImplGrid3d<float>(itemOut_PADBLEqual);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::EqualBiasCorrectedBeerLamber, padbcbl));

                group->addSingularItem(_out_PADBCBL, itemOut_PADBLEqual);
            }
            if(_methodInfos["BC2BL"].isSelected)
            {
                itemOut_PADBLUnequal = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                minMaxs.push_back(itemOut_PADBLUnequal);

                LVOX3_ImplGrid3d<float>* padbc2bl = new LVOX3_ImplGrid3d<float>(itemOut_PADBLEqual);
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::UnequalBiasCorrectedBeerLamber, padbc2bl));

                group->addSingularItem(_out_PADBC2BL, itemOut_PADBLUnequal);
            }
            if(_grilleSup)
            {
                itemOut_deltaSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_deltaEffSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_zSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_zEffSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_deltaSquareSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_1_zleDelta_effSum = new pluginlvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);

                minMaxs.push_back(itemOut_deltaSum);
                minMaxs.push_back(itemOut_deltaEffSum);
                minMaxs.push_back(itemOut_zSum);
                minMaxs.push_back(itemOut_zEffSum);
                minMaxs.push_back(itemOut_deltaSquareSum);
                minMaxs.push_back(itemOut_1_zleDelta_effSum);

                LVOX3_ImplGrid3d<float>* pl_itemOut_deltaSum =  new LVOX3_ImplGrid3d<float>(itemOut_deltaSum);
                LVOX3_ImplGrid3d<float>* pl_itemOut_deltaEffSum =  new LVOX3_ImplGrid3d<float>(itemOut_deltaEffSum);
                LVOX3_ImplGrid3d<float>* pl_itemOut_zSum =   new LVOX3_ImplGrid3d<float>(itemOut_zSum);
                LVOX3_ImplGrid3d<float>* pl_itemOut_zEffSum =  new LVOX3_ImplGrid3d<float>(itemOut_zEffSum);
                LVOX3_ImplGrid3d<float>* pl_itemOut_deltaSquareSum =  new LVOX3_ImplGrid3d<float>(itemOut_deltaSquareSum);
                LVOX3_ImplGrid3d<float>* pl_itemOut_1_zleDelta_effSum =  new LVOX3_ImplGrid3d<float>(itemOut_1_zleDelta_effSum);

                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::deltaSum, pl_itemOut_deltaSum));
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::deltaEffSum, pl_itemOut_deltaEffSum));
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::zSum, pl_itemOut_zSum));
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::zEffSum, pl_itemOut_zEffSum));
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::deltaSquareSum, pl_itemOut_deltaSquareSum));
                input.OutputGrids.push_back(std::pair(lvox::ComputePAD::GridsToCompute::zleDelta_effSum, pl_itemOut_1_zleDelta_effSum));

                group->addSingularItem(_out_FreeSum, itemOut_zSum);
                group->addSingularItem(_out_FreeEffSum, itemOut_zEffSum);
                group->addSingularItem(_out_DeltaSum, itemOut_deltaSum);
                group->addSingularItem(_out_DeltaEffSum, itemOut_deltaEffSum);
                group->addSingularItem(_out_DeltaEffSquareSum, itemOut_deltaSquareSum);
                group->addSingularItem(_out_1ZleDeltaEffSum, itemOut_1_zleDelta_effSum);
            }

            inputs.push_back(std::pair(group, input));
        }
    }

    std::vector<lvox::ComputePAD::Input> ins;

    std::transform(inputs.begin(), inputs.end(), std::back_inserter(ins),
                      [](const std::pair<CT_StandardItemGroup*, lvox::ComputePAD::Input>& pair) { return pair.second; });
    LVOX3_Logger logger;
    lvox::ComputePAD algo(ins, settings);
    algo.compute();

    //Visualisation
    for(auto& e : minMaxs)
        e->computeMinMax();

}
