#ifndef CDM_STEPLISTDOCEXPORTER_H
#define CDM_STEPLISTDOCEXPORTER_H

#include "computreeCore_global.h"
#include "cdm_stepmanager.h"
#include "cdm_pluginmanager.h"
#include "cdm_steplistinfomanager.h"

#include <QProgressDialog>

#include <QObject>


class COMPUTREECORESHARED_EXPORT CDM_StepListDocExporter : public QObject
{
    Q_OBJECT
public:

    class DocumentatedScriptInfo
    {
    public:
        QString _scriptFileName;
        QString _scriptName;
        QString _description;
        QString _author;
    };

    CDM_StepListDocExporter(CDM_StepManager *stepManager, CDM_PluginManager *pluginManager, QString svnRevision);
    ~CDM_StepListDocExporter();

    void createStepHelp(QString exportDir, QProgressDialog& progressDialog);

    void exportDocumentedScript(QDir &mainDir, QString outDirPath, DocumentatedScriptInfo docInfo);

private:
    QString                                 _svnRevision;
    CDM_StepManager*                        _stepManager;
    CDM_PluginManager*                      _pluginManager;
    CDM_StepListInfoManager*                _stepListInfo;


    void createCSS(QString filename);
    void createJSForIframe(QString dir);
    void createTextContent(QString outDirPath, QString fileName, QString content);

};

#endif // CDM_STEPLISTDOCEXPORTER_H
