#include "lvox3_stepcomputepad.h"

#include "qdebug.h"
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

QString LVOX3_StepComputePAD::parametersDescription(bool defaultValue)
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
    float errorVal = -1; // a attenuation coefficient of -0.5 lead to a PAD of -1 (to track error)
    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        if (isStopped()) {return;}

        const lvox::Grid3Di* itemIn_deltaH = group->singularItem(_inDeltaHits);
        const lvox::Grid3Di* itemIn_deltaT = group->singularItem(_inDeltaTheo);
        const lvox::Grid3Di* itemIn_deltaB = group->singularItem(_inDeltaBefore);

        if (itemIn_deltaH != nullptr && itemIn_deltaT != nullptr && itemIn_deltaB != nullptr)
        {
            float xres = itemIn_deltaH->xresolution();
            float yres = itemIn_deltaH->yresolution();
            float zres = itemIn_deltaH->zresolution();
            float NAd = itemIn_deltaH->NA();

            _lambda1 = itemIn_deltaT->getLambda1();

            // qDebug()<<"x array size theo before hit"<< itemIn_deltaT->xArraySize() << itemIn_deltaB->xArraySize() << itemIn_deltaH->xArraySize();
            // qDebug()<<"y array size theo before hit"<< itemIn_deltaT->yArraySize() << itemIn_deltaB->yArraySize() << itemIn_deltaH->yArraySize();
            // qDebug()<<"z array size theo before hit"<< itemIn_deltaT->zArraySize() << itemIn_deltaB->zArraySize() << itemIn_deltaH->zArraySize();
            // qDebug()<<"min z theo before hit"<< itemIn_deltaT->minZ() << itemIn_deltaB->minZ() << itemIn_deltaH->minZ();

            size_t xdim = itemIn_deltaH->xdim();
            size_t ydim = itemIn_deltaH->ydim();
            size_t zdim = itemIn_deltaH->zdim();


            lvox::Grid3Df* itemOut_PADCF = nullptr;
            lvox::Grid3Df* itemOut_PADMLESimple = nullptr;
            lvox::Grid3Df* itemOut_PADMLEBias = nullptr;
            lvox::Grid3Df* itemOut_PADBLBasic = nullptr;
            lvox::Grid3Df* itemOut_PADBLEqual = nullptr;
            lvox::Grid3Df* itemOut_PADBLUnequal = nullptr;

            lvox::Grid3Df* itemOut_deltaSum = nullptr;
            lvox::Grid3Df* itemOut_deltaEffSum = nullptr;
            lvox::Grid3Df* itemOut_zSum = nullptr;
            lvox::Grid3Df* itemOut_zEffSum = nullptr;
            lvox::Grid3Df* itemOut_deltaSquareSum = nullptr;
            lvox::Grid3Df* itemOut_1_zleDelta_effSum = nullptr;

            if(_methodInfos["CF"].isSelected)
                itemOut_PADCF = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_methodInfos["MLE"].isSelected)
                itemOut_PADMLESimple = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_methodInfos["BCMLE"].isSelected)
                itemOut_PADMLEBias = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_methodInfos["BL"].isSelected)
                itemOut_PADBLBasic = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_methodInfos["BCBL"].isSelected)
                itemOut_PADBLEqual = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_methodInfos["BC2BL"].isSelected)
                itemOut_PADBLUnequal = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            if(_grilleSup)
            {
                itemOut_deltaSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_deltaEffSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_zSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_zEffSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_deltaSquareSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
                itemOut_1_zleDelta_effSum = new lvox::Grid3Df(itemIn_deltaH->minX(), itemIn_deltaH->minY(), itemIn_deltaH->minZ(), xdim, ydim, zdim, xres,yres,zres, NAd, NAd);
            }


            if(_methodInfos["CF"].isSelected)
                group->addSingularItem(_out_PADCF, itemOut_PADCF);
            if(_methodInfos["MLE"].isSelected)
                group->addSingularItem(_out_PADMLE, itemOut_PADMLESimple);
            if(_methodInfos["BCMLE"].isSelected)
                group->addSingularItem(_out_PADBCMLE, itemOut_PADMLEBias);
            if(_methodInfos["BL"].isSelected)
                group->addSingularItem(_out_PADBL, itemOut_PADBLBasic);
            if(_methodInfos["BCBL"].isSelected)
                group->addSingularItem(_out_PADBCBL, itemOut_PADBLEqual);
            if(_methodInfos["BC2BL"].isSelected)
                group->addSingularItem(_out_PADBC2BL, itemOut_PADBLUnequal);
            if(_grilleSup)
            {
                group->addSingularItem(_out_FreeSum, itemOut_zSum);
                group->addSingularItem(_out_FreeEffSum, itemOut_zEffSum);
                group->addSingularItem(_out_DeltaSum, itemOut_deltaSum);
                group->addSingularItem(_out_DeltaEffSum, itemOut_deltaEffSum);
                group->addSingularItem(_out_DeltaEffSquareSum, itemOut_deltaSquareSum);
                group->addSingularItem(_out_1ZleDeltaEffSum, itemOut_1_zleDelta_effSum);
            }

            /**@TODO: VTN
            * if Ni = 0 && N > 0 then pad should be 0 instead of errorVal
            **/
            for(size_t i = 0; i < itemIn_deltaH->nCells(); i++)
            {
                int Nt = itemIn_deltaT->valueAtIndex(i);
                int Ni = itemIn_deltaH->valueAtIndex(i);
                int Nb = itemIn_deltaB->valueAtIndex(i);
                int N = Nt - Nb;

                if(Ni == 0 || isOccluded(N, Nt))
                {
                    double noDataOrNoMaterialValue = errorVal;
                    if (!isOccluded(N, Nt)){
                        //qDebug()<<"occluded Ni=" << Ni << " N="<< N << " Nt=" << Nt;
                        noDataOrNoMaterialValue = 0;
                    }
                    if(_methodInfos["BCMLE"].isSelected)
                        itemOut_PADMLEBias->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_methodInfos["CF"].isSelected)
                        itemOut_PADCF->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_methodInfos["MLE"].isSelected)
                        itemOut_PADMLESimple->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_methodInfos["BCBL"].isSelected)
                        itemOut_PADBLEqual->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_methodInfos["BL"].isSelected)
                        itemOut_PADBLBasic->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_methodInfos["BC2BL"].isSelected)
                        itemOut_PADBLUnequal->setValueAtIndex(i, noDataOrNoMaterialValue);
                    if(_grilleSup)
                    {
                        itemOut_zSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                        itemOut_zEffSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                        itemOut_deltaSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                        itemOut_deltaEffSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                        itemOut_deltaSquareSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                        itemOut_1_zleDelta_effSum->setValueAtIndex(i, noDataOrNoMaterialValue);
                    }

                }
                else
                {
                    //qDebug() <<itemIn_deltaT->valueAtIndex(i)<< itemIn_deltaT->getSommaDelta(i);
                    //delta: path formed by the intersection of the ray and the voxel faces
                    //z    : path formed by the intersection of the ray and objects surface z <= delta
                    double deltaSum = itemIn_deltaT->getSommaDelta(i) - itemIn_deltaB->getSommaDelta(i);/*itemIn_deltaH->getSommaDelta(i) +*/
                    double deltaEffSum = itemIn_deltaT->getSommaEffectiveDelta(i) - itemIn_deltaB->getSommaEffectiveDelta(i);/*itemIn_deltaH->getSommaEffectiveDelta(i) +*/
                    double zSum = itemIn_deltaH->getSommaFree(i) + itemIn_deltaT->getSommaDelta(i) - itemIn_deltaB->getSommaDelta(i) - itemIn_deltaH->getSommaDelta(i);
                    double zEffSum = itemIn_deltaH->getSommaEffectiveFree(i) + itemIn_deltaT->getSommaEffectiveDelta(i) - itemIn_deltaB->getSommaEffectiveDelta(i) - itemIn_deltaH->getSommaEffectiveDelta(i);
                    double deltaSquareSum = itemIn_deltaT->getSommaDeltaSquare(i)-itemIn_deltaB->getSommaDeltaSquare(i);/*itemIn_deltaH->getSommaDeltaSquare(i)+*/
                    //qDebug() <<itemIn_deltaT->getSommaDeltaSquare(i)<<itemIn_deltaB->getSommaDeltaSquare(i);
                    double I = static_cast<double>(Ni) / static_cast<double>(N);
                    //qDebug() << I << itemIn_deltaH->valueAtIndex(i) << N;
                    double deltaMean = deltaSum / N;
                    double freeMean = zSum / N;
                    /*
                    if (zSum < 0){
                        qDebug() <<"###"<<i<< " Hit free: " << itemIn_deltaH->getSommaFree(i) << " Theory delta: " << itemIn_deltaT->getSommaDelta(i) << " Before delta: "<< -itemIn_deltaB->getSommaDelta(i) << " Hits delta: "<< -itemIn_deltaH->getSommaDelta(i);
                    }*/
                    double effectiveFreePath = zEffSum/N;
                    double effectiveDeltaPath = deltaEffSum/N;
                    double effectiveFreePathHits = itemIn_deltaH->getSommaEffectiveHits(i)/N;

                    double deltaVarEff = deltaSquareSum/N - effectiveDeltaPath*effectiveDeltaPath;
                    double alpha = deltaVarEff / effectiveDeltaPath;
                    //qDebug() << "alpha="<<_alpha<<deltaSquareSum/N<<_effectiveDeltaPath*_effectiveDeltaPath;
                    //qDebug() << "alpha="<<_alpha<<deltaSquareSum/N<<_deltaMean;
                    //qDebug() << I << deltaMean << freeMean << effectiveFreePath << effectiveDeltaPath << effectiveFreePathHits;


                    //Calculs des Eq.5.8.25.27.32
                    // FP Modified this section to make sure inconsistent statistics lead to PAD of -1 (with G=0.5)
                    double lambdaCF = I/deltaMean;
                    if(deltaMean<=0)
                        lambdaCF = errorVal*0.5;
                    //Eq. 5
                    double lambdaHat = -((log(1- I))/deltaMean);
                    if(isinf(lambdaHat)||deltaMean<=0)
                        lambdaHat = errorVal*0.5;
                    //Eq. 8
                    double lambdaTilda = I / freeMean;
                    if(freeMean <= 0)
                        lambdaTilda = errorVal*0.5;

                    //Eq. 25 : Beer Lambert
                    double capLambdaHat = errorVal*0.5;
                    if(effectiveDeltaPath > 0) {
                        if(I < 1)   {
                            capLambdaHat = (-log(1 - I) - I/(2*N*(1 - I)))/effectiveDeltaPath;
                        } else if(I >= 1) {
                            capLambdaHat = log(2*N + 2)/effectiveDeltaPath;
                        }
                    }
                    // Eq. 27
                    double capLambdaHat2 = capLambdaHat;
                    if (capLambdaHat > errorVal*0.5) {
                        if (1-2*alpha*capLambdaHat >= 0.05)   { // case of correction
                            capLambdaHat2 = 1/alpha*(1 - sqrt(1 - 2*alpha*capLambdaHat));
                        }
                        /*not need
                        else {
                            capLambdaHat2 = capLambdaHat;
                        }*/
                    }

                    //qDebug() << Ni << Nb << Nt << N << I << "effectiveFreePath: "<< effectiveFreePath;
                    //qDebug() << "_effectiveFreePath_Hits: "<< _effectiveFreePath_Hits;
                    //qDebug() << "_deltaMean: " << _deltaMean;
                    //qDebug() << "Ni:" <<Ni<<"Nb:" << Nb<<"Nt:" << Nt<<"N:" << N<<"RDI:" << I<< "Beer Lambert:"<< capLambdaHat2 << "effectiveDeltaPath:" << effectiveDeltaPath << "deltaEffSum" << deltaEffSum;
                    //qDebug() << "Theory Eff delta" << itemIn_deltaT->getSommaEffectiveDelta(i) << "Before Eff delta"<< itemIn_deltaB->getSommaEffectiveDelta(i);
                    /**
                     * Van-Tho Nguyen
                     * Refactor to avoid divison by zero or very small double number
                     **/
                    double capLambdaTilda = 0;
                    if (effectiveFreePath <= std::numeric_limits<double>::epsilon()){
                        capLambdaTilda = errorVal*0.5;
                        Eigen::Vector3d cCenter;
                        itemIn_deltaH->getCellCenterCoordinates(i, cCenter);
                        //qDebug() <<"#vid:"<<i<< "v coords:"<< cCenter.x()<< cCenter.y()<< cCenter.z()
                        //         << "Z:"<< effectiveFreePath
                        //         << "Hit eff free:" << itemIn_deltaH->getSommaEffectiveFree(i)
                        //         << "Theory eff delta:" << itemIn_deltaT->getSommaEffectiveDelta(i)
                        //         << "Before eff delta:"<< -itemIn_deltaB->getSommaEffectiveDelta(i)
                        //         << "Hits effdelta: "<< -itemIn_deltaH->getSommaEffectiveDelta(i)
                        //         << "Nt ni nb:"<< itemIn_deltaT->valueAtIndex(i) << itemIn_deltaH->valueAtIndex(i)<<itemIn_deltaB->valueAtIndex(i)
                        //         << "nt - ni -nb:" << itemIn_deltaT->valueAtIndex(i) - itemIn_deltaH->valueAtIndex(i) - itemIn_deltaB->valueAtIndex(i);
                    }else{

                        capLambdaTilda = I / effectiveFreePath - effectiveFreePathHits / ( N*effectiveFreePath * effectiveFreePath );
                        if (capLambdaTilda < 0){
                            capLambdaTilda = 0; // force to be equal to zero if bias correction lead to negative value (this is not an error here)
                        }
                    }

                    //qDebug() << capLambdaTilda << lambdaTilda << capLambdaHat << lambdaHat << capLambdaHat2;

                    // Attenuation coefficients are converted to PAD assuming a G function equal to 0.5
                    if(_methodInfos["CF"].isSelected)itemOut_PADCF->setValueAtIndex(i,lambdaCF * (1/0.5));
                    if(_methodInfos["BCMLE"].isSelected)itemOut_PADMLEBias->setValueAtIndex(i,capLambdaTilda * (1/0.5));
                    if(_methodInfos["MLE"].isSelected)itemOut_PADMLESimple->setValueAtIndex(i,lambdaTilda* (1/0.5));
                    if(_methodInfos["BCBL"].isSelected)itemOut_PADBLEqual->setValueAtIndex(i,capLambdaHat* (1/0.5));
                    if(_methodInfos["BL"].isSelected)itemOut_PADBLBasic->setValueAtIndex(i,lambdaHat* (1/0.5));
                    if(_methodInfos["BC2BL"].isSelected)itemOut_PADBLUnequal->setValueAtIndex(i,capLambdaHat2* (1/0.5));
                    if(_grilleSup)
                    {
                        itemOut_zSum->setValueAtIndex(i,zSum);
                        itemOut_zEffSum->setValueAtIndex(i,zEffSum);
                        itemOut_deltaSum->setValueAtIndex(i,deltaSum);
                        itemOut_deltaEffSum->setValueAtIndex(i,deltaEffSum);
                        itemOut_deltaSquareSum->setValueAtIndex(i,deltaSquareSum);
                        itemOut_1_zleDelta_effSum->setValueAtIndex(i,itemIn_deltaH->getSommaEffectiveHits(i));
                    }
                }
            }

            //Visualisation
            if(_methodInfos["CF"].isSelected)
                itemOut_PADCF->computeMinMax();
            if(_methodInfos["MLE"].isSelected)
                itemOut_PADMLESimple->computeMinMax();
            if(_methodInfos["BCMLE"].isSelected)
                itemOut_PADMLEBias->computeMinMax();
            if(_methodInfos["BL"].isSelected)
                itemOut_PADBLBasic->computeMinMax();
            if(_methodInfos["BCBL"].isSelected)
                itemOut_PADBLEqual->computeMinMax();
            if(_methodInfos["BC2BL"].isSelected)
                itemOut_PADBLUnequal->computeMinMax();
            if(_grilleSup)
            {
                itemOut_zSum->computeMinMax();
                itemOut_zEffSum->computeMinMax();
                itemOut_deltaSum->computeMinMax();
                itemOut_deltaEffSum->computeMinMax();
                itemOut_deltaSquareSum->computeMinMax();
                itemOut_1_zleDelta_effSum->computeMinMax();
            }

        }
    }
}

