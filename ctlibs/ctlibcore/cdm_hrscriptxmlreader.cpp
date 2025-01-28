#include "cdm_hrscriptxmlreader.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>

#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "ct_abstractstepplugin.h"

#include "cdm_pluginmanager.h"
#include "cdm_stepmanager.h"
#include "cdm_xmltools.h"

CDM_HRScriptXmlReader::CDM_HRScriptXmlReader()
{
    m_pluginManager = nullptr;
    m_stepManager = nullptr;
}

void CDM_HRScriptXmlReader::setPluginManager(const CDM_PluginManager *pm)
{
    m_pluginManager = const_cast<CDM_PluginManager*>(pm);
}

void CDM_HRScriptXmlReader::setStepManager(const CDM_StepManager *stepManager)
{
    m_stepManager = const_cast<CDM_StepManager*>(stepManager);
}

void CDM_HRScriptXmlReader::setScriptLoadCallBack(const CDM_ScriptManagerAbstract::IScriptLoadCallBack *c)
{
    m_scriptLoadCallBack = const_cast<CDM_ScriptManagerAbstract::IScriptLoadCallBack*>(c);
}

CDM_ScriptManagerAbstract::IScriptLoadCallBack *CDM_HRScriptXmlReader::scriptLoadCallBack() const
{
    return m_scriptLoadCallBack;
}

void CDM_HRScriptXmlReader::readFile(const QString &absoluteFilePath)
{
    m_errors = "";

    QFile f(absoluteFilePath);
    QFileInfo info(absoluteFilePath);
    m_scriptPath = info.absolutePath();

    if(f.open(QIODevice::ReadOnly))
    {
        QDomDocument document;
        m_documentDecrypted = QDomDocument();

        if (document.setContent(&f))
            readDocument(document);

        f.close();
    }
}

void CDM_HRScriptXmlReader::readDocument(const QDomDocument &doc)
{
    QDomElement root = doc.documentElement();

    if(!root.isNull()) {
        loadXMLElements(root);
        loadPrePath();
        processWorkflow();
    }

    m_parameters.clear();
}

void CDM_HRScriptXmlReader::verifyFile(const QString &absoluteFilePath)
{
    m_errors = "";

    QFile f(absoluteFilePath);
    QFileInfo info(absoluteFilePath);
    m_scriptPath = info.absolutePath();

    if(f.open(QIODevice::ReadOnly))
    {
        QDomDocument document;
        m_documentDecrypted = QDomDocument();

        if (document.setContent(&f))
            verifyDocument(document);

        f.close();
    }
}

void CDM_HRScriptXmlReader::verifyDocument(const QDomDocument &doc)
{
    QDomElement root = doc.documentElement();

    if(!root.isNull()) {
        loadXMLElements(root);
        loadPrePath();
        verifyWorkflow();
    }

    m_parameters.clear();
}

QString CDM_HRScriptXmlReader::errors() const
{
    return m_errors;
}

bool CDM_HRScriptXmlReader::findCallerWithUniqueIDAndSetToCurrent(const QObject * /*caller*/, const int &/*uniqueID*/)
{
    // TODO !
    return false;
}

int CDM_HRScriptXmlReader::parameter(const QObject *caller, const QString &parameterName, QVariant &parameterValueRestored, const QVariant &defaultParameterValue, const int &uniqueID)
{
    QString v;
    const int id = genericParameter(caller, parameterName, "value", v, defaultParameterValue.toString(), uniqueID);

    parameterValueRestored.setValue(v);

    if(id == 0) {
        m_allParametersFounded = false;
        mLastErrorAction = QObject::tr("Erreur lors de la lecture du paramètre %1::%2").arg(caller->metaObject()->className()).arg(parameterName);
    }

    return id;
}

int CDM_HRScriptXmlReader::parameterCount(const QObject *caller, const QString &parameterName, const int &uniqueID)
{
    DomElementCallerInfo* elInfo = findDomElementForCaller(caller);
    QDomElement el = elInfo->element.firstChildElement(parameterName);

    int count = 0;

    while(!el.isNull()) {

        if(uniqueID == el.attribute("pUID", "-1").toInt())
            ++count;

        el = el.nextSiblingElement(parameterName);
    };

    return count;
}

