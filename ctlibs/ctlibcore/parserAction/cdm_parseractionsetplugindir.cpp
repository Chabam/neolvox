#include "cdm_parseractionsetplugindir.h"

CDM_ParserActionSetPluginDir::CDM_ParserActionSetPluginDir(const CDM_StepManager &stepManager, CDM_PluginManager &pluginManager) : CDM_AbstractParserAction()
{
    _stepManager = &stepManager;
    _pluginManager = &pluginManager;
}

bool CDM_ParserActionSetPluginDir::internalExecute(const QString &fullCmd)
{
    if(_stepManager->isRunning())
    {
        setExecuteError("Le manageur d'etape est lance.");
        return false;
    }
    else if(!_stepManager->getStepRootList().isEmpty())
    {
        setExecuteError("Le manageur d'etape contient des etapes, veuillez le vider d'abord.");
        return false;
    }

    if(!fullCmd.isEmpty())
        _pluginManager->setSearchPath(fullCmd);

    if(!_pluginManager->load())
    {
        setExecuteError(_pluginManager->getError());
        return false;
    }

    return true;
}
