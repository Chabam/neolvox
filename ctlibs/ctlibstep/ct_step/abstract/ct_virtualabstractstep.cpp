/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#include "ct_virtualabstractstep.h"

#include "ct_result/abstract/ct_abstractresult.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawable.h"
#include "ct_itemattributes/abstract/ct_abstractitemattribute.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"

#include "ct_view/tools/ct_configurablewidgettodialog.h"
#include "ct_risformat/ct_parseris.h"

#include <QWaitCondition>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QUrl>
#include <QDir>
//#include<QTimer>

int CT_VirtualAbstractStep::CURRENT_ID = 1;
QString CT_VirtualAbstractStep::CURRENT_LANGAGE = "en";

CT_VirtualAbstractStep::CT_VirtualAbstractStep() :
// Correction AP 10/09/2021 : pour que les numéros d'étapes commencent à 1 dans le script
//    m_uniqueId(CURRENT_ID++),
    m_uniqueId(0),
    m_parentStep(nullptr),
    m_plugin(nullptr),
    m_uniqueIndexGenerator(new CT_UniqueIndexGenerator()),
    m_preInputConfigDialog(nullptr),
    m_postInputConfigDialog(nullptr),
    m_isRunning(false),
    m_mustStop(false),
    m_currentErrorCode(0),
    m_elapsedTimeInMs(0),
    m_currentProgressValue(0),
    m_currentClearMemoryProgressValue(0),
    m_isSettingsModified(true),
    m_isDebuggable(false),
    m_isDebugModeOn(false),
    m_hasReceivedAckForDebugMode(false),
    m_currentBreakPointNumber(0),
    m_nBreakPointToJumpSetInAckDebugMode(0),
    m_nextBreakPointNumberToStop(0),
    m_isModifiable(false),
    m_isManual(false),
    m_isTheFirstCallToAckManualMode(true),
    m_mustQuitManualMode(false),
    m_currentGuiContext(nullptr),
    m_isAPrototype(false)

{
    m_stepTool.m_pointer = this;

    connect(&m_outputManager, SIGNAL(resultAdded(const CT_AbstractResult*)), this, SIGNAL(resultAdded(const CT_AbstractResult*)), Qt::DirectConnection);
}

CT_VirtualAbstractStep::~CT_VirtualAbstractStep()
{
    UniqueIndexGeneratorPtr uig = uniqueIndexGenerator();

    if(uig != nullptr)
        uig->objectWillBeDeleted(this);

    qDeleteAll(m_childrens);
    m_childrens.clear();

    clearOutResult();

    delete m_preInputConfigDialog;
    m_preInputConfigDialog = nullptr;

    delete m_postInputConfigDialog;
    m_postInputConfigDialog = nullptr;

    delete m_uniqueIndexGenerator;
    m_uniqueIndexGenerator = nullptr;
}

IStepToolForModel* CT_VirtualAbstractStep::stepToolForModel() const
{
    return &const_cast<StepToolForModel&>(m_stepTool);
}

int CT_VirtualAbstractStep::uniqueID() const
{
    return m_uniqueId;
}

QString CT_VirtualAbstractStep::toolTip() const
{
    return description();
}

QString CT_VirtualAbstractStep::name() const
{
    return metaObject()->className();
}

QString CT_VirtualAbstractStep::extendedName() const
{
    return name() + " (" + QString().setNum(uniqueID()) + ")";
}

QString CT_VirtualAbstractStep::displayableName() const
{
    return name();
}

QString CT_VirtualAbstractStep::displayableExtendedName() const
{
    return displayableName() + " (" + QString().setNum(uniqueID()) + ")";
}

QString CT_VirtualAbstractStep::displayableCustomName() const
{
    if(m_displayableCustomName.isEmpty())
        return displayableExtendedName();

    return m_displayableCustomName;
}

void CT_VirtualAbstractStep::setDisplayableCustomName(const QString& customName)
{
    if(m_displayableCustomName != customName) {
        m_displayableCustomName = customName;
        emit displayableCustomNameChanged();
    }
}

QString CT_VirtualAbstractStep::description() const
{
    return tr("No description for this step");
}

QString CT_VirtualAbstractStep::detailledDescription() const
{
    return tr("No detailled description for this step");
}

QString CT_VirtualAbstractStep::URL() const
{
    return plugin()->pluginToolForStep()->url();
}

QStringList CT_VirtualAbstractStep::getStepRISCitations() const
{
    return QStringList();
}

QString CT_VirtualAbstractStep::parametersDescription(bool addDescriptions)
{
    QString str;

    if (m_preInputConfigDialog == nullptr) {this->createPreInputConfigurationDialog();}
    if (m_preInputConfigDialog != nullptr)
    {
       str.append(tr("<strong>Paramètres de pré-configuration</strong> (non modifiables une fois l'étape ajoutée) :<br><br>"));
       str.append(m_preInputConfigDialog->helpText(addDescriptions));
    }
    delete m_preInputConfigDialog;
    m_preInputConfigDialog = nullptr;


    if (m_postInputConfigDialog == nullptr) {this->createPostInputConfigurationDialog();}
    if (m_postInputConfigDialog != nullptr)
    {
        if (m_preInputConfigDialog != nullptr) {str.append("<br>");}

        str.append(tr("<strong>Paramètres de l'étape</strong> :<br><br>"));
        str.append(m_postInputConfigDialog->helpText(addDescriptions));
    }

    delete m_postInputConfigDialog;
    m_postInputConfigDialog = nullptr;

    return str;
}