int CDM_HRScriptXmlReader::parameterPath(const QObject *caller, const QString &parameterName, QString &fileOrDirectoryPathRestored, const QString &defaultFileOrDirectoryPath, const int &uniqueID)
{
    const int id = genericParameter(caller, parameterName, "", fileOrDirectoryPathRestored, defaultFileOrDirectoryPath, uniqueID);

    if(id > 0) {
        bool ok;
        QDomElement el = m_parameters.value(id);
        fileOrDirectoryPathRestored = el.text();
        const int prePathID = el.attribute("prePathID", "-1").toInt(&ok);

        if(ok) {
            const QString prePath = m_prePath.value(prePathID, "");

            if(!prePath.isEmpty()) {
                if(prePath.endsWith('/') || prePath.endsWith('\\'))
                    fileOrDirectoryPathRestored = QDir::toNativeSeparators(prePath + fileOrDirectoryPathRestored);
                else
                    fileOrDirectoryPathRestored = QDir::toNativeSeparators(prePath + QDir::separator() + fileOrDirectoryPathRestored);
            }
        } else {
            m_allParametersFounded = false;
            mLastErrorAction = QObject::tr("Erreur lors de la lecture du chemin %1::%2").arg(caller->metaObject()->className()).arg(parameterName);
        }
    }

    if(id == 0) {
        m_allParametersFounded = false;
        mLastErrorAction = QObject::tr("Erreur lors de la lecture du chemin %1::%2").arg(caller->metaObject()->className()).arg(parameterName);
    }

    return id;
}

int CDM_HRScriptXmlReader::parameterPathCount(const QObject *caller, const QString &parameterName, const int &uniqueID)
{
    // TODO : find a way to verify that is a path
    return parameterCount(caller, parameterName, uniqueID);
}

bool CDM_HRScriptXmlReader::parameterInfo(const QObject *caller, const int &parameterUniqueID, const QString &extraInfoName, QVariant &extraInfoValueRestored, const QVariant &defaultValue)
{
    Q_UNUSED(caller)

    extraInfoValueRestored = defaultValue;

    QDomElement el = m_parameters.value(parameterUniqueID);

    if(el.isNull() || !el.hasAttribute(extraInfoName)) {
        m_allParametersFounded = false;
        mLastErrorAction = QObject::tr("Erreur lors de la lecture de l'information %1::%2").arg(caller->metaObject()->className()).arg(extraInfoName);
        return false;
    }

    extraInfoValueRestored.setValue(CDM_XMLTools::decodeEntities(el.attribute(extraInfoName, defaultValue.toString())));
    return true;
}

void CDM_HRScriptXmlReader::beginReadChildrens(const QObject *caller)
{
    m_parametersStack.push(findDomElementForCaller(caller));
}

void CDM_HRScriptXmlReader::endReadChildrens(const QObject *caller)
{
    Q_UNUSED(caller)

    m_parametersStack.pop();
}

void CDM_HRScriptXmlReader::loadXMLElements(QDomElement root)
{
    m_domPrePath = root.firstChildElement("PrePath");
    m_domPostParameters = root.firstChildElement("PostParameters");
    m_domWorkflow = root.firstChildElement("Workflow");

    // crypted
    m_domPreParameters = root.firstChildElement("PreParameters");
    m_domInputModels = root.firstChildElement("InputModels");
    m_domOthersParameters = root.firstChildElement("OthersParameters");

    QDomElement notModifiableEL = root.firstChildElement("NotModifiable");

    if(!notModifiableEL.isNull()) {
        const QString str = notModifiableEL.text();
        const QByteArray unCrypted = QByteArray::fromBase64(QByteArray::fromStdString(str.toStdString()));

        const QString unCryptedStr = "<DOC>" + QString(unCrypted) + "</DOC>";

        m_documentDecrypted.setContent(unCryptedStr);

        QDomElement root = m_documentDecrypted.documentElement();
        QDomElement pre = root.firstChildElement("PreParameters");

        if(!pre.isNull())
            m_domPreParameters = pre;

        QDomElement input = root.firstChildElement("InputModels");

        if(!input.isNull())
            m_domInputModels= input;

        QDomElement others = root.firstChildElement("OthersParameters");

        if(!others.isNull())
            m_domOthersParameters = others;

        /*QFile fil("tmp.xml");
        fil.open(QFile::WriteOnly);
        QTextStream tmpStream(&fil);
        tmpStream << m_documentDecrypted.toString();
        fil.close();*/
    }
}

