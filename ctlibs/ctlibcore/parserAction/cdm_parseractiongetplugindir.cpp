#include "cdm_parseractiongetplugindir.h"

CDM_ParserActionGetPluginDir::CDM_ParserActionGetPluginDir(const CDM_PluginManager &pluginManager) : CDM_AbstractParserAction()
{
    _pluginManager = &pluginManager;
}

bool CDM_ParserActionGetPluginDir::internalExecute(const QString &fullCmd)
{
    Q_UNUSED(fullCmd)

    setExecuteResponse(_pluginManager->getSearchPath());

    return true;
}
