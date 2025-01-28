#ifndef CDM_HRSCRIPTXMLWRITER_H
#define CDM_HRSCRIPTXMLWRITER_H

#include "computreeCore_global.h"

#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "ct_abstractstepplugin.h"

#include <QDomElement>
#include <QStack>

class CDM_PluginManager;

/**
 * @brief Use this class to write a human readable xml script
 */
class COMPUTREECORESHARED_EXPORT CDM_HRScriptXMLWriter : public SettingsWriterInterface
{
    typedef SettingsWriterInterface SuperClass;

public:
    CDM_HRScriptXMLWriter();

    /**
     * @brief Set the plugin manager
     */
    void setPluginManager(const CDM_PluginManager *pm);

    /**
     * @brief Set the absolute filepath to use
     */
    void setFilePath(const QString& absoluteFilePath);

    /**
     * @brief Set the workflow
     */
    void setWorkflow(const QList<CT_VirtualAbstractStep*>& rootSteps);

    /**
     * @brief Write all (this will erase the file if he already exist)
     */
    void writeAll();

    /**
     * @brief Returns true if it was errors for the last write
     */
    bool hasErrors() const;

    void setCallerDescription(const QObject* caller, const QString& description);
    void setCallerUniqueID(const QObject* caller, const int& uniqueID);
    int addParameter(const QObject* caller, const QString& parameterName, const QVariant& parameterValue, const QString& description = "", const int& uniqueID = -1);
    int addParameterPath(const QObject* caller, const QString& parameterName, const QString& fileOrDirectoryPath, const QString& description = "", const int& uniqueID = -1);
    bool addParameterInfo(const QObject* caller, const int& parameterUniqueID, const QString& extraInfoName, const QVariant& extraInfoValue);
    void beginWriteChildrens(const QObject* caller);
    void endWriteChildrens(const QObject* caller);
    int createExtraParametersGroup(const QObject* caller, const QString& valuesName, const QString& groupDescription = "");
    bool linkParameterAndExtraParametersGroup(const QObject* caller, const int& parameterUniqueID, const int& parameterValuesGroupID);
    bool addExtraParametersValueToGroup(const QObject* caller, const QVariant& value, const int& parameterValuesGroupID, const QString& description = "");

private:
    struct ParameterGroupInfo {
        QDomElement element;
        QString     valuesName;
    };

    CDM_PluginManager*              m_pluginManager;
    QString                         m_filePath;
    QList<CT_VirtualAbstractStep*>  m_rootSteps;
    bool                            m_error;

    QDomDocument                    m_document;
    QDomElement                     m_domRoot;
    QDomElement                     m_domPlugins;
    QDomElement                     m_domPrePath;
    QDomElement                     m_domPreParameters;
    QDomElement                     m_domExtraParameters;
    QDomElement                     m_domInputModels;
    QDomElement                     m_domPostParameters;
    QDomElement                     m_domOthersParameters;
    QDomElement                     m_domWorkflow;

    QHash<QObject*, QDomElement>    m_callerDomElements;
    QMap<int, QDomElement>          m_parameters;
    QMap<int, ParameterGroupInfo>   m_extraParametersGroup;
    QStack<QDomElement>             m_parametersStack;
    QStringList                     m_prePath;

    int genericAddParameter(const QObject* caller, const QString& parameterName, const QString& parameterValueName, const QVariant& parameterValue, const QString& description = "", const int& uniqueID = -1);

    void recursiveProcessStep(CT_VirtualAbstractStep* step, QDomElement& parentDom, int& stepID, QSet<CT_AbstractStepPlugin *> &pluginsUsed);
    void processStepPreSettings(CT_VirtualAbstractStep* step, const int& stepID);
    void processStepInputSettings(CT_VirtualAbstractStep* step, const int& stepID);
    void processStepPostSettings(CT_VirtualAbstractStep* step, const int& stepID);
    void processStepOthersSettings(CT_VirtualAbstractStep* step, const int& stepID);

    QString convertPathToRelativePathAndAddToPrePath(QString absolutePath, int& prePathID, bool& isAFolder);
    QDomElement getOrCreateDomElementForCaller(const QObject* caller);
    QDomElement getOrCreateExtraDomElementForCaller(const QObject* caller);
    QDomElement createDomElementForStep(CT_VirtualAbstractStep* step, int stepID);
    void fillDomPlugins(const QSet<CT_AbstractStepPlugin*>& plugins);
    void fillPrePath();

    QString cryptElements(const QList<QDomElement>& toCrypt) const;
};

#endif // CDM_HRSCRIPTXMLWRITER_H