void CDM_HRScriptXmlReader::loadPrePath()
{
    m_prePath.clear();

    if(!m_domPrePath.isNull()) {
        QDomElement el_relative = m_domPrePath.firstChildElement("RelativeToScriptPath");
        bool isRelative = !el_relative.isNull();

        QDomElement el = m_domPrePath.firstChildElement("Path");

        while(!el.isNull()) {
            const int prePathID = el.attribute("prePathID", "-1").toInt();

            if(prePathID >= 0 && !el.firstChild().toText().isNull())
            {
                QString path = el.firstChild().toText().data();

                if (isRelative)
                {
                    path = m_scriptPath + "/" + path;
                }

                m_prePath.insert(prePathID, path);
            }

            el = el.nextSiblingElement("Path");
        }
    }
}

void CDM_HRScriptXmlReader::processWorkflow()
{
    QDomElement rootStepEL = m_domWorkflow.firstChildElement("Step");

    LoadErrorStrategy loadErrorStrategy = LES_Continue;
    QList<CT_VirtualAbstractStep*> rootStepsAdded;

    while(!rootStepEL.isNull() && (loadErrorStrategy == LES_Continue)) {

        CT_VirtualAbstractStep* rootStep = recursiveAddAndLoadStep(rootStepEL, nullptr, loadErrorStrategy);

        if(rootStep != nullptr)
            rootStepsAdded.append(rootStep);

        rootStepEL = rootStepEL.nextSiblingElement("Step");
    }

    if(loadErrorStrategy == LES_StopAndClear) {
        foreach(CT_VirtualAbstractStep* rootStep, rootStepsAdded) {
            m_stepManager->removeStep(rootStep);
        }
    }
}

void CDM_HRScriptXmlReader::verifyWorkflow()
{
    QDomElement rootStepEL = m_domWorkflow.firstChildElement("Step");

    bool ok = true;

    while(!rootStepEL.isNull() && ok) {

        ok = recursiveVerifyIfFoundAll(rootStepEL, false);

        rootStepEL = rootStepEL.nextSiblingElement("Step");
    }
}

int CDM_HRScriptXmlReader::genericParameter(const QObject *caller, const QString &parameterName, const QString &parameterValueName, QString &parameterValueRestored, const QString &defaultParameterValue, const int &uniqueID)
{
    const int id = m_parameters.isEmpty() ? 1 : (m_parameters.lastKey()+1);

    parameterValueRestored = defaultParameterValue;

    DomElementCallerInfo* elInfo = findDomElementForCaller(caller);
    QDomElement child = getNextParameterOrFindCurrentAndAddItInInfo(elInfo, parameterName, uniqueID);

    if(child.isNull())
        return 0;

    if(!parameterValueName.isEmpty())
        parameterValueRestored = CDM_XMLTools::decodeEntities(child.attribute(parameterValueName, defaultParameterValue));

    m_parameters.insert(id, child);
    return id;
}

