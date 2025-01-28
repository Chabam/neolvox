#ifndef CDM_HRSCRIPTMANAGERXML_H
#define CDM_HRSCRIPTMANAGERXML_H

#include "cdm_scriptmanagerabstract.h"

class QDomElement;
class QTextStream;

/**
 * @brief Class that can read/write a script in XML that was human readable/modifiable
 */
class COMPUTREECORESHARED_EXPORT CDM_HRScriptManagerXML : public CDM_ScriptManagerAbstract
{
public:
    CDM_HRScriptManagerXML(CDM_PluginManager &pluginManager);

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
};

#endif // CDM_HRSCRIPTMANAGERXML_H
