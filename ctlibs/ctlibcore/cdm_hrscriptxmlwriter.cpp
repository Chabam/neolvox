#include "cdm_hrscriptxmlwriter.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>

#include "ct_abstractstepplugin.h"

#include "cdm_pluginmanager.h"
#include "cdm_xmltools.h"

CDM_HRScriptXMLWriter::CDM_HRScriptXMLWriter()
{
    m_error = false;
    m_pluginManager = nullptr;
}

void CDM_HRScriptXMLWriter::setPluginManager(const CDM_PluginManager *pm)
{
    m_pluginManager = (CDM_PluginManager*)pm;
}

void CDM_HRScriptXMLWriter::setFilePath(const QString &absoluteFilePath)
{
    m_filePath = absoluteFilePath;
}

void CDM_HRScriptXMLWriter::setWorkflow(const QList<CT_VirtualAbstractStep *> &rootSteps)
{
    m_rootSteps = rootSteps;
}

void CDM_HRScriptXMLWriter::writeAll()
{
    const bool cryptPre = true;
    const bool cryptOthers = true;
    const bool cryptInputs = true;

    m_error = false;

    QFile f(m_filePath);

    if(f.open(QIODevice::WriteOnly))
    {
        m_parameters.clear();

        m_document = QDomDocument();

        m_domRoot = m_document.createElement("CompuTree");
        m_domRoot.setAttribute("scriptVersion", 1);

        m_domPlugins = m_document.createElement("Plugins");

        m_domPrePath = m_document.createElement("PrePath");

        m_domPreParameters = m_document.createElement("PreParameters");
        m_domExtraParameters = m_document.createElement("ExtraParameters");
        m_domInputModels = m_document.createElement("InputModels");
        m_domPostParameters = m_document.createElement("PostParameters");
        m_domOthersParameters = m_document.createElement("OthersParameters");
        m_domWorkflow = m_document.createElement("Workflow");

        m_document.appendChild(m_domRoot);

        QSet<CT_AbstractStepPlugin*> pluginsUsed;

        int stepID = 1;

        foreach (CT_VirtualAbstractStep* step, m_rootSteps) {
            recursiveProcessStep(step, m_domWorkflow, stepID, pluginsUsed);
        }

        fillDomPlugins(pluginsUsed);
        fillPrePath();

        if(m_domPlugins.hasAttributes() || m_domPlugins.hasChildNodes()) {
            m_domRoot.appendChild(m_document.createComment(QObject::tr("(optionnel) Plugins utilisés (seulement à titre indicatif pour l'utilisateur). La section est ignorée à la lecture !")));
            m_domRoot.appendChild(m_domPlugins);
        }

        if(m_domPrePath.hasChildNodes()) {
            m_domRoot.appendChild(m_document.createComment(QObject::tr("(optionnel) Pré-chemins. Utilisés lors de la lecture des paramètres du type chemin (Path) pour compléter les chemins relatifs.")));
            m_domRoot.appendChild(m_domPrePath);
        }

        if(m_domPostParameters.hasChildNodes()) {
            m_domRoot.appendChild(m_document.createComment(QObject::tr("(obligatoire) Paramètres de chaque étape modifiables par l'utilisateur.")));
            m_domRoot.appendChild(m_domPostParameters);
        }

        if(m_domExtraParameters.hasChildNodes()) {
            m_domRoot.appendChild(m_document.createComment(QObject::tr("(optionnel) Choix multiples des paramètres (seulement à titre indicatif pour l'utilisateur). Certains paramètres peuvent avoir la valeur de l'un des choix ci-dessous. La section est ignorée à la lecture !")));
            m_domRoot.appendChild(m_domExtraParameters);
        }

        m_domRoot.appendChild(m_document.createComment(QObject::tr("(obligatoire) Flux des étapes. Contient l'enchainement des étapes ainsi que les informations permettant de re-créer les étapes du bon type et charger leurs paramètres.")));
        m_domRoot.appendChild(m_domWorkflow);

        if(!cryptPre)
            m_domRoot.appendChild(m_domPreParameters);

        if(!cryptOthers)
            m_domRoot.appendChild(m_domOthersParameters);

        if(!cryptInputs)
            m_domRoot.appendChild(m_domInputModels);

        QList<QDomElement> elementsToCrypt;

        if(cryptPre)
            elementsToCrypt << m_domPreParameters;

        if(cryptOthers)
            elementsToCrypt << m_domOthersParameters;

        if(cryptInputs)
            elementsToCrypt << m_domInputModels;

        QDomElement notModifiableEL = m_document.createElement("NotModifiable");
        notModifiableEL.appendChild(m_document.createTextNode(cryptElements(elementsToCrypt)));

        m_domRoot.appendChild(m_document.createComment(QObject::tr("Paramètres non modifiable. Contient des paramètres non modifiables par l'utilisateur. Si cette section n'est pas présente chaque étape devra être configurée manuellement.")));
        m_domRoot.appendChild(notModifiableEL);

        QTextStream stream(&f);
        stream.setCodec("UTF-8");
        stream << m_document.toString();

        f.close();
        return;
    }

    m_error = true;
}

