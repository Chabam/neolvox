#ifndef CDM_SCRIPTMANAGERXMLALLVERSIONS_H
#define CDM_SCRIPTMANAGERXMLALLVERSIONS_H

#include "cdm_scriptmanagerabstract.h"

/**
 * @brief Class that can read/write a script in XML (new version or last version of scripts)
 */
class COMPUTREECORESHARED_EXPORT CDM_ScriptManagerXMLAllVersions : public CDM_ScriptManagerAbstract
{
    typedef CDM_ScriptManagerAbstract SuperClass;

public:
    CDM_ScriptManagerXMLAllVersions(CDM_PluginManager &pluginManager);
    ~CDM_ScriptManagerXMLAllVersions();

    QList<QString> getFileExtensionAccepted() const;

    QString verifyScript(const QString &filePath);
    QString verifyScriptText(const QString &text);

    QString loadScript(const QString &filePath,
                       CDM_StepManager &stepManager);

    QString loadScriptText(const QString &text,
                           CDM_StepManager &stepManager);

    bool writeScript(const QString &filePath,
                     bool saveResult,
                     CDM_StepManager &stepManager);

private:
    QList<CDM_ScriptManagerAbstract*>   m_xmlManagers;

protected:
    void scriptLoadCallBackChanged();
};

#endif // CDM_SCRIPTMANAGERXMLALLVERSIONS_H
