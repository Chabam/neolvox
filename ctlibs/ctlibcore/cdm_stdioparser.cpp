#include "cdm_stdioparser.h"

#include "parserAction/cdm_parseractiongetplugindir.h"
#include "parserAction/cdm_parseractionsetplugindir.h"
#include "parserAction/cdm_parseractionlaunchstepmanager.h"

#include "computreeCoreDefaultIOCmd.h"

#include <QCoreApplication>
#include <QDebug>

CDM_StdIOParser::CDM_StdIOParser(CDM_ScriptManagerAbstract &scriptMan,
                                 CDM_StepManager &stepMan,
                                 CDM_PluginManager &pluginMan)
{
    _parserActionManager = new CDM_ParserActionManager();

    _parserActionManager->addAction(XSTR_TO_QSTRING(IOCmd_REQUEST_GET_PLUGIN_DIR_BEGIN), new CDM_ParserActionGetPluginDir(pluginMan));
    _parserActionManager->addAction(XSTR_TO_QSTRING(IOCmd_REQUEST_RELOAD_PLUGIN_BEGIN), new CDM_ParserActionSetPluginDir(stepMan, pluginMan));
    _parserActionManager->addAction(XSTR_TO_QSTRING(IOCmd_REQUEST_START_STEP_MANAGER_BEGIN), new CDM_ParserActionLaunchStepManager(stepMan));

    _scriptMan = &scriptMan;
    _stepMan = &stepMan;

    connect(_stepMan, SIGNAL(completed(bool)), this, SLOT(stepManagerCompleted(bool)));
}

CDM_StdIOParser::~CDM_StdIOParser()
{
    delete _parserActionManager;
}

QString CDM_StdIOParser::parseLine(const QString &line)
{
    QString error = XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_BEGIN) + XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_END);

    if(_parserActionManager != nullptr)
    {
        QString newCmd;

        CDM_IParserAction *pAction = _parserActionManager->findAction(line, &newCmd);

        if(pAction != nullptr)
        {
            if(!pAction->execute(newCmd))
            {
                return XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_BEGIN) + pAction->getExecuteError() + XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_END);
            }

            return XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_BEGIN) + pAction->getExecuteResponse() + XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_END);
        }
    }

    _script += line;

    if(line == "</root>")
    {
        QString tmp = _script;
        _script = "";

        if((_scriptMan != nullptr)
                && (_stepMan != nullptr))
        {
            if(_stepMan->isRunning())
                return QCoreApplication::translate("parseLine", "Un processus est deja en cours, veuillez recommencer plus tard.");

            error = _scriptMan->verifyScriptText(tmp);

            if(error.isEmpty())
            {
                error = _scriptMan->loadScriptText(tmp,
                                                   *_stepMan);

                if(error.isEmpty())
                    error = XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_BEGIN) + XSTR_TO_QSTRING(IOCmd_RESPONSE_OK_END);
                else
                    error = XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_BEGIN) + error + XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_END);
            }
            else
                error = XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_BEGIN) + error + XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_END);
        }
        else if(_scriptMan == nullptr)
        {
            return XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_BEGIN) + "Aucun manageur de script connu" + XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_END);
        }
        else if(_stepMan == nullptr)
        {
            return XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_BEGIN) + "Aucun manageur d'etape connu" + XSTR_TO_QSTRING(IOCmd_RESPONSE_ERROR_END);
        }
    }

    return error;
}

void CDM_StdIOParser::setParserActionManager(CDM_ParserActionManager *parserActionMan)
{
    delete _parserActionManager;
    _parserActionManager = parserActionMan;
}

void CDM_StdIOParser::stepManagerCompleted(bool success)
{
    emit asyncMessage(XSTR_TO_QSTRING(IOCmd_STEPMANAGER_COMPLETED_BEGIN) + QString("%1").arg(success ? "true" : "false") + XSTR_TO_QSTRING(IOCmd_STEPMANAGER_COMPLETED_END));
}