void LVOX3_StepComputePAD::computeStatistique(int i)
{
    Q_UNUSED(i)
}

//Préléminaires pour futures vérifications (Présentement non utilisé)
void LVOX3_StepComputePAD::computeVariance()
{
    //_estimatedAttenuationCoefficient = (_I/_effectiveFreePath) - (_effectiveFreePath_Hits/(_N*pow(_effectiveFreePath,2)));
    //_estimatedVariance = (_I/_effectiveFreePath_Hits)*pow(1-(_effectiveFreePath_Hits/(_N*_I*_effectiveFreePath)),2); // Equation Non-fini
}

//Préléminaires pour futures vérifications (Présentement non utilisé)
void LVOX3_StepComputePAD::computeIntervals()
{
    /*
    double confidanceIntervalTop = 0;
    double confidanceIntervalBot = 0;

    bool ok = false;
    double gaussianDistribution = 1 - (_confidenceLevel.toDouble(&ok))/2;

    if(_estimatedAttenuationCoefficient*_deltaMean <= 0.5)
    {
        double IC = (_I + pow(gaussianDistribution,2)/(2*_N)) / (1+pow(gaussianDistribution,2)/(_N));
        double numShotsC = _N + pow(gaussianDistribution,2);
        double attenuationCoefficientC = (IC/_effectiveFreePath) - (_effectiveFreePath_Hits/(numShotsC*pow(_effectiveFreePath,2)));
        double estimatedVarianceC = (IC/_effectiveFreePath_Hits)*pow(1-(_effectiveFreePath_Hits/(numShotsC*IC*_effectiveFreePath)),2); // Equation Non-fini

        confidanceIntervalBot = attenuationCoefficientC -(gaussianDistribution * sqrt(estimatedVarianceC));
        confidanceIntervalTop = attenuationCoefficientC +(gaussianDistribution * sqrt(estimatedVarianceC));
    }
    else
    {
        confidanceIntervalBot = _estimatedAttenuationCoefficient -(gaussianDistribution * sqrt(_estimatedVariance));
        confidanceIntervalTop = _estimatedAttenuationCoefficient +(gaussianDistribution * sqrt(_estimatedVariance));
    }
    */
}