QString CT_VirtualAbstractStep::inputDescription() const
{
    QString str;
    int nbTab = 0;
    this->visitInResultModels([this, &str, &nbTab](const CT_InAbstractResultModel* child) -> bool {
        this->recursiveCreateHelpStrForResultModel(str, nbTab, child);
        return true;
    });

    if (!str.isEmpty())
    {
        str = tr("Structure des données d'entrée recherchées :<br>") + str;
    }
    return str;
}

QString CT_VirtualAbstractStep::outputDescription() const
{
    QString str;
    int nbTab = 0;
    this->visitOutResultModels([this, &str, &nbTab](const CT_OutAbstractResultModel* child) -> bool {
        this->recursiveCreateHelpStrForResultModel(str, nbTab, child);
        return true;
    });

    if (!str.isEmpty())
    {
        str = tr("Données en sortie (nouvelles données en <strong>gras</strong>) :<br>") + str;
    }

    return str;
}

QString CT_VirtualAbstractStep::detailsDescription() const
{
    return "";
}

void CT_VirtualAbstractStep::setPlugin(const IPluginForStep* plugin)
{
    m_plugin = const_cast<IPluginForStep*>(plugin);
}

IPluginForStep* CT_VirtualAbstractStep::plugin() const
{
    return m_plugin;
}

void CT_VirtualAbstractStep::setIsAPrototype()
{
    m_isAPrototype = true;
}

bool CT_VirtualAbstractStep::isAPrototype() const
{
    return m_isAPrototype;
}

void CT_VirtualAbstractStep::init()
{
    if(isAPrototype()) {
        // to show input models
        internalDeclareInputModels();

        // to show a default out model :
        finalizeConfiguration(); // create output models
    }
    // Correction AP 10/09/2021 : pour que les numéros d'étapes commencent à 1 dans le script
    else {
        m_uniqueId = CURRENT_ID++;
    }
}

bool CT_VirtualAbstractStep::showPreInputConfigurationDialog()
{
    createPreInputConfigurationDialog();

    if(preInputConfigure()) {
        finalizePreSettings();
        return true;
    }

    return false;
}

bool CT_VirtualAbstractStep::showInputResultConfigurationDialog()
{
    //const bool firstTime = mustDeclareInputModels();

    return configureInputs(hasChildrens() /*!firstTime*/);
}

bool CT_VirtualAbstractStep::showPostConfigurationDialog()
{
    createPostInputConfigurationDialog();

    if(postInputConfigure()) {
        finalizePostSettings();
        return true;
    }

    return false;
}

bool CT_VirtualAbstractStep::canFinalizeConfiguration(CT_OutManager::CreateOutputModelsErrorType* errorType, QString* why) const
{
    return m_outputManager.canCreateOutputModels(*this, errorType, why);
}

bool CT_VirtualAbstractStep::finalizeConfiguration()
{
    // now
    //     * settings of step has been restored
    //  or * showPostConfigurationDialog has been called
    // so we can finalize the configuration by creating all output models
    return m_outputManager.createOutputModels(*this);
}

bool CT_VirtualAbstractStep::isSettingsModified() const
{
    return m_isSettingsModified;
}

bool CT_VirtualAbstractStep::acceptAddAfterThisStep(const CT_VirtualAbstractStep* step)
{
    return m_inputManager.wereAllInputsFoundInOutputsOfThisStepOrRecursively(step);
}

bool CT_VirtualAbstractStep::appendStep(CT_VirtualAbstractStep* step)
{
    Q_ASSERT(step->parentStep() == this);
    if (step->parentStep() != this) {qDebug() << "CT_VirtualAbstractStep::appendStep" << ", " <<  "step->parentStep() != this";}

    if(!step->acceptAddAfterThisStep(this))
        return false;

    m_childrens.append(step);

    return true;
}

bool CT_VirtualAbstractStep::removeStep(CT_VirtualAbstractStep* step)
{
    if(!m_childrens.removeOne(step))
        return false;

    delete step;
    return true;
}

bool CT_VirtualAbstractStep::visitChildrens(const CT_VirtualAbstractStep::ChildVisitor& visitor) const
{
    for(const CT_VirtualAbstractStep* step : m_childrens) {
        if(!visitor(this, step))
            return false;
    }

    return true;
}

bool CT_VirtualAbstractStep::recursiveVisitChildrens(const CT_VirtualAbstractStep::ChildVisitor& visitor) const
{
    if(m_childrens.isEmpty())
        return true;

    return visitChildrens([&visitor](const CT_VirtualAbstractStep* parent, const CT_VirtualAbstractStep* step) -> bool {

        if(!visitor(parent, step))
            return false;

        return step->recursiveVisitChildrens(visitor);
    });
}

bool CT_VirtualAbstractStep::hasChildrens() const
{
    return !m_childrens.isEmpty();
}

CT_VirtualAbstractStep* CT_VirtualAbstractStep::parentStep() const
{
    return m_parentStep;
}

