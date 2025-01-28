#ifndef CDM_STDIOPARSER_H
#define CDM_STDIOPARSER_H

#include <QString>

#include "parserAction/cdm_parseractionmanager.h"
#include "cdm_scriptmanagerabstract.h"
#include "cdm_stepmanager.h"

class COMPUTREECORESHARED_EXPORT CDM_StdIOParser : public QObject
{
    Q_OBJECT

public:
    CDM_StdIOParser(CDM_ScriptManagerAbstract &scriptMan,
                    CDM_StepManager &stepMan,
                    CDM_PluginManager &pluginMan);
    virtual ~CDM_StdIOParser();

    void setParserActionManager(CDM_ParserActionManager *parserActionMan);

    virtual QString parseLine(const QString &line);

private:
    CDM_ParserActionManager     *_parserActionManager;
    CDM_ScriptManagerAbstract   *_scriptMan;
    CDM_StepManager             *_stepMan;

    QString                     _script;

signals:

    void asyncMessage(const QString &mess);

protected slots:

    virtual void stepManagerCompleted(bool success);
};

#endif // CDM_STDIOPARSER_H
