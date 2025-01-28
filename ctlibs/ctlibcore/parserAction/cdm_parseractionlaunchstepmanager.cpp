#include "parserAction/cdm_parseractionlaunchstepmanager.h"

CDM_ParserActionLaunchStepManager::CDM_ParserActionLaunchStepManager(CDM_StepManager &stepManager) : CDM_AbstractParserAction()
{
    _stepManager = &stepManager;
}

bool CDM_ParserActionLaunchStepManager::internalExecute(const QString &fullCmd)
{
    Q_UNUSED(fullCmd)

    if(_stepManager->isRunning())
    {
        setExecuteError("Le gestionnaire d'etape est déjà lancé.");
        return false;
    }

    return _stepManager->executeStep();
}