bool CDM_HRScriptXMLWriter::hasErrors() const
{
    return false;
}

void CDM_HRScriptXMLWriter::setCallerDescription(const QObject *caller, const QString &description)
{
    if(description.isEmpty())
        return;

    QDomElement el = getOrCreateDomElementForCaller(caller);
    el.setAttribute("description", CDM_XMLTools::encodeEntities(description));
}

void CDM_HRScriptXMLWriter::setCallerUniqueID(const QObject *caller, const int &uniqueID)
{
    if(uniqueID == -1)
        return;

    QDomElement el = getOrCreateDomElementForCaller(caller);
    el.setAttribute("uid", uniqueID);
}

int CDM_HRScriptXMLWriter::addParameter(const QObject *caller,
                                        const QString &parameterName,
                                        const QVariant &parameterValue,
                                        const QString &description,
                                        const int &uniqueID)
{
    return genericAddParameter(caller, parameterName, "value", parameterValue, description, uniqueID);
}

int CDM_HRScriptXMLWriter::addParameterPath(const QObject *caller,
                                            const QString &parameterName,
                                            const QString &fileOrDirectoryPath,
                                            const QString &description,
                                            const int &uniqueID)
{
    int prePathID = -1;
    bool isAFolder = true;
    QString relativePath = convertPathToRelativePathAndAddToPrePath(fileOrDirectoryPath, prePathID, isAFolder);

    if(relativePath.isEmpty() && !isAFolder)
        return -1;

    const int id = genericAddParameter(caller, parameterName, "", "", description, uniqueID);

    if(id <= 0)
        return id;

    QDomElement parameter = m_parameters.value(id);
    parameter.setAttribute("prePathID", prePathID);
    parameter.appendChild(m_document.createTextNode(relativePath));

    return id;
}

bool CDM_HRScriptXMLWriter::addParameterInfo(const QObject *caller,
                                             const int &parameterUniqueID,
                                             const QString &extraInfoName,
                                             const QVariant &extraInfoValue)
{
    Q_UNUSED(caller)

    QDomElement parameter = m_parameters.value(parameterUniqueID);

    if(parameter.isNull() || parameter.hasAttribute(extraInfoName))
        return false;

    parameter.setAttribute(extraInfoName, CDM_XMLTools::encodeEntities(extraInfoValue.toString()));

    return true;
}

void CDM_HRScriptXMLWriter::beginWriteChildrens(const QObject *caller)
{
    m_parametersStack.push(getOrCreateDomElementForCaller(caller));
}

void CDM_HRScriptXMLWriter::endWriteChildrens(const QObject *caller)
{
    Q_UNUSED(caller)

    m_parametersStack.pop();
}

int CDM_HRScriptXMLWriter::createExtraParametersGroup(const QObject *caller, const QString &valuesName, const QString &groupDescription)
{
    if(valuesName.isEmpty())
        return 0;

    const int id = m_extraParametersGroup.isEmpty() ? 1 : (m_extraParametersGroup.lastKey()+1);

    QDomElement extraParameterGroup = m_document.createElement(caller->metaObject()->className());

    if(!groupDescription.isEmpty())
        extraParameterGroup.setAttribute("description", CDM_XMLTools::encodeEntities(groupDescription));

    extraParameterGroup.setAttribute("epID", id);

    ParameterGroupInfo info;
    info.element = extraParameterGroup;
    info.valuesName = valuesName;

    m_extraParametersGroup.insert(id, info);

    return id;

}