CT_VirtualAbstractStep* CT_VirtualAbstractStep::rootStep() const
{
    CT_VirtualAbstractStep* parentStep = const_cast<CT_VirtualAbstractStep*>(this);

    while(parentStep->parentStep() != nullptr)
        parentStep = parentStep->parentStep();

    return parentStep;
}

bool CT_VirtualAbstractStep::needInputResults() const
{
    return m_inputManager.needInputs();
}

bool CT_VirtualAbstractStep::visitInResultModels(const CT_VirtualAbstractStep::InResultModelVisitor& visitor) const
{
    return m_inputManager.visitResultModels(visitor);
}

bool CT_VirtualAbstractStep::visitOutResultModels(const CT_VirtualAbstractStep::OutResultModelVisitor& visitor) const
{
    return m_outputManager.visitResultModels(visitor);
}

bool CT_VirtualAbstractStep::visitOutResults(const CT_VirtualAbstractStep::OutResultVisitor& visitor) const
{
    return m_outputManager.visitResults(visitor);
}

const CT_AbstractResult* CT_VirtualAbstractStep::outResultAt(const int& index) const
{
    return m_outputManager.resultAt(index);
}

int CT_VirtualAbstractStep::nOutResult() const
{
    return m_outputManager.nResult();
}

void CT_VirtualAbstractStep::clearOutResult()
{
    setClearMemoryProgress(0);

    const int size = nOutResult();

    for(int i=0; i<size; ++i) {
        removeOutResult(0);
        setClearMemoryProgress(((i+1)*100)/size);
    }

    setClearMemoryProgress(100);
}

void CT_VirtualAbstractStep::clearOutResultFromMemory()
{
    setClearMemoryProgress(0);

    const int size = nOutResult();
    int index = 0;

    m_outputManager.visitResults([this, &size, &index](const CT_AbstractResult* result) -> bool {
        emit this->resultToBeClearedFromMemory(result);

        CT_AbstractResult* r = const_cast<CT_AbstractResult*>(result);

        r->setBusy(true);
        r->clearFromMemory();
        r->setBusy(false);

        this->setClearMemoryProgress(((++index)*100)/size);

        return true;
    });

    setClearMemoryProgress(100);
}

void CT_VirtualAbstractStep::execute()
{
    runProcessing();
}

void CT_VirtualAbstractStep::stop()
{
    m_mustStop = true;

    emit stopped();
}

bool CT_VirtualAbstractStep::isRunning() const
{
    return m_isRunning;
}

bool CT_VirtualAbstractStep::isStopped() const
{
    return m_mustStop;
}

int CT_VirtualAbstractStep::executeTime() const
{
    return m_elapsedTimeInMs;
}

int CT_VirtualAbstractStep::progressValue() const
{
    return m_currentProgressValue;
}

int CT_VirtualAbstractStep::errorCode() const
{
    return m_currentErrorCode;
}

QString CT_VirtualAbstractStep::errorMessage(int errorCode) const
{
    if(errorCode != 0)
    {
        const QString errMess = m_errorMessagesByErrorCode.value(errorCode, QString());

        if(!errMess.isEmpty())
            return errMess;

        return tr("Erreur %1").arg(errorCode);
    }

    return tr("Aucune erreur détecté");
}

bool CT_VirtualAbstractStep::mustBeRestarted() const
{
    return (isSettingsModified()
            || (nOutResult() == 0)
            || ((nOutResult() > 0)
                && (m_outputManager.resultAt(0)->isClearedFromMemory())));
}

CT_VirtualAbstractStep* CT_VirtualAbstractStep::restartComputeFromStep() const
{
    return nullptr;
}

void CT_VirtualAbstractStep::savePreSettings(SettingsWriterInterface &writer) const
{
    if(m_preInputConfigDialog != nullptr)
        m_preInputConfigDialog->saveSettings(writer);
}

void CT_VirtualAbstractStep::saveInputSettings(SettingsWriterInterface &writer) const
{
    m_inputManager.saveSettings(writer);
}

void CT_VirtualAbstractStep::savePostSettings(SettingsWriterInterface &writer) const
{
    if(m_postInputConfigDialog != nullptr)
        m_postInputConfigDialog->saveSettings(writer);
}

void CT_VirtualAbstractStep::saveOthersSettings(SettingsWriterInterface &writer) const
{
    if(!m_displayableCustomName.isEmpty())
        writer.addParameter(this, "CustomName", m_displayableCustomName);
}

bool CT_VirtualAbstractStep::restorePreSettings(SettingsReaderInterface &reader)
{
    createPreInputConfigurationDialog();

    bool ok = true;

    if(m_preInputConfigDialog != nullptr)
        ok = m_preInputConfigDialog->restoreSettings(reader);

    if(ok)
        finalizePreSettings();

    return ok;
}

bool CT_VirtualAbstractStep::restoreInputSettings(SettingsReaderInterface &reader)
{
    // crée les résultats modèles d'entrée après avoir initialisé les paramètres des étapes
    // puisque certaines etapes modifie leur inResultModel après la pré-configuration !
    if(!internalDeclareInputModels())
        return false;

    return m_inputManager.restoreSettings(reader, this->getHelpPageForStepPath());
}