CT_VirtualAbstractStep* CDM_HRScriptXmlReader::recursiveAddAndLoadStep(QDomElement currentStepEL, CT_VirtualAbstractStep *parentStep, LoadErrorStrategy &loadErrorStrategy)
{
    QString pluginName;
    QString stepKey;
    int stepID;
    CT_AbstractStepPlugin* plugin = nullptr;

    if(extractInformationsToCreateStep(currentStepEL, parentStep, pluginName, stepKey, stepID, &plugin, loadErrorStrategy)) {

        CT_VirtualAbstractStep* newStep = createStepFromOriginal(stepKey, parentStep, plugin, loadErrorStrategy);

        if(newStep == nullptr)
            return nullptr;

        // return false if manually configured
        if(!processStepPreSettings(newStep, stepID, stepKey, parentStep, pluginName, loadErrorStrategy)) {

            // if manually configuration was canceled
            if(loadErrorStrategy != LES_Continue) {
                delete newStep;
                return nullptr;
            }
        } else if(!processStepInputSettings(newStep, stepID, stepKey, parentStep, pluginName, loadErrorStrategy)) {

            // if manually configuration was canceled
            if(loadErrorStrategy != LES_Continue) {
                delete newStep;
                return nullptr;
            }
        } else if(!processStepPostSettings(newStep, stepID, stepKey, parentStep, pluginName, loadErrorStrategy)) {

            // if manually configuration was canceled
            if(loadErrorStrategy != LES_Continue) {
                delete newStep;
                return nullptr;
            }
        }

        if(!processStepOthersSettings(newStep, stepID, stepKey, parentStep, pluginName, loadErrorStrategy)) {
            delete newStep;
            return nullptr;
        }

        const bool debugOn = currentStepEL.attribute("debug", "false") == "true";

        newStep->setDebugModeOn(debugOn);
        newStep->finalizeConfiguration();

        if(!m_stepManager->addStep(newStep, parentStep))
        {
            delete newStep;

            const QString err = QObject::tr("Erreur lors de l'ajout de l'étape \"%1\" du plugin \"%2\" au gestionnaire d'étapes.").arg(stepKey).arg(pluginName);

            m_errors += err + QObject::tr("\r\n");

            CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_NoSolution, err, nullptr, parentStep, m_pluginManager->getPluginIndex(plugin), pluginName, stepKey);

            if(scriptLoadCallBack() != nullptr)
                scriptLoadCallBack()->loadScriptError(problem);

            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);

            return nullptr;
        }

        QDomElement childEL = currentStepEL.firstChildElement("Step");

        if(!childEL.isNull())
            recursiveAddAndLoadStep(childEL, newStep, loadErrorStrategy);

        if(parentStep != nullptr) {
            currentStepEL = currentStepEL.nextSiblingElement("Step");

            // while(!currentStepEL.isNull()) {
            if(!currentStepEL.isNull()) {
                recursiveAddAndLoadStep(currentStepEL, parentStep, loadErrorStrategy);
            }
        }

        return newStep;
    }

    return nullptr;
}

bool CDM_HRScriptXmlReader::recursiveVerifyIfFoundAll(QDomElement currentStepEL, bool hasParentStep) const
{
    QString pluginName;
    QString stepKey;
    int stepID;
    CT_AbstractStepPlugin* plugin = nullptr;
    LoadErrorStrategy loadErrorStrategy = LES_StopAndClear; // LES_StopAndClear to disable search plugin or step manually

    if(extractInformationsToCreateStep(currentStepEL, nullptr, pluginName, stepKey, stepID, &plugin, loadErrorStrategy)) {
        QDomElement childEL = currentStepEL.firstChildElement("Step");

        if(!childEL.isNull()) {
            if(!recursiveVerifyIfFoundAll(childEL, true))
                return false;
        }

        if(hasParentStep) {
            currentStepEL = currentStepEL.nextSiblingElement("Step");

            while(!currentStepEL.isNull()) {
                if(!recursiveVerifyIfFoundAll(currentStepEL, hasParentStep))
                    return false;

                currentStepEL = currentStepEL.nextSiblingElement("Step");
            }
        }
    }

    return false;
}

