#ifndef CDM_HRSCRIPTXMLREADER_H
#define CDM_HRSCRIPTXMLREADER_H

#include "cdm_scriptmanagerabstract.h"

#include <QDomElement>
#include <QStack>

class CDM_PluginManager;
class CDM_StepManager;

/**
 * @brief Use this class to read a human readable xml script
 */
class COMPUTREECORESHARED_EXPORT CDM_HRScriptXmlReader : public SettingsReaderInterface
{
    typedef SettingsReaderInterface SuperClass;

public:
    CDM_HRScriptXmlReader();

    /**
     * @brief Set the plugin manager
     */
    void setPluginManager(const CDM_PluginManager* pm);

    /**
     * @brief Set the step manager
     */
    void setStepManager(const CDM_StepManager* stepManager);

    /**
     * @brief Set the object who get the callback error when a script is not successfully loaded
     */
    void setScriptLoadCallBack(const CDM_ScriptManagerAbstract::IScriptLoadCallBack *c);

    /**
     * @brief Returns the object that was called when an error ocurred when a script is loaded
     */
    CDM_ScriptManagerAbstract::IScriptLoadCallBack* scriptLoadCallBack() const;

    /**
     * @brief Read the file
     * @param absoluteFilePath : absolute filepath to use
     */
    void readFile(const QString& absoluteFilePath);

    /**
     * @brief Read the document
     */
    void readDocument(const QDomDocument& doc);

    /**
     * @brief Only verify if plugins and step is founded
     * @param absoluteFilePath : absolute filepath to use
     */
    void verifyFile(const QString& absoluteFilePath);

    /**
     * @brief Only verify if plugins and step is founded
     */
    void verifyDocument(const QDomDocument& doc);

    /**
     * @brief Returns errors of last read
     */
    QString errors() const;

    bool findCallerWithUniqueIDAndSetToCurrent(const QObject* caller, const int& uniqueID);
    int parameter(const QObject* caller, const QString& parameterName, QVariant& parameterValueRestored, const QVariant& defaultParameterValue = QVariant(), const int& uniqueID = -1);
    int parameterCount(const QObject *caller, const QString &parameterName, const int& uniqueID = -1);
    int parameterPath(const QObject* caller, const QString& parameterName, QString& fileOrDirectoryPathRestored, const QString& defaultFileOrDirectoryPath = "", const int& uniqueID = -1);
    int parameterPathCount(const QObject *caller, const QString &parameterName, const int& uniqueID = -1);
    bool parameterInfo(const QObject* caller, const int& parameterUniqueID, const QString& extraInfoName, QVariant& extraInfoValueRestored, const QVariant& defaultValue = QVariant());
    void beginReadChildrens(const QObject* caller);
    void endReadChildrens(const QObject* caller);

private:
    struct DomElementCallerInfo {
        QDomElement element;
        QHash<QString, QDomElement> currentParameters;
        QHash<QString, QDomElement> currentChilds;
    };

    enum LoadErrorStrategy {
        LES_Continue,
        LES_StopAndKeep,
        LES_StopAndClear
    };

    CDM_ScriptManagerAbstract::IScriptLoadCallBack* m_scriptLoadCallBack;

    CDM_PluginManager*              m_pluginManager;
    CDM_StepManager*                m_stepManager;

    QDomDocument                    m_documentDecrypted;
    QDomElement                     m_domPrePath;
    QDomElement                     m_domPreParameters;
    QDomElement                     m_domInputModels;
    QDomElement                     m_domPostParameters;
    QDomElement                     m_domOthersParameters;
    QDomElement                     m_domWorkflow;

    QString                         m_scriptPath;

    mutable QString                 m_errors;
    QString                         mLastErrorAction;

    bool                            m_allParametersFounded;

    QHash<QObject*, DomElementCallerInfo*>  m_callerDomElements;
    QMap<int, QDomElement>                  m_parameters;
    //QMap<int, ParameterGroupInfo>         m_extraParametersGroup;
    QStack<DomElementCallerInfo*>           m_parametersStack;
    QHash<int, QString>                     m_prePath;

    void loadXMLElements(QDomElement root);
    void loadPrePath();
    void processWorkflow();
    void verifyWorkflow();

    int genericParameter(const QObject* caller, const QString& parameterName, const QString& parameterValueName, QString& parameterValueRestored, const QString& defaultParameterValue = QString(), const int& uniqueID = -1);

    CT_VirtualAbstractStep* recursiveAddAndLoadStep(QDomElement currentStepEL, CT_VirtualAbstractStep* parentStep, LoadErrorStrategy &loadErrorStrategy);
    bool recursiveVerifyIfFoundAll(QDomElement currentStepEL, bool hasParentStep) const;
    bool extractInformationsToCreateStep(QDomElement currentStepEL, CT_VirtualAbstractStep* parentStep, QString& pluginName, QString& stepKey, int& stepID, CT_AbstractStepPlugin** plugin, LoadErrorStrategy& loadErrorStrategy) const;

    CT_AbstractStepPlugin* findPlugin(QString pluginName, const QString& stepKey, LoadErrorStrategy& loadErrorStrategy) const;
    QString findStepInPlugin(QString stepKey, CT_VirtualAbstractStep *parentStep, CT_AbstractStepPlugin *plugin, LoadErrorStrategy& loadErrorStrategy) const;
    CT_VirtualAbstractStep* createStepFromOriginal(const QString &stepKey, CT_VirtualAbstractStep *parentStep, CT_AbstractStepPlugin *plugin, LoadErrorStrategy& loadErrorStrategy) const;

    QDomElement findDomElementForStep(const int &stepID, QDomElement parentElement);
    DomElementCallerInfo* findDomElementForStepAndCreateInfo(const int &stepID, QDomElement parentElement);
    DomElementCallerInfo* findDomElementForCaller(const QObject* caller);
    QDomElement getNextParameterOrFindCurrentAndAddItInInfo(DomElementCallerInfo *info, const QString &parameterName, const int &uniqueID = -1);

    bool processStepPreSettings(CT_VirtualAbstractStep* step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy);
    bool processStepInputSettings(CT_VirtualAbstractStep* step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy);
    bool processStepPostSettings(CT_VirtualAbstractStep* step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy);
    bool processStepOthersSettings(CT_VirtualAbstractStep* step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy);
};

#endif // CDM_HRSCRIPTXMLREADER_H