//Préléminaires pour futures vérifications (Présentement non utilisé)
void LVOX3_StepComputePAD::computeBias()
{
    /*
    double L1 = _lambda1 * deltaMean;

    //Table 2

    //Bias <1%
    bool Bias1 = true;
    //1
    if(L1 <= 0.01 && _numShots >=100){}else{Bias1 = false;}
    //2
    if(L1 <= 0.01 && _N >= 3){}else{Bias1 = false;}
    if(L1 <= 0.1 && _N >= 5){}else{Bias1 = false;}
    if(L1 <= 0.2 && _N >= 15){}else{Bias1 = false;}
    if(L1 <= 0.3 && _N >= 30){}else{Bias1 = false;}
    //3
    if(L1 <= 2 && L1<=0.01 && _numShots >= 100){}else{Bias1 = false;}
    //4
    if(L1 <= 0.5 && L1 <=0.2 && _numShots >= 7){}else{Bias1 = false;}
    if(L1 <= 1 && L1 <= 0.2 && _numShots >= 10){}else{Bias1 = false;}
    if(L1 <= 1.5 && L1 <=0.2 && _numShots >= 15){}else{Bias1 = false;}
    if(L1 <= 2 && L1 <= 0.05 && _numShots >= 40){}else{Bias1 = false;}
    if(L1 <= 2.5 && L1 <= 0.005 && _numShots >= 75){}else{Bias1 = false;}
    if(L1 <= 3 && L1 <= 0.001 && _numShots >= 75){}else{Bias1 = false;}

    //Bias <5%
    bool Bias5 = true;
    //1
    if(L1 <= 0.01 && _numShots >=20){}else{Bias5 = false;}
    if(L1 <= 0.05 && _numShots >=30){}else{Bias5 = false;}
    //2
    if(L1 <= 0.05 && _N >= 3){}else{Bias5 = false;}
    if(L1 <= 0.1 && _N >= 5){}else{Bias5 = false;}
    if(L1 <= 0.3 && _N >= 10){}else{Bias5 = false;}
    //3
    if(L1<=0.05 && _numShots >= 40){}else{Bias5 = false;}
    //4
    if(L1 <= 1 && L1 <= 0.1 && _numShots >= 5){}else{Bias5 = false;}
    if(L1 <= 1.5 && L1 <=0.2 && _numShots >= 10){}else{Bias5 = false;}
    if(L1 <= 2 && L1 <= 0.2 && _numShots >= 15){}else{Bias5 = false;}
    if(L1 <= 2.5 && L1 <= 0.2 && _numShots >= 40){}else{Bias5 = false;}
    if(L1 <= 3 && L1 <= 0.1 && _numShots >= 75){}else{Bias5 = false;}

    //Bias <10%
    bool Bias10 = true;
    //1
    if(L1 <= 0.01 && _numShots >=10){}else{Bias10 = false;}
    if(L1 <= 0.1 && _numShots >=20){}else{Bias10 = false;}
    //2
    if(L1 <= 0.1 && _N >= 3){}else{Bias10 = false;}
    if(L1 <= 0.2 && _N >= 5){}else{Bias10 = false;}
    if(L1 <= 0.3 && _N >= 7){}else{Bias10 = false;}
    if(L1 <= 0.5 && _N >= 10){}else{Bias10 = false;}
    //3
    if(L1 <= 2 && L1<=0.01 && _numShots >= 10){}else{Bias10 = false;}
    if(L1 <= 2.5 && L1<=0.1 && _numShots >= 20){}else{Bias10 = false;}
    if(L1 <= 3 && L1<=0.1 && _numShots >= 30){}else{Bias10 = false;}
    //4
    if(L1 <= 1.5 && L1 <=0.2 && _numShots >= 5){}else{Bias10 = false;}
    if(L1 <= 2 && L1 <= 0.3 && _numShots >= 10){}else{Bias10 = false;}
    if(L1 <= 2.5 && L1 <=0.3 && _numShots >= 20){}else{Bias10 = false;}
    if(L1 <= 3 && L1 <= 0.3 && _numShots >= 40){}else{Bias10 = false;}
    */
}

bool LVOX3_StepComputePAD::isOccluded(int enteredShots, int thereticalShots){
    if(_isRelativeThreshold){
        return enteredShots*100.0/thereticalShots <= _NThreshold;
    }else{
        return enteredShots <= _NThreshold;
    }
}