bool CDM_HRScriptXmlReader::extractInformationsToCreateStep(QDomElement currentStepEL, CT_VirtualAbstractStep* parentStep, QString &pluginName, QString &stepKey, int &stepID, CT_AbstractStepPlugin **plugin, LoadErrorStrategy& loadErrorStrategy) const
{
    bool ok;
    pluginName = CDM_XMLTools::decodeEntities(currentStepEL.attribute("plugin", ""));
    stepKey = CDM_XMLTools::decodeEntities(currentStepEL.attribute("type", ""));
    stepID = currentStepEL.attribute("id", "-1").toInt(&ok);
    *plugin = nullptr;

    if((stepID == -1) || !ok)
    {
        const QString err = QObject::tr("Erreur lors de la lecture du paramètre \"id\" de l'étape \"%1\".").arg(stepKey);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_NoSolution, err, nullptr, parentStep, -1, "", stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);
        return false;
    }

    *plugin = findPlugin(pluginName, stepKey, loadErrorStrategy);

    if((*plugin) == nullptr)
        return false;

    stepKey = findStepInPlugin(stepKey, parentStep, (*plugin), loadErrorStrategy);

    if(stepKey.isEmpty())
        return false;

    return true;
}

CT_AbstractStepPlugin* CDM_HRScriptXmlReader::findPlugin(QString pluginName, const QString &stepKey, LoadErrorStrategy& loadErrorStrategy) const
{
    CT_AbstractStepPlugin* plugin = m_pluginManager->getPlugin(pluginName);

    if(plugin == nullptr) {
        const QString err = QObject::tr("Le plugin \"%1\" pour l'étape \"%2\" ne semble pas être chargé.").arg(pluginName).arg(stepKey);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, ((loadErrorStrategy != LES_Continue) ? CDM_ScriptProblem::TOP_NoSolution : CDM_ScriptProblem::TOP_PluginNotFound), err, nullptr, nullptr, -1, pluginName);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        pluginName = m_pluginManager->getPluginName(problem.getSolutionPluginToUse());

        if(!pluginName.isEmpty())
            plugin = m_pluginManager->getPlugin(pluginName);

        if(plugin == nullptr)
            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);
    }

    return plugin;
}

QString CDM_HRScriptXmlReader::findStepInPlugin(QString stepKey, CT_VirtualAbstractStep* parentStep, CT_AbstractStepPlugin* plugin, LoadErrorStrategy& loadErrorStrategy) const
{
    const int pluginIndex = m_pluginManager->getPluginIndex(plugin);

    CT_VirtualAbstractStep *step = plugin->getStepFromKey(stepKey);

    if(step == nullptr)
    {
        const QString pluginName = m_pluginManager->getPluginName(plugin);

        const QString err = QObject::tr("L'étape \"%1\" n'a pas été trouvée dans le plugin \"%2\".").arg(stepKey).arg(pluginName);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, ((loadErrorStrategy != LES_Continue) ? CDM_ScriptProblem::TOP_NoSolution : CDM_ScriptProblem::TOP_StepNotFound), err, nullptr, parentStep, pluginIndex, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr) {
            scriptLoadCallBack()->loadScriptError(problem);
            step = problem.getSolutionUseStep();
        }

        stepKey = "";

        if(step != nullptr)
            stepKey = plugin->getKeyForStep(*step);
        else
            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);
    }

    return stepKey;
}

CT_VirtualAbstractStep* CDM_HRScriptXmlReader::createStepFromOriginal(const QString& stepKey, CT_VirtualAbstractStep* parentStep, CT_AbstractStepPlugin* plugin, LoadErrorStrategy& loadErrorStrategy) const
{
    CT_VirtualAbstractStep *originalStep = plugin->getStepFromKey(stepKey);
    CT_VirtualAbstractStep* copyStep = plugin->createNewInstanceOfStep(*originalStep, parentStep);

    if(copyStep == nullptr)
    {
        const int pluginIndex = m_pluginManager->getPluginIndex(plugin);
        const QString pluginName = m_pluginManager->getPluginName(plugin);

        const QString err = QObject::tr("Erreur lors de la copie de l'étape \"%1\" du plugin \"%2\".").arg(stepKey).arg(pluginName);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_NoSolution, err, copyStep, parentStep, pluginIndex, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);
    }

    return copyStep;
}