bool CDM_HRScriptXMLWriter::linkParameterAndExtraParametersGroup(const QObject *caller, const int &parameterUniqueID, const int &parameterValuesGroupID)
{
    Q_UNUSED(caller)

    ParameterGroupInfo info = m_extraParametersGroup.value(parameterValuesGroupID);

    if(info.element.isNull())
        return false;

    QDomElement parameter = m_parameters.value(parameterUniqueID);

    if(parameter.isNull() || parameter.hasAttribute("epID"))
        return false;

    parameter.setAttribute("epID", parameterValuesGroupID);

    return true;
}

bool CDM_HRScriptXMLWriter::addExtraParametersValueToGroup(const QObject *caller, const QVariant &value, const int &parameterValuesGroupID, const QString &description)
{
    Q_UNUSED(caller)

    ParameterGroupInfo info = m_extraParametersGroup.value(parameterValuesGroupID);

    if(info.element.isNull())
        return false;

    QDomElement el = m_document.createElement(info.valuesName);
    el.setAttribute("value", CDM_XMLTools::encodeEntities(value.toString()));

    if(!description.isEmpty())
        el.setAttribute("description", CDM_XMLTools::encodeEntities(description));

    info.element.appendChild(el);

    return true;
}

int CDM_HRScriptXMLWriter::genericAddParameter(const QObject *caller, const QString &parameterName, const QString &parameterValueName, const QVariant &parameterValue, const QString &description, const int &uniqueID)
{
    const int id = m_parameters.isEmpty() ? 1 : (m_parameters.lastKey()+1);

    QDomElement el = getOrCreateDomElementForCaller(caller);
    QDomElement parameter = m_document.createElement(parameterName);
    el.appendChild(parameter);

    if(!parameterValueName.isEmpty())
        parameter.setAttribute(parameterValueName, CDM_XMLTools::encodeEntities(parameterValue.toString()));

    if(!description.isEmpty())
        parameter.setAttribute("pDescription", CDM_XMLTools::encodeEntities(description));

    if(uniqueID != -1)
        parameter.setAttribute("pUID", uniqueID);

    m_parameters.insert(id, parameter);

    return id;
}

void CDM_HRScriptXMLWriter::recursiveProcessStep(CT_VirtualAbstractStep* step, QDomElement &parentDom, int& stepID, QSet<CT_AbstractStepPlugin*>& pluginsUsed)
{
    pluginsUsed.insert(step->pluginStaticCastT<>());

    processStepPreSettings(step, stepID);
    processStepInputSettings(step, stepID);
    processStepPostSettings(step, stepID);
    processStepOthersSettings(step, stepID);

    QDomElement stepEL = createDomElementForStep(step, stepID);
    stepEL.setAttribute("debug", step->isDebugModeOn() ? "true" : "false");

    ++stepID;

    step->visitChildrens([this, &stepEL, &stepID, &pluginsUsed](const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep* child) -> bool {
        this->recursiveProcessStep(const_cast<CT_VirtualAbstractStep*>(child), stepEL, stepID, pluginsUsed);
        return true;
    });

    parentDom.appendChild(stepEL);
}

void CDM_HRScriptXMLWriter::processStepPreSettings(CT_VirtualAbstractStep *step, const int &stepID)
{
    QDomElement stepEL = createDomElementForStep(step, stepID);
    m_parametersStack.push(stepEL);
    step->savePreSettings(*this);
    m_parametersStack.clear();
    m_callerDomElements.clear();

    if(stepEL.hasChildNodes())
        m_domPreParameters.appendChild(stepEL);
}

void CDM_HRScriptXMLWriter::processStepInputSettings(CT_VirtualAbstractStep *step, const int &stepID)
{
    QDomElement stepEL = createDomElementForStep(step, stepID);
    m_parametersStack.push(stepEL);
    step->saveInputSettings(*this);
    m_parametersStack.clear();
    m_callerDomElements.clear();

    if(stepEL.hasChildNodes())
        m_domInputModels.appendChild(stepEL);
}

