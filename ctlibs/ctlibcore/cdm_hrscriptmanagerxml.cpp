#include "cdm_hrscriptmanagerxml.h"

#include "cdm_hrscriptxmlwriter.h"
#include "cdm_hrscriptxmlreader.h"

CDM_HRScriptManagerXML::CDM_HRScriptManagerXML(CDM_PluginManager &pluginManager) : CDM_ScriptManagerAbstract(pluginManager)
{

}

QList<QString> CDM_HRScriptManagerXML::getFileExtensionAccepted() const
{
    QStringList l;
    l << ".cts";

    return std::move(l);
}

QString CDM_HRScriptManagerXML::verifyScript(const QString &filePath)
{
    CDM_HRScriptXmlReader reader;
    reader.setPluginManager(getPluginManager());
    reader.setScriptLoadCallBack(scriptLoadCallBack());
    reader.verifyFile(filePath);

    return reader.errors();
}

QString CDM_HRScriptManagerXML::verifyScriptText(const QString &text)
{
    QDomDocument doc;
    if(!doc.setContent(text)) {
        return QObject::tr("Texte non XML");
    }

    CDM_HRScriptXmlReader reader;
    reader.setPluginManager(getPluginManager());
    reader.setScriptLoadCallBack(scriptLoadCallBack());
    reader.verifyDocument(doc);

    return reader.errors();
}

QString CDM_HRScriptManagerXML::loadScript(const QString &filePath,
                                           CDM_StepManager &stepManager)
{
    CDM_HRScriptXmlReader reader;
    reader.setPluginManager(getPluginManager());
    reader.setStepManager(&stepManager);
    reader.setScriptLoadCallBack(scriptLoadCallBack());
    reader.readFile(filePath);

    return reader.errors();
}

QString CDM_HRScriptManagerXML::loadScriptText(const QString &text,
                                               CDM_StepManager &stepManager)
{
    QDomDocument doc;
    if(!doc.setContent(text)) {
        return QObject::tr("Texte non XML");
    }

    CDM_HRScriptXmlReader reader;
    reader.setPluginManager(getPluginManager());
    reader.setStepManager(&stepManager);
    reader.setScriptLoadCallBack(scriptLoadCallBack());
    reader.readDocument(doc);

    return reader.errors();
}

bool CDM_HRScriptManagerXML::writeScript(const QString &filePath,
                                         bool /*saveResult*/,
                                         CDM_StepManager &stepManager)
{
    CDM_HRScriptXMLWriter writer;
    writer.setPluginManager(getPluginManager());
    writer.setFilePath(filePath);
    writer.setWorkflow(stepManager.getStepRootList());
    writer.writeAll();

    return writer.hasErrors();
}