bool CT_VirtualAbstractStep::restorePostSettings(SettingsReaderInterface &reader)
{
    createPostInputConfigurationDialog();

    bool ok = true;
    // pre-configuration
    if(m_postInputConfigDialog != nullptr)
        ok = m_postInputConfigDialog->restoreSettings(reader);

    if(ok)
        finalizePostSettings();

    return ok;
}

bool CT_VirtualAbstractStep::restoreOthersSettings(SettingsReaderInterface &reader)
{
    QVariant customName;

    if(reader.parameter(this, "CustomName", customName))
        m_displayableCustomName = customName.toString();

    return true;
}

void CT_VirtualAbstractStep::aboutToBeDeleted()
{
    delete m_preInputConfigDialog;
    m_preInputConfigDialog = nullptr;

    delete m_postInputConfigDialog;
    m_postInputConfigDialog = nullptr;

    m_inputManager.aboutToBeDeleted();
}

void CT_VirtualAbstractStep::setGuiContext(const GuiContextInterface *context)
{
    m_currentGuiContext = const_cast<GuiContextInterface*>(context);
}

GuiContextInterface* CT_VirtualAbstractStep::guiContext() const
{
    return m_currentGuiContext;
}

bool CT_VirtualAbstractStep::isDebuggable() const
{
    return m_isDebuggable;
}

bool CT_VirtualAbstractStep::setDebugModeOn(bool enable)
{
    if(enable && !isDebuggable())
        return false;

    if(!isRunning())
        m_isDebugModeOn = enable;
    else if(!enable)
        m_nextBreakPointNumberToStop = -1;
    else
        return false;

    return true;
}

bool CT_VirtualAbstractStep::isDebugModeOn() const
{
    return m_isDebugModeOn;
}

void CT_VirtualAbstractStep::ackDebugMode(int nBreakPointToJump)
{
    nBreakPointToJump = qMax(1, nBreakPointToJump);
    m_nBreakPointToJumpSetInAckDebugMode = nBreakPointToJump; // store the value, for it to be accessible in the step (see CT_StepEndLoop for use exemple)
    m_nextBreakPointNumberToStop += nBreakPointToJump;
    m_hasReceivedAckForDebugMode = true;
}

bool CT_VirtualAbstractStep::isManual() const
{
    return m_isManual;
}

bool CT_VirtualAbstractStep::isInManualMode() const
{
    return !m_mustQuitManualMode;
}

void CT_VirtualAbstractStep::ackManualMode()
{
    if(m_isTheFirstCallToAckManualMode)
        STEP_LOG->addInfoMessage(displayableCustomName() + tr(" enter manual mode"));

    if(m_isTheFirstCallToAckManualMode
            && guiContext() != nullptr)
    {
        initManualMode();
        m_isTheFirstCallToAckManualMode = false;
    }

    if(guiContext() != nullptr)
        useManualMode(false);
}

void CT_VirtualAbstractStep::quitManualMode()
{
    STEP_LOG->addInfoMessage(displayableCustomName() + tr(" quit manual mode"));

    if(guiContext() != nullptr)
        useManualMode(true);

    m_mustQuitManualMode = true;
}

bool CT_VirtualAbstractStep::isModifiable() const
{
    return (m_isManual && m_isModifiable && m_inputManager.hasAllModelsTheMinimumNumberOfSelectedPossibilityRequired());
}

void CT_VirtualAbstractStep::executeModify()
{
    if(isModifiable())
        runProcessing(true);
}

void CT_VirtualAbstractStep::setSettingsModified(bool modified)
{
    if(m_isSettingsModified != modified)
    {
        m_isSettingsModified = modified;
        emit settingsModified();
    }
}

CT_StepConfigurableDialog* CT_VirtualAbstractStep::createOrGetStandardPreInputConfigurationDialog()
{
    if(m_preInputConfigDialog != nullptr)
        return m_preInputConfigDialog;

    m_preInputConfigDialog = new CT_StepConfigurableDialog();
    m_preInputConfigDialog->setWindowTitle(m_preInputConfigDialog->windowTitle() + QString(" : %1 (%2)").arg(description(), QString().setNum(uniqueID())));
    return m_preInputConfigDialog;
}

bool CT_VirtualAbstractStep::preInputConfigure()
{
    if(m_preInputConfigDialog != nullptr)
    {
        if(CT_ConfigurableWidgetToDialog::exec(m_preInputConfigDialog) != QDialog::Accepted)
            return false;

        setSettingsModified(m_preInputConfigDialog->isSettingsModified());
    }

    return true;
}

bool CT_VirtualAbstractStep::configureInputs(bool forceReadOnly)
{
    const bool firstTime = mustDeclareInputModels();

    if(firstTime) {
        if(!internalDeclareInputModels())
            return false;
    }

    const CT_InManager::ConfigureReturn ret = m_inputManager.configureInputs(this, hasChildrens() || forceReadOnly);

    if(ret == CT_InManager::HasModification)
        setSettingsModified(true);

    return (ret == CT_InManager::HasModification);
}

bool CT_VirtualAbstractStep::exportViewForINModelConfig(QString exportPath, QStringList &exportedFiles)
{
    return m_inputManager.exportViewForINModelConfig(exportPath, exportedFiles);
}

