#ifndef CDM_PARSERACTIONSETPLUGINDIR_H
#define CDM_PARSERACTIONSETPLUGINDIR_H

#include "cdm_abstractparseraction.h"
#include "cdm_pluginmanager.h"
#include "cdm_stepmanager.h"

class COMPUTREECORESHARED_EXPORT CDM_ParserActionSetPluginDir : public CDM_AbstractParserAction
{
public:
    CDM_ParserActionSetPluginDir(const CDM_StepManager &stepManager, CDM_PluginManager &pluginManager);

protected:

    bool internalExecute(const QString &fullCmd);

private:

    const CDM_StepManager       *_stepManager;
    CDM_PluginManager           *_pluginManager;
};

#endif // CDM_PARSERACTIONSETPLUGINDIR_H
