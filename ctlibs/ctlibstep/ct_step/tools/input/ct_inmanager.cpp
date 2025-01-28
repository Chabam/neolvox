#include "ct_inmanager.h"

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"
#include "ct_model/outModel/abstract/ct_outabstractmodel.h"
#include "ct_result/abstract/ct_abstractresult.h"
#include "ct_step/abstract/ct_virtualabstractstep.h"

#include "ct_model/inModel/views/ctg_inmodelpossibilitieschoice.h"
#include "ctlibmodelsextraviews/ctg_modelslinkconfigurationflowview.h"

//#define InModelPossibilitiesChoiceWidgetType CTG_InModelPossibilitiesChoice
#define InModelPossibilitiesChoiceWidgetType CTG_ModelsLinkConfigurationFlowView

CT_InManager::CT_InManager()
{
    m_inModelConfigurationManager = new CT_InResultModelConfigurationManager(m_inModelsStructureManager);
}

CT_InManager::~CT_InManager()
{
    delete m_inModelConfigurationManager;
}

CT_InModelStructureManager* CT_InManager::inModelStructureManager() const
{
    return &const_cast<CT_InModelStructureManager&>(m_inModelsStructureManager);
}

bool CT_InManager::createInputModels(CT_VirtualAbstractStep& step)
{
    if(step.hasChildrens())
        return false;

    m_stepName = step.description();

    // clear all previously result models added
    m_inModelsStructureManager.clearResults();

    // we ask the step to produce input models, so it will be added to the input models manager
    step.declareInputModels(m_inModelsStructureManager);

    if(m_inModelsStructureManager.nResults() == 0)
        m_inModelsStructureManager.setNotNeedInputResult();

    // we set the step to all models created
    m_inModelsStructureManager.visitResults([&step](const CT_InAbstractResultModel* inResultModel) -> bool {
        const_cast<CT_InAbstractResultModel*>(inResultModel)->setStep(&step);

        return inResultModel->recursiveVisitInChildrensOrInModelOfPossibilities([&step](const CT_InAbstractModel* inModel) -> bool {
            const_cast<CT_InAbstractModel*>(inModel)->setStep(&step);
            return true;
        });
    });

    return true;
}

bool CT_InManager::needInputs() const
{
    return m_inModelsStructureManager.needInputs();
}

bool CT_InManager::wereAllInputsFoundInOutputsOfThisStepOrRecursively(const CT_VirtualAbstractStep* step) const
{
    QVector<CT_InAbstractResultModel*> resModels(m_inModelsStructureManager.nResults());
    int currentIndex = 0;

    m_inModelsStructureManager.visitResults([&resModels, &currentIndex](const CT_InAbstractResultModel* inResultModel) -> bool {
        resModels[currentIndex++] = static_cast<CT_InAbstractResultModel*>(inResultModel->copy(false));
        return true;
    });

    const bool result = findInputsInOutputsOfThisStepOrRecursivelyWithSpecifiedResultModels(step, resModels.begin(), resModels.end());

    qDeleteAll(resModels);

    return result;
}

bool CT_InManager::findInputsInOutputsOfThisStepOrRecursively(const CT_VirtualAbstractStep* step)
{
    QVector<CT_InAbstractResultModel*> resModels(m_inModelsStructureManager.nResults());
    int currentIndex = 0;

    m_inModelsStructureManager.visitResults([&resModels, &currentIndex](const CT_InAbstractResultModel* inResultModel) -> bool {
        resModels[currentIndex++] = const_cast<CT_InAbstractResultModel*>(inResultModel);
        return true;
    });

    return findInputsInOutputsOfThisStepOrRecursivelyWithSpecifiedResultModels(step, resModels.begin(), resModels.end());
}

CT_InManager::ConfigureReturn CT_InManager::configureInputs(const CT_VirtualAbstractStep* step, bool forceReadOnly)
{
    const QString extraTitle = QObject::tr(" de l'étape \"%1\"").arg(m_stepName);
    const CT_InResultModelConfigurationManager::CreateDialogReturn crReturn = m_inModelConfigurationManager->createInResultModelConfigurationDialog<InModelPossibilitiesChoiceWidgetType>(extraTitle, step->getHelpPageForStepPath());

    // If it was an error (does not happen)
    if(crReturn == CT_InResultModelConfigurationManager::CreateError) {
        qFatal("Erreur lors de la creation de la fenetre de configuration des resultats d'entree");
        return ConfigureReturn::Error;
    }

    if(forceReadOnly) {
        m_inModelConfigurationManager->showReadOnlyInResultModel();
        return ConfigureReturn::NoModification;
    }

    CT_InResultModelConfigurationManager::ConfigureReturn cReturn = m_inModelConfigurationManager->configureInResultModel();

    if(cReturn == CT_InResultModelConfigurationManager::ConfigureSuccess)    // If modification has been made by the user (user click the "Ok" button)
        return ConfigureReturn::HasModification;

    if(cReturn == CT_InResultModelConfigurationManager::ConfigureError) {    // If it was an error (does not happen)
        qFatal("Erreur lors de la définition des modèles d'entrées");
        return ConfigureReturn::Error;
    }

    return ConfigureReturn::NoModification;
}