bool CT_VirtualAbstractStep::postInputConfigure()
{
    if(m_postInputConfigDialog != nullptr)
    {
        if(CT_ConfigurableWidgetToDialog::exec(m_postInputConfigDialog) != QDialog::Accepted)
            return false;

        setSettingsModified(m_postInputConfigDialog->isSettingsModified());
    }

    return true;
}

void CT_VirtualAbstractStep::setProgress(float progress)
{
    if(((int)progress) != m_currentProgressValue)
    {
        m_currentProgressValue = (int)progress;
        m_elapsedTimeInMs = m_executeTime.elapsed();
    }
}

void CT_VirtualAbstractStep::setErrorCode(int errorCode)
{
    m_currentErrorCode = errorCode;
}

void CT_VirtualAbstractStep::setErrorMessage(int errorCode, const QString &errorMessage)
{
    m_errorMessagesByErrorCode.insert(errorCode, errorMessage);
}

void CT_VirtualAbstractStep::clearErrorMessage()
{
    m_errorMessagesByErrorCode.clear();
}

void CT_VirtualAbstractStep::setDebuggable(bool debuggable)
{
    m_isDebuggable = debuggable;
}

void CT_VirtualAbstractStep::waitForAckIfInDebugMode()
{
    if(m_isDebugModeOn)
    {
        if(m_currentBreakPointNumber == m_nextBreakPointNumberToStop)
        {
            m_inputManager.setResultsBusy(false);

            m_hasReceivedAckForDebugMode = false;
            emit waitForAckInDebugMode();

            // preWaitForAckIfInDebugMode() and postWaitForAckIfInDebugMode() was called
            // by the GUI now

            QMutex mutex;

            while((m_nextBreakPointNumberToStop != -1)
                    && (!m_hasReceivedAckForDebugMode)
                    && (!m_mustStop))
            {
                mutex.lock();
                QWaitCondition().wait(&mutex, 100);
                mutex.unlock();
            }

            m_inputManager.setResultsBusy(true);
        }

        ++m_currentBreakPointNumber;
    }
}

int CT_VirtualAbstractStep::currentDebugBreakPointNumber() const
{
    return m_currentBreakPointNumber;
}

int CT_VirtualAbstractStep::currentNumberOfBreakPointToJump() const
{
    return m_nBreakPointToJumpSetInAckDebugMode;
}

void CT_VirtualAbstractStep::setModifiable(bool modifiableOn)
{
    if(modifiableOn)
        m_isManual = true;

    m_isModifiable = modifiableOn;
}

void CT_VirtualAbstractStep::setManual(bool manualModeOn)
{
    m_isManual = manualModeOn;
}

void CT_VirtualAbstractStep::requestManualMode()
{
    if(m_isManual)
    {
        m_mustQuitManualMode = false;
        m_inputManager.setResultsBusy(false);

        emit manualModeRequired();

        QMutex mutex;

        while(!m_mustQuitManualMode
                && (!isStopped()))
        {
            mutex.lock();
            QWaitCondition().wait(&mutex, 100);
            mutex.unlock();

// Possible alternative to QWaitCondition and QMutex
//            QEventLoop loop;
//            QTimer t;
//            t.connect(&t, &QTimer::timeout, &loop, &QEventLoop::quit);
//            t.start(100);
//            loop.exec();
        }

        m_inputManager.setResultsBusy(true);

        emit manualModeCompleted();
    }
}

void CT_VirtualAbstractStep::initManualMode()
{
}

void CT_VirtualAbstractStep::useManualMode(bool)
{
}

void CT_VirtualAbstractStep::resetCurrentID()
{
    CURRENT_ID = 1;
}

QString CT_VirtualAbstractStep::generateHTMLDocumentation(QString directory, QString cssRelativeDirectory)
{
    QString outFilename = QString("%1/%2.html").arg(directory, this->name());
    QString pluginName = this->plugin()->pluginToolForStep()->officialName();
    QString pluginURL = this->plugin()->pluginToolForStep()->url();

    QFile f(outFilename);
    if (f.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&f);
        stream << "<!DOCTYPE html>\n";
        stream << "<html>\n";
        stream << "<head>\n";
        stream << "<metacharset=\"utf-8\">\n";
        stream << "<title>Computree Documentation</title>";
        stream << "<link rel=\"stylesheet\" href=\"" << cssRelativeDirectory << "/style.css\" />";
        stream << "</head>\n";
        stream << "<body>";
        stream << "<div class=\"mainBlock\">";

        stream << "<h1>" << this->description() << "</h1>\n";

        stream << "<Strong>" << tr("Plugin : </Strong><a href=\"") << pluginURL << "\" target=\"_blank\" rel=\"noreferrer noopener\">"<< pluginName << "</a>\n";
        stream << "<br>";
        stream << "<Strong>" << tr("Nom de classe : </strong>") << this->name() << "\n";
        stream << "<br><br>";

        stream << "<section>\n";
        stream << "<h2>" << tr("Description") << "</h2>\n";
        stream << "<div class=\"descBlock\">";
        stream << this->detailledDescription() << "\n";
        stream << "</div>";
        stream << "</section>\n";

        QString parameters = this->parametersDescription(true);
        if (!parameters.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Paramètres") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << parameters;
            stream << "</div>";
            stream << "</section>\n";
        }

        QString inputResults = this->inputDescription();
        if (!inputResults.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Données d'entrée") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << inputResults;
            stream << "</div>";
            stream << "</section>\n";
        }

        QString outputResults = this->outputDescription();
        if (!outputResults.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Données de sortie") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << outputResults;
            stream << "</div>";
            stream << "</section>\n";
        }

        QString details = this->detailsDescription();
        if (!details.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Détails") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            stream << details;
            stream << "</div>";
            stream << "</section>\n";
        }

        QStringList references = this->getStepRISCitations();
        if (!references.isEmpty())
        {
            stream << "<section>\n";
            stream << "<h2>" << tr("Références") << "</h2>\n";
            stream << "<div class=\"descBlock\">";
            for (int i = 0 ; i < references.size() ; i++)
            {
                stream << CT_ParseRIS::parseRIS(references.at(i));
                stream << "<br>";
            }
            stream << "</div>";
            stream << "</section>\n";
        }

        stream << "</div>\n";
        stream << "</body>\n";
        stream << "</html>";
        f.close();
    }

    return outFilename;
}