void CDM_HRScriptXMLWriter::processStepPostSettings(CT_VirtualAbstractStep *step, const int &stepID)
{
    m_extraParametersGroup.clear();

    QDomElement stepEL = createDomElementForStep(step, stepID);
    m_parametersStack.push(stepEL);
    step->savePostSettings(*this);
    m_parametersStack.clear();
    m_callerDomElements.clear();

    if(stepEL.hasChildNodes())
        m_domPostParameters.appendChild(stepEL);

    // only post settings can add extra parameters
    QDomElement extraStepEL = createDomElementForStep(step, stepID);

    QMapIterator<int, ParameterGroupInfo> it(m_extraParametersGroup);

    while(it.hasNext()) {
        it.next();

        if(it.value().element.hasChildNodes())
            extraStepEL.appendChild(it.value().element);
    }

    if(extraStepEL.hasChildNodes())
        m_domExtraParameters.appendChild(extraStepEL);

    m_extraParametersGroup.clear();
}

void CDM_HRScriptXMLWriter::processStepOthersSettings(CT_VirtualAbstractStep *step, const int &stepID)
{
    QDomElement stepEL = createDomElementForStep(step, stepID);
    m_parametersStack.push(stepEL);
    step->saveOthersSettings(*this);
    m_parametersStack.clear();
    m_callerDomElements.clear();

    if(stepEL.hasChildNodes())
        m_domOthersParameters.appendChild(stepEL);
}

QString CDM_HRScriptXMLWriter::convertPathToRelativePathAndAddToPrePath(QString absolutePath, int& prePathID, bool& isAFolder)
{
    isAFolder = true;

    if(absolutePath.isEmpty())
        return QString();

    QString relativePath;

    QFileInfo f(absolutePath);
    relativePath = f.fileName();
    absolutePath = f.absolutePath();

    isAFolder = !f.isFile();

    if(isAFolder && relativePath.isEmpty())
        relativePath = ".";

    prePathID = m_prePath.indexOf(absolutePath);

    if(prePathID == -1) {
        prePathID = m_prePath.size();
        m_prePath.append(absolutePath);
    }

    return relativePath;
}

QDomElement CDM_HRScriptXMLWriter::getOrCreateDomElementForCaller(const QObject *caller)
{
    if(!m_parametersStack.isEmpty() && dynamic_cast<const CT_VirtualAbstractStep*>(caller) != nullptr)
        return m_parametersStack.first();

    QDomElement el = m_callerDomElements.value((QObject*)caller, QDomElement());

    if(el.isNull()) {

        QDomElement parent = m_parametersStack.last();

        el = m_document.createElement(caller->metaObject()->className());
        m_callerDomElements.insert((QObject*)caller, el);

        parent.appendChild(el);
    }

    return el;
}

QDomElement CDM_HRScriptXMLWriter::createDomElementForStep(CT_VirtualAbstractStep *step, int stepID)
{
    QDomElement stepEL = m_document.createElement("Step");
    stepEL.setAttribute("plugin", CDM_XMLTools::encodeEntities(m_pluginManager->getPluginName(step->pluginStaticCastT<>())));
    stepEL.setAttribute("type", CDM_XMLTools::encodeEntities(step->pluginStaticCastT<>()->getKeyForStep(*step)));
    stepEL.setAttribute("id", stepID);

    return stepEL;
}

void CDM_HRScriptXMLWriter::fillDomPlugins(const QSet<CT_AbstractStepPlugin *> &plugins)
{
    QString pluginValues;

    foreach (CT_AbstractStepPlugin* plugin, plugins) {
        pluginValues += m_pluginManager->getPluginName(plugin) + " ";
    }

    if(!pluginValues.isEmpty())
        pluginValues.remove(pluginValues.size()-1, 1);

    if(!pluginValues.isEmpty())
        m_domPlugins.setAttribute("values", CDM_XMLTools::encodeEntities(pluginValues));
}

void CDM_HRScriptXMLWriter::fillPrePath()
{
    int i = 0;

    foreach (const QString& prePath, m_prePath) {
        QDomElement pathEL = m_document.createElement("Path");
        pathEL.setAttribute("prePathID", i++);
        pathEL.appendChild(m_document.createTextNode(prePath));

        m_domPrePath.appendChild(pathEL);
    }
}

QString CDM_HRScriptXMLWriter::cryptElements(const QList<QDomElement> &toCrypt) const
{
    QByteArray finalArray;
    QTextStream stream(&finalArray);

    foreach(QDomElement el, toCrypt) {
        stream << el;
    }

    stream.flush();

    return QString::fromStdString(finalArray.toBase64().toStdString());
}
