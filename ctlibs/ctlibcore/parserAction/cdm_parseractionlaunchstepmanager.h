#ifndef CDM_PARSERACTIONLAUNCHSTEPMANAGER_H
#define CDM_PARSERACTIONLAUNCHSTEPMANAGER_H

#include "parserAction/cdm_abstractparseraction.h"
#include "cdm_stepmanager.h"

class COMPUTREECORESHARED_EXPORT CDM_ParserActionLaunchStepManager : public CDM_AbstractParserAction
{
public:
    CDM_ParserActionLaunchStepManager(CDM_StepManager &stepManager);

protected:

    bool internalExecute(const QString &fullCmd);

private:

    CDM_StepManager       *_stepManager;
};

#endif // CDM_PARSERACTIONLAUNCHSTEPMANAGER_H