void CT_VirtualAbstractStep::createPreInputConfigurationDialog()
{
    if(m_preInputConfigDialog == nullptr) {
        m_preInputConfigDialog = new CT_StepConfigurableDialog();
        m_preInputConfigDialog->setWindowTitle(m_preInputConfigDialog->windowTitle() + QString(" : %1 (%2)").arg(description(), QString().setNum(uniqueID())));

        fillPreInputConfigurationDialog(m_preInputConfigDialog);

        if(m_preInputConfigDialog->isEmpty()) {
            delete m_preInputConfigDialog;
            m_preInputConfigDialog = nullptr;
        }
    }
}

void CT_VirtualAbstractStep::createPostInputConfigurationDialog()
{
    if(m_postInputConfigDialog == nullptr) {
        m_postInputConfigDialog = new CT_StepConfigurableDialog();
        m_postInputConfigDialog->setWindowTitle(m_postInputConfigDialog->windowTitle() + QString(" : %1 (%2)").arg(description(), QString().setNum(uniqueID())));

        fillPostInputConfigurationDialog(m_postInputConfigDialog);

        if(m_postInputConfigDialog->isEmpty()) {
            delete m_postInputConfigDialog;
            m_postInputConfigDialog = nullptr;
        }
    }
}

void CT_VirtualAbstractStep::recursiveClearOutResult()
{
    recursiveClearChildOutResult();
    clearOutResult();
}

void CT_VirtualAbstractStep::recursiveClearChildOutResult()
{
    QListIterator<CT_VirtualAbstractStep*> it(m_childrens);

    while(it.hasNext())
        it.next()->recursiveClearOutResult();
}

void CT_VirtualAbstractStep::recursiveClearOutResultFromMemory()
{
    QListIterator<CT_VirtualAbstractStep*> it(m_childrens);

    while(it.hasNext())
        it.next()->recursiveClearOutResultFromMemory();

    clearOutResultFromMemory();
}

bool CT_VirtualAbstractStep::mustDeclareInputModels() const
{
    return (m_inputManager.nResultModel() == 0);
}

bool CT_VirtualAbstractStep::canDeclareInputModels() const
{
    return m_childrens.isEmpty() && mustDeclareInputModels();
}

bool CT_VirtualAbstractStep::internalDeclareInputModels()
{
    if(canDeclareInputModels())
    {
        m_inputManager.createInputModels(*this);

        if(parentStep() == nullptr)
            return (isAPrototype() || !needInputResults()); // we return true if we are a prototype otherwise false because the GUI don't set the parent step

        // this method must always returns true and only returns false if the parent step is nullptr(happen when it was the prototype) or
        // if the collection of input result model is not empty
        return m_inputManager.findInputsInOutputsOfThisStepOrRecursively(parentStep());
    }

    return false;
}

void CT_VirtualAbstractStep::setParentStep(const CT_VirtualAbstractStep* parent)
{
    m_parentStep = const_cast<CT_VirtualAbstractStep*>(parent);
}