QDomElement CDM_HRScriptXmlReader::findDomElementForStep(const int &stepID, QDomElement parentElement)
{
    bool ok;
    QDomElement el = parentElement.firstChildElement("Step");

    while(!el.isNull()) {

        const int cStepID = el.attribute("id", "-1").toInt(&ok);

        if(ok && cStepID == stepID)
            return el;

        el = el.nextSiblingElement("Step");
    }

    return el;
}

CDM_HRScriptXmlReader::DomElementCallerInfo* CDM_HRScriptXmlReader::findDomElementForStepAndCreateInfo(const int &stepID, QDomElement parentElement)
{
    QDomElement el = findDomElementForStep(stepID, parentElement);

    if(!el.isNull()) {
        DomElementCallerInfo* info = new DomElementCallerInfo();
        info->element = el;

        return info;
    }

    return nullptr;
}

CDM_HRScriptXmlReader::DomElementCallerInfo* CDM_HRScriptXmlReader::findDomElementForCaller(const QObject *caller)
{
    if(!m_parametersStack.isEmpty() && dynamic_cast<const CT_VirtualAbstractStep*>(caller) != nullptr)
        return m_parametersStack.first();

    DomElementCallerInfo* elInfo = m_callerDomElements.value(const_cast<QObject*>(caller), nullptr);

    if(elInfo == nullptr) {

        if(!m_parametersStack.isEmpty()) {
            DomElementCallerInfo* parent = m_parametersStack.last();

            elInfo = new DomElementCallerInfo();

            const QString className = caller->metaObject()->className();

            if(parent->currentChilds.contains(className)) {
                QDomElement child = parent->currentChilds.value(className);

                if(!child.isNull()) {
                    child = child.nextSiblingElement(className);
                    parent->currentChilds.insert(className, child);
                }

                elInfo->element = child;
            } else {
                elInfo->element = parent->element.firstChildElement(className);
                parent->currentChilds.insert(className, elInfo->element);
            }

            m_callerDomElements.insert(const_cast<QObject*>(caller), elInfo);
        }
    }

    return elInfo;
}

QDomElement CDM_HRScriptXmlReader::getNextParameterOrFindCurrentAndAddItInInfo(CDM_HRScriptXmlReader::DomElementCallerInfo* info, const QString &parameterName, const int &uniqueID)
{
    QDomElement child;

    if(info == nullptr)
        return child;

    if(!info->currentParameters.contains(parameterName)) {
        child = info->element.firstChildElement(parameterName);

        while(!child.isNull() && (uniqueID != child.attribute("pUID", "-1").toInt()))
            child = child.nextSiblingElement(parameterName);

        info->currentParameters.insert(parameterName, child);
    } else {
        child = info->currentParameters.value(parameterName);

        if(!child.isNull()) {
            child = child.nextSiblingElement(parameterName);

            while(!child.isNull() && (uniqueID != child.attribute("pUID", "-1").toInt()))
                child = child.nextSiblingElement(parameterName);

            info->currentParameters.insert(parameterName, child);
        }
    }

    return child;
}

bool CDM_HRScriptXmlReader::processStepPreSettings(CT_VirtualAbstractStep* step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy)
{
    DomElementCallerInfo* info = nullptr;

    if(!m_domPreParameters.isNull())
        info = findDomElementForStepAndCreateInfo(stepID, m_domPreParameters);

    bool ok = false;

    m_allParametersFounded = true;
    mLastErrorAction.clear();

    if(info != nullptr) {
        m_parametersStack.append(info);
        ok = step->restorePreSettings(*this);
        qDeleteAll(m_parametersStack.begin(), m_parametersStack.end());
        m_parametersStack.clear();
    } else {
        ok = step->restorePreSettings(*this);
    }

    ok = ok && m_allParametersFounded;

    m_callerDomElements.clear();

    if(!ok) {
        const QString err = QObject::tr("Erreur lors de l'affectation des paramètres de pré-configuration à l'étape \"%1\" du plugin \"%2\". %3").arg(stepKey).arg(pluginName).arg(mLastErrorAction);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_StepPreParamsCanNotBeConfigured, err, step, parentStep, -1, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        if(!problem.isConfigureStepWellConfigured())
            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);

        return false;
    }

    return true;
}

