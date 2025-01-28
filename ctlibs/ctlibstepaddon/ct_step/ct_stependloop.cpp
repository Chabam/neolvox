#include "ct_stependloop.h"

#include "ct_stepbeginloop.h"

CT_StepEndLoop::CT_StepEndLoop() : CT_AbstractStep()
{
    m_mustRestartFromStep = nullptr;
    setDebuggable(true);
}

QString CT_StepEndLoop::description() const
{
    return tr("Fin de boucle");
}

QString CT_StepEndLoop::detailledDescription() const
{
    return tr("Cette étape permet de terminer une boucle dans un script. Elle nécessite une étape de démarrage de boucle en amont.");
}

QString CT_StepEndLoop::inputDescription() const
{
    return CT_AbstractStep::inputDescription() + tr("<br><br>Le résultat compteur choisi détermine à quel début de boucle cette fin de boucle correspond.");
}

QString CT_StepEndLoop::outputDescription() const
{
    return tr("Cette étape génère une simple copie du résultat d'entrée, sans créer de nouvelle donnée. Cela permet de continuer le script après la boucle. ");
}

QString CT_StepEndLoop::detailsDescription() const
{
    return tr("Il est possible d'emboîter plusieurs boucles. Pour cela il faut bien séléctionner les bon résultats compteurs pour faire correspondre deux à deux les étapes d'ouverture et de fermeture de boucle.");
}

CT_VirtualAbstractStep* CT_StepEndLoop::createNewInstance() const
{
    // cree une copie de cette etape
    return new CT_StepEndLoop();
}

CT_VirtualAbstractStep* CT_StepEndLoop::restartComputeFromStep() const
{
    return m_mustRestartFromStep;
}

void CT_StepEndLoop::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(m_hInResultCopy, tr("Résultat compteur"), QString(), true);
    manager.setRootGroup(m_hInResultCopy, m_hInRootGroup);
    manager.addItem(m_hInRootGroup, m_hInLoopCounter, tr("Compteur"));
}

void CT_StepEndLoop::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(m_hInResultCopy);
}

//////////////////// PROTECTED //////////////////

void CT_StepEndLoop::compute()
{    
    m_mustRestartFromStep = nullptr;

    for(CT_LoopCounter* counter : m_hInLoopCounter.iterateOutputs(m_hInResultCopy)) {

        STEP_LOG->addInfoMessage(tr("Fin de boucle, tour %1 sur %2").arg(counter->currentTurn()).arg(counter->nTurns()));

        // Use the debug mode at step loop scale (and not at step scale as usual)
        if ((currentNumberOfBreakPointToJump() != 0) && ((counter->currentTurn() % currentNumberOfBreakPointToJump()) == 0))
            waitForAckIfInDebugMode();

        if(counter->hasNextTurn())
        {
            m_mustRestartFromStep = counter->beginLoopStep();
            counter->beginNextTurn();
        }
        else
        {
            counter->setCurrentTurn(1);
        }

        return;
    }
}