void CT_VirtualAbstractStep::setLaunchTime(const QDateTime& launchTime)
{
    m_launchDateTime = launchTime;
}
void CT_VirtualAbstractStep::runProcessing(bool modificationMode)
{
    STEP_LOG->addTraceMessage(displayableCustomName() + tr(" - Start computing at \t%1").arg(QTime::currentTime().toString("hh:mm:ss")));

    m_isRunning = true;
    m_mustStop = false;

    if(modificationMode) // supprime tous les anciens resultats des etapes enfants
        recursiveClearChildOutResult();
    else // supprime tous les anciens resultats ainsi que ceux des etapes enfants
        recursiveClearOutResult();

    // en fonction des resultats modeles configurer on cree les resultats à utiliser
    bool canCompute = !m_inputManager.needInputs() || (m_inputManager.nResults() > 0);

    // debut du lancement
    setLaunchTime(QDateTime::currentDateTime());
    m_currentBreakPointNumber = 0;
    m_nextBreakPointNumberToStop = 0;
    setErrorCode(-2);

    m_executeTime.start();
    setProgress(0);

    m_isTheFirstCallToAckManualMode = true;

    //PS_MODELS->clearCache();

    // en fonction des resultats modeles de sortie on cree les resultats de sortie que l'etape
    // pourra utiliser dans sa methode compute.
    if(!modificationMode)
        canCompute = m_outputManager.createOutputResults(*this);

    if(canCompute)
    {
        setErrorCode(0);

        m_inputManager.setResultsBusy(true);

        // le traitement lui-meme
        if(modificationMode)
            modify();
        else
            compute();

        m_inputManager.setResultsBusy(false);

        // si l'utilisateur a stopper le traitement
        if(isStopped())
        {
            // on supprime tous les resultats genere
            setProgress(0);
            clearOutResult();
        }
        else if(errorCode() == 0)
        {
            // on peut dire au ResultManager que les résultats du tour courant sont OK
            if(!modificationMode)
                m_outputManager.finalizeResults();

            setSettingsModified(false);
            setProgress(100);
        }
    }

    if(!canCompute)
        setErrorCode(-3);

    m_isRunning = false;

    int elaps = m_executeTime.elapsed();
    int hour = elaps/3600000;
    elaps -= hour*3600000;
    int min = elaps/60000;
    elaps -= min*60000;
    int sec = elaps/1000;
    elaps -= sec*1000;
    int ms = elaps;

    STEP_LOG->addTraceMessage(displayableCustomName() + tr(" - Computing completed, elapsed time:\t\t%1h:%2m:%3s:%4ms").arg(hour).arg(min).arg(sec).arg(ms));

    emit isCompleted();
}

void CT_VirtualAbstractStep::removeOutResult(int n)
{
    CT_AbstractResult* res = m_outputManager.takeResultAt(n);

    emit resultToBeClearedFromMemory(res);

    res->setBusy(true);
    res->clearFromMemory();
    res->setBusy(false);

    emit resultToBeRemoved(res);

    delete res;
}

CT_InManager* CT_VirtualAbstractStep::inManager() const
{
    return &const_cast<CT_InManager&>(m_inputManager);
}

CT_OutManager* CT_VirtualAbstractStep::outManager() const
{
    return &const_cast<CT_OutManager&>(m_outputManager);
}

CT_UniqueIndexGenerator* CT_VirtualAbstractStep::uniqueIndexGenerator() const
{
    return rootStep()->m_uniqueIndexGenerator;
}