bool CT_InManager::exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles)
{
    return m_inModelConfigurationManager->exportViewForINModelConfig(exportPath, exportedFiles);
}

bool CT_InManager::hasAllModelsTheMinimumNumberOfSelectedPossibilityRequired() const
{
    return m_inModelsStructureManager.visitResults([](const CT_InAbstractResultModel* model) -> bool {
        return model->hasTheMinimumNumberOfSelectedPossibilityRequired();
    });
}

bool CT_InManager::visitResultModels(const CT_InManager::InResultModelVisitor& visitor) const
{
    return m_inModelsStructureManager.visitResults(visitor);
}

int CT_InManager::nResultModel() const
{
    return m_inModelsStructureManager.nResults();
}

int CT_InManager::nResults() const
{
    int n = 0;

    m_inModelsStructureManager.visitResults([&n](const CT_InAbstractResultModel* inResultModel) -> bool {
        return inResultModel->possibilitiesGroup()->visitSelectedPossibilities([&n](const CT_InStdModelPossibility* p) -> bool {
            const CT_OutAbstractModel* outModel = p->outModel();

            Q_ASSERT(outModel != nullptr);
            if (outModel == nullptr) {qDebug() << "CT_InManager::nResults" << ", " <<  "outModel == nullptr";}

            Q_ASSERT(dynamic_cast<CT_AbstractResult*>(outModel->result()) != nullptr);
            if (dynamic_cast<CT_AbstractResult*>(outModel->result()) == nullptr) {qDebug() << "CT_InManager::nResults" << ", " <<  "dynamic_cast<CT_AbstractResult*>(outModel->result()) == nullptr";}

            ++n;

            return true;
        });
    });

    return n;
}

void CT_InManager::setResultsBusy(bool busy)
{
    m_inModelsStructureManager.visitResults([&busy](const CT_InAbstractResultModel* inResultModel) -> bool {
        return inResultModel->possibilitiesGroup()->visitSelectedPossibilities([&busy](const CT_InStdModelPossibility* p) -> bool {
            const CT_OutAbstractModel* outModel = p->outModel();

            Q_ASSERT(outModel != nullptr);
            if (outModel == nullptr) {qDebug() << "CT_InManager::setResultsBusy" << ", " <<  "outModel == nullptr";}

            Q_ASSERT(dynamic_cast<CT_AbstractResult*>(outModel->result()) != nullptr);
            if (dynamic_cast<CT_AbstractResult*>(outModel->result()) == nullptr) {qDebug() << "CT_InManager::setResultsBusy" << ", " <<  "dynamic_cast<CT_AbstractResult*>(outModel->result()) == nullptr";}

            static_cast<CT_AbstractResult*>(outModel->result())->setBusy(busy);

            return true;
        });
    });
}

void CT_InManager::aboutToBeDeleted()
{
    m_inModelConfigurationManager->aboutToBeDeleted();
}

void CT_InManager::saveSettings(SettingsWriterInterface& writer) const
{
    m_inModelConfigurationManager->saveSettings(writer);
}

bool CT_InManager::restoreSettings(SettingsReaderInterface& reader, QUrl pathHelp)
{
    return m_inModelConfigurationManager->restoreSettings<InModelPossibilitiesChoiceWidgetType>(reader, pathHelp);
}

bool CT_InManager::findInputsInOutputsOfThisStepOrRecursivelyWithSpecifiedResultModels(const CT_VirtualAbstractStep* step,
                                                                                      QVector<CT_InAbstractResultModel*>::iterator begin,
                                                                                      QVector<CT_InAbstractResultModel*>::iterator end) const
{
    if(step == nullptr)
        return !needInputs();

    if(!needInputs())
        return true;

    const auto nResults = std::distance(begin, end);

    CT_VirtualAbstractStep* parentStep = const_cast<CT_VirtualAbstractStep*>(step);
    bool isCurrentlyInRecursivity = false;
    bool atLeastOneModelIsRecursive = true;

    QSet<CT_InAbstractResultModel*> modelsThatHaveFound;

    while((parentStep != nullptr) && atLeastOneModelIsRecursive) {
        CT_OutModelStructureManager* omsm = parentStep->outManager()->outModelStructureManager();

        atLeastOneModelIsRecursive = false;

        QVector<CT_InAbstractResultModel*>::iterator it = begin;

        while(it != end) {
            CT_InAbstractResultModel* inResultModel = *it;

            const bool firstLoop = !isCurrentlyInRecursivity;
            const bool isModelRecursive = inResultModel->isRecursive();

            if(firstLoop)
                inResultModel->recursiveClearPossibilitiesSaved();

            if(isModelRecursive)
                atLeastOneModelIsRecursive = true;

            if(firstLoop || isModelRecursive) {

                omsm->visitResults([&modelsThatHaveFound, &inResultModel](const CT_OutAbstractResultModel* outResultModel) -> bool {
                    inResultModel->recursiveFindAllPossibilitiesInModel(*outResultModel, false);

                    if(inResultModel->hasTheMinimumNumberOfPossibilityRequired())
                        modelsThatHaveFound.insert(inResultModel);

                    return true;
                });
            }

            ++it;
        }

        isCurrentlyInRecursivity = true;
        parentStep = parentStep->parentStep();
    }

    return (modelsThatHaveFound.size() == nResults);
}
