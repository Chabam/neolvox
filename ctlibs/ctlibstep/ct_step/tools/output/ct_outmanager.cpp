#include "ct_outmanager.h"

#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "ct_result/abstract/ct_abstractresult.h"

CT_OutManager::CT_OutManager()
{
}

CT_OutManager::~CT_OutManager()
{
    qDeleteAll(m_results);
}

CT_OutModelStructureManager* CT_OutManager::outModelStructureManager() const
{
    return &const_cast<CT_OutModelStructureManager&>(m_outModelsStructureManager);
}

bool CT_OutManager::visitResultModels(const CT_OutManager::OutResultModelVisitor& visitor) const
{
    return m_outModelsStructureManager.visitResults(visitor);
}

bool CT_OutManager::canCreateOutputModels(const CT_VirtualAbstractStep& step, CreateOutputModelsErrorType* errorType, QString* why) const
{
    if(errorType != nullptr)
        (*errorType) = CreationOK;

    if(step.hasChildrens())
    {
        if(why != nullptr)
            *why = tr("Des étapes filles utilise les modèles de sortie de cette étape.");

        if(errorType != nullptr)
            (*errorType) = StepHasChildrens;

        return false;
    }

    // can not create new output result model if the result is visible in a document
    const bool continueCreate = step.visitOutResultModels([](const CT_OutAbstractResultModel* rModel) -> bool {
        return !rModel->isVisible() && rModel->recursiveVisitOutChildrens([](const CT_OutAbstractModel* model) -> bool {
            return !model->isVisible();
        });
    });

    if(!continueCreate)
    {
        if(why != nullptr)
            *why = tr("Un des résultats de l'étape est affiché dans un document.");

        if(errorType != nullptr)
            (*errorType) = AtLeastOneResultOfStepIsVisibleInDocument;

        return false;
    }

    return true;
}

bool CT_OutManager::createOutputModels(CT_VirtualAbstractStep& step)
{
    if(!canCreateOutputModels(step))
        return false;

    CT_VirtualAbstractStep::UniqueIndexGeneratorPtr uig = step.uniqueIndexGenerator();

    Q_ASSERT(uig != nullptr);
    if (uig == nullptr) {qDebug() << "CT_OutManager::createOutputModels" << ", " <<  "uig == nullptr";}

    uig->resetIndexOfObject(&step);
    uig->setCurrentObject(&step);

    // clear all previously result models added
    m_outModelsStructureManager.clearResults();

    // ignore invalid parent handle if the step is a prototype to resolve possible crash due to Q_ASSERT
    m_outModelsStructureManager.setIgnoreInvalidParentHandle(step.isAPrototype());

    // we ask the step to produce output models, so it will be added to the output models manager
    step.declareOutputModels(m_outModelsStructureManager);

    // we set the step to all models created
    m_outModelsStructureManager.visitResults([&step](const CT_OutAbstractResultModel* outResultModel) -> bool {
        const_cast<CT_OutAbstractResultModel*>(outResultModel)->setStep(&step);
        return true;
    });

    // we ask the output models manager that from now we don't add other models to it and it can
    // finalize models (set a unique index)
    m_outModelsStructureManager.finalize(*uig);

    return true;
}

bool CT_OutManager::createOutputResults(CT_VirtualAbstractStep& step)
{
    if(!m_results.isEmpty())
        return false;

    ResultCollection& results = m_results;

    return m_outModelsStructureManager.visitResults([&step, &results](const CT_OutAbstractResultModel* resultModel) -> bool {
        CT_OutAbstractResultModel* rm = const_cast<CT_OutAbstractResultModel*>(resultModel);
        CT_AbstractResult* result = static_cast<CT_AbstractResult*>(rm->createResult());

        Q_ASSERT(result != nullptr);
        if (result == nullptr) {qDebug() << "CT_OutManager::createOutputResults" << ", " <<  "result == nullptr";}

        result->setParentStep(&step);
        results.append(result);
        return true;
    });
}

void CT_OutManager::finalizeResults()
{
    for(CT_AbstractResult* result : m_results) {
        //PS_MODELS->removeCacheForResult(result);

        if(result->isClearedFromMemory())
        {
            result->setComplete();
            emit resultAdded(result);
        }
    }
}

bool CT_OutManager::visitResults(const CT_OutManager::OutResultVisitor& visitor) const
{
    for(const CT_AbstractResult* result : m_results) {
        if(!visitor(result))
            return false;
    }

    return true;
}

int CT_OutManager::nResult() const
{
    return m_results.size();
}

const CT_AbstractResult* CT_OutManager::resultAt(int index) const
{
    return m_results.at(index);
}

CT_AbstractResult* CT_OutManager::takeResultAt(int index)
{
    return m_results.takeAt(index);
}