bool CDM_HRScriptXmlReader::processStepInputSettings(CT_VirtualAbstractStep *step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy)
{    
    DomElementCallerInfo* info = nullptr;

    if(!m_domInputModels.isNull())
        info = findDomElementForStepAndCreateInfo(stepID, m_domInputModels);

    bool ok = false;

    m_allParametersFounded = true;
    mLastErrorAction.clear();

    if(info != nullptr) {
        m_parametersStack.append(info);
        ok = step->restoreInputSettings(*this);
        qDeleteAll(m_parametersStack.begin(), m_parametersStack.end());
        m_parametersStack.clear();
    } else {
        ok = step->restoreInputSettings(*this);
    }

    ok = ok && m_allParametersFounded;

    m_callerDomElements.clear();

    if(!ok) {
        const QString err = QObject::tr("Erreur lors de l'affectation des paramètres d'entrée à l'étape \"%1\" du plugin \"%2\". %3").arg(stepKey).arg(pluginName).arg(mLastErrorAction);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_StepInputParamsCanNotBeConfigured, err, step, parentStep, -1, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        if(!problem.isConfigureStepWellConfigured())
            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);

        return false;
    }

    return true;
}

bool CDM_HRScriptXmlReader::processStepPostSettings(CT_VirtualAbstractStep *step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy)
{
    DomElementCallerInfo* info = nullptr;

    if(!m_domPostParameters.isNull())
        info = findDomElementForStepAndCreateInfo(stepID, m_domPostParameters);

    bool ok = false;

    m_allParametersFounded = true;
    mLastErrorAction.clear();

    if(info != nullptr) {
        m_parametersStack.append(info);
        ok = step->restorePostSettings(*this);
        qDeleteAll(m_parametersStack.begin(), m_parametersStack.end());
        m_parametersStack.clear();
    } else {
        ok = step->restorePostSettings(*this);
    }

    ok = ok && m_allParametersFounded;

    m_callerDomElements.clear();

    if(!ok) {
        const QString err = QObject::tr("Erreur lors de l'affectation des paramètres de post-configuration à l'étape \"%1\" du plugin \"%2\". %3").arg(stepKey).arg(pluginName).arg(mLastErrorAction);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_StepPostParamsCanNotBeConfigured, err, step, parentStep, -1, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        if(!problem.isConfigureStepWellConfigured())
            loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);

        return false;
    }

    return true;
}

bool CDM_HRScriptXmlReader::processStepOthersSettings(CT_VirtualAbstractStep *step, const int &stepID, const QString& stepKey, CT_VirtualAbstractStep* parentStep, const QString& pluginName, LoadErrorStrategy& loadErrorStrategy)
{
    if(m_domOthersParameters.isNull())
        return true;

    DomElementCallerInfo* info = findDomElementForStepAndCreateInfo(stepID, m_domOthersParameters);

    bool ok = false;

    mLastErrorAction.clear();

    if(info != nullptr) {
        m_parametersStack.append(info);
        ok = step->restoreOthersSettings(*this);
        qDeleteAll(m_parametersStack.begin(), m_parametersStack.end());
        m_parametersStack.clear();
    } else {
        ok = step->restoreOthersSettings(*this);
    }

    m_callerDomElements.clear();

    if(!ok) {
        const QString err = QObject::tr("Erreur lors de l'affectation d'autres paramètres à l'étape \"%1\" du plugin \"%2\". %3").arg(stepKey).arg(pluginName).arg(mLastErrorAction);

        m_errors += err + QObject::tr("\r\n");

        CDM_ScriptProblem problem(*m_pluginManager, CDM_ScriptProblem::TOP_NoSolution, err, step, parentStep, -1, pluginName, stepKey);

        if(scriptLoadCallBack() != nullptr)
            scriptLoadCallBack()->loadScriptError(problem);

        loadErrorStrategy = (problem.getSolutionKeepSteps() ? LES_StopAndKeep : LES_StopAndClear);
    }

    return ok;
}
