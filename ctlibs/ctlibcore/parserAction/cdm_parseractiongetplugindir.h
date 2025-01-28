#ifndef CDM_PARSERACTIONGETPLUGINDIR_H
#define CDM_PARSERACTIONGETPLUGINDIR_H

#include "cdm_abstractparseraction.h"
#include "cdm_pluginmanager.h"

class COMPUTREECORESHARED_EXPORT CDM_ParserActionGetPluginDir : public CDM_AbstractParserAction
{
public:
    CDM_ParserActionGetPluginDir(const CDM_PluginManager &pluginManager);

protected:

    bool internalExecute(const QString &fullCmd);

private:

    const CDM_PluginManager     *_pluginManager;
};

#endif // CDM_PARSERACTIONGETPLUGINDIR_H
