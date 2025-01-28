#include "cdm_scriptmanagerxmlallversions.h"

#include "cdm_hrscriptmanagerxml.h"

CDM_ScriptManagerXMLAllVersions::CDM_ScriptManagerXMLAllVersions(CDM_PluginManager& pluginManager) : SuperClass(pluginManager)
{
    m_xmlManagers.append(new CDM_HRScriptManagerXML(pluginManager));
}

CDM_ScriptManagerXMLAllVersions::~CDM_ScriptManagerXMLAllVersions()
{
    qDeleteAll(m_xmlManagers.begin(), m_xmlManagers.end());
}

QList<QString> CDM_ScriptManagerXMLAllVersions::getFileExtensionAccepted() const
{
    QList<QString> l;

    for(SuperClass* manager : m_xmlManagers)
        l.append(manager->getFileExtensionAccepted());

    return l;
}

QString CDM_ScriptManagerXMLAllVersions::verifyScript(const QString &filePath)
{
    for(SuperClass* manager : m_xmlManagers) {
        if(manager->acceptFile(filePath))
            return manager->verifyScript(filePath);
    }

    return "";
}

QString CDM_ScriptManagerXMLAllVersions::verifyScriptText(const QString &text)
{
    for(SuperClass* manager : m_xmlManagers) {
        if(dynamic_cast<CDM_HRScriptManagerXML*>(manager) != nullptr)
            return manager->verifyScriptText(text);
    }

    return "";
}

QString CDM_ScriptManagerXMLAllVersions::loadScript(const QString &filePath, CDM_StepManager &stepManager)
{
    for(SuperClass* manager : m_xmlManagers) {
        if(manager->acceptFile(filePath))
            return manager->loadScript(filePath, stepManager);
    }

    return "";
}

QString CDM_ScriptManagerXMLAllVersions::loadScriptText(const QString &text, CDM_StepManager &stepManager)
{
    for(SuperClass* manager : m_xmlManagers) {
        if(dynamic_cast<CDM_HRScriptManagerXML*>(manager) != nullptr)
            return manager->loadScriptText(text, stepManager);
    }

    return "";
}

bool CDM_ScriptManagerXMLAllVersions::writeScript(const QString &filePath, bool saveResult, CDM_StepManager &stepManager)
{
    for(SuperClass* manager : m_xmlManagers) {
        if(manager->acceptFile(filePath))
            return manager->writeScript(filePath, saveResult, stepManager);
    }

    return false;
}

void CDM_ScriptManagerXMLAllVersions::scriptLoadCallBackChanged()
{
    for(SuperClass* manager : m_xmlManagers)
        manager->setScriptLoadCallBack(scriptLoadCallBack());
}
