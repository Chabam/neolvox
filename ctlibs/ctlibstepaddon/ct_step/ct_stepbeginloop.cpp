#include "ct_stepbeginloop.h"

#include "ct_step/tools/ct_logmanageradapterforstep.h"

#include <limits>

CT_StepBeginLoop::CT_StepBeginLoop() : SuperClass()
{
    m_nTurns = 1;
    _counter = QSharedPointer<CT_Counter>(new CT_Counter(m_nTurns));
}

QString CT_StepBeginLoop::description() const
{
    return tr("Démarre une boucle dans le script");
}

QString CT_StepBeginLoop::detailledDescription() const
{
    return tr("Nécessite une CT_StepEndLoop pour terminer la boucle");
}

bool CT_StepBeginLoop::isSettingsModified() const
{
    if(SuperClass::isSettingsModified())
        return true;

    return !recursiveVisitChildrens([](const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep* child) -> bool {
        return !child->isSettingsModified();
    });
}

CT_VirtualAbstractStep* CT_StepBeginLoop::createNewInstance() const
{
    // cree une copie de cette etape
    return new CT_StepBeginLoop();
}

void CT_StepBeginLoop::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.setNotNeedInputResult();
}

void CT_StepBeginLoop::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog)
{
    configDialog->addInt(tr("Nombre d'itérations"), "", 1, std::numeric_limits<int>::max(), m_nTurns);
}

void CT_StepBeginLoop::finalizePostSettings()
{
    _counter->setCurrentTurn(1);
}

void CT_StepBeginLoop::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResult(m_hOutResult, tr("Compteur"));
    manager.setRootGroup(m_hOutResult, m_hOutRootGroup);
    manager.addItem(m_hOutRootGroup, m_hOutLoopCounter, tr("Compteur"));
}

void CT_StepBeginLoop::compute()
{
    for(CT_ResultGroup* result : m_hOutResult.iterateOutputs()) {
        CT_StandardItemGroup* rootGroup = new CT_StandardItemGroup();
        result->addRootGroup(m_hOutRootGroup, rootGroup);

        _counter->setTurnName(QString("Turn%1").arg(_counter->currentTurn()));

        rootGroup->addSingularItem(m_hOutLoopCounter, m_hOutLoopCounter.createInstance(_counter, this));
    }
}

QSharedPointer<CT_Counter> CT_StepBeginLoop::counter() const
{
    return _counter;
}

void CT_StepBeginLoop::addToLogCurrentTurnInformation()
{
    STEP_LOG->addInfoMessage("________________________________________");
    STEP_LOG->addInfoMessage(QString(tr("Début de boucle, tour %1 sur %2 (Nom : %3)")).arg(_counter->currentTurn()).arg(_counter->nTurns()).arg(_counter->turnName()));
}