void CT_VirtualAbstractStep::setClearMemoryProgress(int progress)
{
    if(progress != m_currentClearMemoryProgressValue)
        m_currentClearMemoryProgressValue = progress;
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForModel(QString &str, int nbTab, const CT_OutAbstractModel *model) const
{
    const CT_OutAbstractResultModel *rModel = dynamic_cast<const CT_OutAbstractResultModel*>(model);

    if(rModel != nullptr)
    {
        recursiveCreateHelpStrForResultModel(str, nbTab, rModel);
    }
    else
    {
        const CT_OutAbstractItemModel *iModel = dynamic_cast<const CT_OutAbstractItemModel*>(model);

        if(iModel != nullptr)
        {
            recursiveCreateHelpStrForItemModel(str, nbTab, iModel);
        }
        else
        {
            const CT_OutAbstractItemAttributeModel *iaModel = dynamic_cast<const CT_OutAbstractItemAttributeModel*>(model);

            if(iaModel != nullptr)
                recursiveCreateHelpStrForItemAttributesModel(str, nbTab, iaModel);
            else
                qFatal("In CT_VirtualAbstractStep::recursiveCreateItemsForModel : model is not known");
        }
    }
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForModel(QString &str, int nbTab, const CT_InAbstractModel *model) const
{
    const CT_InAbstractResultModel *rModel = dynamic_cast<const CT_InAbstractResultModel*>(model);

    if(rModel != nullptr)
    {
        recursiveCreateHelpStrForResultModel(str, nbTab, rModel);
    }
    else
    {
        const CT_InAbstractItemModel *iModel = dynamic_cast<const CT_InAbstractItemModel*>(model);

        if(iModel != nullptr)
        {
            recursiveCreateHelpStrForItemModel(str, nbTab, iModel);
        }
        else
        {
            const CT_InAbstractItemAttributeModel *iaModel = dynamic_cast<const CT_InAbstractItemAttributeModel*>(model);

            if(iaModel != nullptr)
                recursiveCreateHelpStrForItemAttributesModel(str, nbTab, iaModel);
            else
                qFatal("In CT_VirtualAbstractStep::recursiveCreateItemsForModel : model is not known");
        }
    }
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForResultModel(QString &str, int nbTab, const CT_OutAbstractResultModel *rModel) const
{
    for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}
    str.append("<br><span class=\"resultOutDescr\">Result : " + rModel->displayableName() + "</span>");
    str.append("<br>");

    createHelpStrForChildrens(str, nbTab+1, rModel);
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForResultModel(QString &str, int nbTab, const CT_InAbstractResultModel *rModel) const
{
    for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}

    if (rModel->needOutputModel())
    {
        if (rModel->isOptionnal())
        {
            str.append("<br><span class=\"resultInDescr\"><em>Result : " + rModel->displayableName() + " - " + tr("Optionnel") + "</em></span>");
        } else {
            str.append("<br><span class=\"resultInDescr\">Result : " + rModel->displayableName() + "</span>");
        }
        str.append("<br>");
    } else {
        str.append(tr("<br>&nbsp;&nbsp;&nbsp;&nbsp;Aucune donnée nécessaire en entrée."));
    }

    createHelpStrForChildrens(str, nbTab+1, rModel);
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForItemModel(QString &str, int nbTab, const CT_OutAbstractItemModel *iModel) const
{
    QString bold1 = "";
    QString bold2 = "";
    if (iModel == iModel->originalModel())
    {
        bold1 = "<strong>";
        bold2 = "</strong>";
    }

    for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}
    str.append(bold1 + iModel->displayableName() + bold2 + " <span style=\"color:grey;font-weight:lighter;\">(" + iModel->itemDrawableStaticCastT<>()->name() + ")</span>");
    str.append("<br>");

    if (dynamic_cast<CT_StandardItemGroup*>(iModel->prototype()) != nullptr || iModel == iModel->originalModel())
    {
        createHelpStrForChildrens(str, nbTab+1, iModel);
    }
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForItemModel(QString &str, int nbTab, const CT_InAbstractItemModel *iModel) const
{
    for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}
    if (iModel->displayableName() == "*")
    {
        str.append("...");
    } else {
        if (iModel->isOptionnal())
        {
            str.append("<em>" + iModel->displayableName() + " <span style=\"color:grey;font-weight:lighter;\">(" + CT_AbstractItemDrawable::nameFromType(iModel->itemType()) + ")</span> - " + tr("Optionnel") + "</em>");
        } else {
            str.append(iModel->displayableName() + " <span style=\"color:grey;font-weight:lighter;\">(" + CT_AbstractItemDrawable::nameFromType(iModel->itemType()) + ")</span>");
        }
    }
    str.append("<br>");

    createHelpStrForChildrens(str, nbTab+1, iModel);
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForItemAttributesModel(QString &str, int nbTab, const CT_OutAbstractItemAttributeModel *iaModel) const
{
    QString bold1 = "<strong>";
    QString bold2 = "</strong>";

    if (iaModel == iaModel->originalModel() && !iaModel->isADefaultItemAttributeModel())
    {
        for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}
        str.append(bold1 + iaModel->itemAttributeStaticCastT<>()->displayableName() + bold2 + " <span style=\"color:grey;font-weight:lighter;\">(" + iaModel->itemAttributeStaticCastT<>()->valueTypeToString().toLower() + ")</span>");
        str.append("<br>");

        createHelpStrForChildrens(str, nbTab+1, iaModel);
    }
}

void CT_VirtualAbstractStep::recursiveCreateHelpStrForItemAttributesModel(QString &str, int nbTab, const CT_InAbstractItemAttributeModel *iaModel) const
{
    for (int i = 0 ; i < nbTab ; i++) {str.append("&nbsp;&nbsp;");}
    if (iaModel->isOptionnal())
    {
        str.append("<em>" + iaModel->displayableName() + " <span style=\"color:grey;font-weight:lighter;\">(" + CT_AbstractCategory::valueTypeToString(CT_AbstractCategory::ValueType(iaModel->valueType())) + ")</span> - " + tr("Optionnel") + "</em>");
    } else {
        str.append(iaModel->displayableName() + " <span style=\"color:grey;font-weight:lighter;\">(" + CT_AbstractCategory::valueTypeToString(CT_AbstractCategory::ValueType(iaModel->valueType())) + ")</span>");
    }
    str.append("<br>");

    createHelpStrForChildrens(str, nbTab+1, iaModel);
}

void CT_VirtualAbstractStep::createHelpStrForChildrens(QString &str, int nbTab, const CT_OutAbstractModel *model) const
{
    model->visitOutChildrens([this, &str, &nbTab](const CT_OutAbstractModel* child) -> bool {
        this->recursiveCreateHelpStrForModel(str, nbTab+1, child);
        return true;
    });
}

void CT_VirtualAbstractStep::createHelpStrForChildrens(QString &str, int nbTab, const CT_InAbstractModel *model) const
{
    model->visitInChildrens([this, &str, &nbTab](const CT_InAbstractModel* child) -> bool {
        this->recursiveCreateHelpStrForModel(str, nbTab+1, child);
        return true;
    });
}

void CT_VirtualAbstractStep::openHelpPageForStep() const
{
    QDesktopServices::openUrl(getHelpPageForStepPath());
}

QUrl CT_VirtualAbstractStep::getHelpPageForStepPath() const
{
    QString currentLanguageDir = "doc_" + CURRENT_LANGAGE;
#if defined(__linux__) // Linux
    currentLanguageDir = QDir::homePath() + "/.computree/" + currentLanguageDir;
#endif

    QFile currentFile(currentLanguageDir + "/steps/current.html");
    if (currentFile.exists()) {currentFile.remove();}
    QFile::copy(currentLanguageDir + "/steps/" + this->name() + ".html", currentLanguageDir + "/steps/current.html");

    return QUrl("file:///" + QCoreApplication::applicationDirPath() + "/" + currentLanguageDir + "/index.html");

}


