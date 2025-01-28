#include "cdm_stepmanager.h"

#include "cdm_scriptmanagerabstract.h"
#include "cdm_actionsmanager.h"

#include "ct_result/ct_resultgroup.h"

#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QWaitCondition>
#include <QLocale>
#include <QCoreApplication>

#define CDM_SCRIPT_BACKUP_FILEPATH QDir::toNativeSeparators(".\\backupSteps")+_scripManager->getFileExtensionAccepted().first()

CDM_StepManager::CDM_StepManager(const CDM_ScriptManagerAbstract *scriptManager,
                                 const CDM_ActionsManager *actionManager) : QThread()
{
    setScriptManager(scriptManager);

    _beginStep = nullptr;
    m_currentStep = nullptr;
    _debugMode = false;
    _stop = false;
    _action = None;
    m_guiContext = nullptr;
    m_actionsManager = (CDM_ActionsManager*)actionManager;
    _force = false;
    m_debugAutoMode = false;

    _options.load();
    setDefaultQLocale(_options.getQLocale());

    connect(this, SIGNAL(stepFinishExecuted(CT_VirtualAbstractStep*)), this, SLOT(slotRemoveActionsAfterStepExecuted()), Qt::QueuedConnection);
}

CDM_StepManager::~CDM_StepManager()
{
    stop();

    wait();

    qDeleteAll(_stepRootList.begin(), _stepRootList.end());
    //qDeleteAll(_threadCreated.begin(), _threadCreated.end());

    _options.save();

    QFile(CDM_SCRIPT_BACKUP_FILEPATH).remove();
}

void CDM_StepManager::setGuiContext(const GuiContextInterface *guiContext)
{
    m_guiContext = (GuiContextInterface*)guiContext;
}

void CDM_StepManager::setScriptManager(const CDM_ScriptManagerAbstract *scriptManager)
{
    _scripManager = (CDM_ScriptManagerAbstract*)scriptManager;
}

CDM_ScriptManagerAbstract* CDM_StepManager::getScriptManager() const
{
    return _scripManager;
}

bool CDM_StepManager::addStep(CT_VirtualAbstractStep *step, CT_VirtualAbstractStep *parent)
{
    if(parent == nullptr)
    {
        _stepRootList.append(step);
        //createThread(*step);

        connectStep(step);

        emit stepAdded(step);

        return true;
    }
    else
    {
        if(parent->appendStep(step))
        {
            //createThread(*step);

            connectStep(step);

            emit stepAdded(step);

            return true;
        }
    }

    return false;
}

bool CDM_StepManager::removeStep(CT_VirtualAbstractStep *step)
{
    if(isRunning())
    {
        return false;
    }

    if(step->parentStep() != nullptr)
    {
        emit stepToBeRemoved(step);

        //_threadCreated.removeOne(step->thread());
        step->parentStep()->removeStep(step);

        return true;
    }
    else
    {
        emit stepToBeRemoved(step);

        //_threadCreated.removeOne(step->thread());
        _stepRootList.removeOne(step);
        delete step;

        return true;
    }

    return false;
}

bool CDM_StepManager::clearStep()
{
    if(isRunning())
        return false;

    while(!_stepRootList.isEmpty())
        removeStep(_stepRootList.first());

    return true;
}

void CDM_StepManager::setStepDebugModeOn(CT_VirtualAbstractStep *step, bool debugModeOn)
{
    if(m_currentStep == step)
        step->postWaitForAckIfInDebugMode();

    step->setDebugModeOn(debugModeOn);
}

QList<CT_VirtualAbstractStep*> CDM_StepManager::getStepRootList() const
{
    return _stepRootList;
}

bool CDM_StepManager::executeStep(CT_VirtualAbstractStep *beginStep)
{
    if(isRunning())
    {
        if(_debugMode)
            return ackDebugMode(-1);

        return false;
    }

    return internalExecuteStep(beginStep, false);
}

bool CDM_StepManager::executeModifyStep(CT_VirtualAbstractStep *beginStep)
{
    if(isRunning() || (beginStep == nullptr) || !beginStep->isModifiable())
        return false;

    return internalExecuteModifyStep(beginStep, false);
}

bool CDM_StepManager::executeOrForwardStepInDebugMode(CT_VirtualAbstractStep *beginStep)
{
    if(!isRunning())
        return internalExecuteStep(beginStep, true);

    return ackDebugMode(1);
}

bool CDM_StepManager::executeOrForwardStepFastInDebugMode(CT_VirtualAbstractStep *beginStep)
{
    if(!isRunning())
        return internalExecuteStep(beginStep, true);

    return ackDebugMode(_options.getFastJumpValueInDebugMode());
}

bool CDM_StepManager::executeOrForwardStepAutoInDebugMode(CT_VirtualAbstractStep *beginStep)
{
    m_debugAutoMode = true;

    if(!isRunning())
        return internalExecuteStep(beginStep, true);

    return ackDebugMode(1);
}

bool CDM_StepManager::quitManualMode()
{
    QMutexLocker locker(&_mutex);

    if(m_currentStep == nullptr)
        return false;

    m_currentStep->quitManualMode();

    return true;
}

void CDM_StepManager::stop()
{
    QMutexLocker locker(&_mutex);

    _stop = true;
    m_debugAutoMode = false;
    _debugMode = false;

    if(m_currentStep != nullptr)
        m_currentStep->stop();
}

bool CDM_StepManager::setFastForwardJumpInDebugMode(int value)
{
    return _options.changeFastJumpInDebugMode(value);
}

void CDM_StepManager::setTimeToSleepInAutoDebugMode(int timeInMs)
{
    _options.setTimeToSleepInAutoDebugMode(timeInMs);
}

void CDM_StepManager::setNJumpInAutoDebugMode(int n)
{
    _options.setNJumpInAutoDebugMode(n);
}

bool CDM_StepManager::setOptions(CDM_StepManagerOptions options)
{
    if(!isRunning())
    {
        _options = options;
        setDefaultQLocale(_options.getQLocale());

        return true;
    }

    return false;
}

CDM_StepManagerOptions CDM_StepManager::getOptions() const
{
    return _options;
}

CDM_StepManager::ActionType CDM_StepManager::getAction() const
{
    return _action;
}

bool CDM_StepManager::checkOneStepIsInDebugModeFromStep(CT_VirtualAbstractStep *step) const
{
    QList<CT_VirtualAbstractStep*> stepChildList;

    if(step != nullptr)
    {
        if(step->isDebugModeOn())
            return true;

        step->visitChildrens([&stepChildList](const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep* child) -> bool {
            stepChildList.append(const_cast<CT_VirtualAbstractStep*>(child));
            return true;
        });
    }
    else
    {
        stepChildList = getStepRootList();
    }

    for(CT_VirtualAbstractStep* step : stepChildList)
    {
        if(checkOneStepIsInDebugModeFromStep(step))
            return true;
    }

    return false;
}

bool CDM_StepManager::isInManualMode() const
{
    QMutexLocker locker((QMutex*)&_mutex);

    if(m_currentStep == nullptr)
        return false;

    return m_currentStep->isInManualMode();
}

bool CDM_StepManager::isScriptBackupAvailable() const
{
    return QFile::exists(CDM_SCRIPT_BACKUP_FILEPATH);
}

void CDM_StepManager::restoreScriptBackup()
{
    if(!isRunning()) {
        clearStep();

        QString path = getScriptBackupFilePath();
        _scripManager->loadScript(path, *this);

        QFile(path).remove();
    }
}

QString CDM_StepManager::getScriptBackupFilePath() const
{
    if(isScriptBackupAvailable())
        return QString(CDM_SCRIPT_BACKUP_FILEPATH);

    return "";
}

////////////// PROTECTED //////////////

bool CDM_StepManager::internalExecuteStep(CT_VirtualAbstractStep *beginStep, bool debugMode)
{
    _action = ExecuteStep;
    _debugMode = debugMode;
    _stop = false;
    _force = (beginStep != nullptr);

    //save script before launch to get the user the ability to recover the tree of steps if a crash was happening !
    _scripManager->writeScript(CDM_SCRIPT_BACKUP_FILEPATH, true, *this);

    emit started(true);

    _beginStep = beginStep;

    start();

    return true;
}

bool CDM_StepManager::internalExecuteModifyStep(CT_VirtualAbstractStep *beginStep, bool debugMode)
{
    _action = ExecuteModifyStep;
    _debugMode = debugMode;
    _stop = false;
    _force = (beginStep != nullptr);

    emit started(true);

    _beginStep = beginStep;

    start();

    return true;
}

void CDM_StepManager::connectStep(CT_VirtualAbstractStep *step)
{
    connect(step, SIGNAL(resultAdded(const CT_AbstractResult*)), this, SIGNAL(resultAdded(const CT_AbstractResult*)), Qt::DirectConnection);
    connect(step, SIGNAL(resultToBeClearedFromMemory(const CT_AbstractResult*)), this, SIGNAL(resultToBeClearedFromMemory(const CT_AbstractResult*)), Qt::DirectConnection);
    connect(step, SIGNAL(resultToBeRemoved(const CT_AbstractResult*)), this, SIGNAL(resultToBeRemoved(const CT_AbstractResult*)), Qt::DirectConnection);
}

void CDM_StepManager::connectStepBeforeRunning(CT_VirtualAbstractStep *step)
{
    connect(step, SIGNAL(waitForAckInDebugMode()), this, SLOT(slotStepWaitForAckInDebugMode()), Qt::QueuedConnection);
    connect(step, SIGNAL(showMessage(QString)), this, SIGNAL(stepNeedShowMessage(QString)), Qt::QueuedConnection);
    connect(step, SIGNAL(manualModeRequired()), this, SLOT(slotStepRequiredManualMode()), Qt::QueuedConnection);
    connect(step, SIGNAL(manualModeCompleted()), this, SLOT(slotStepManualModeCompleted()), Qt::QueuedConnection);
}

void CDM_StepManager::disconnectStepAfterRunning(CT_VirtualAbstractStep *step)
{
    disconnect(step, SIGNAL(waitForAckInDebugMode()), this, SLOT(slotStepWaitForAckInDebugMode()));
    disconnect(step, SIGNAL(showMessage(QString)), this, SIGNAL(stepNeedShowMessage(QString)));
    disconnect(step, SIGNAL(manualModeRequired()), this, SLOT(slotStepRequiredManualMode()));
    disconnect(step, SIGNAL(manualModeCompleted()), this, SLOT(slotStepManualModeCompleted()));
}

void CDM_StepManager::run()
{
    if(_action == ExecuteStep)
    {
        CT_VirtualAbstractStep* restartFromStep;
        bool forceExecution = false;

        do
        {
            restartFromStep = nullptr;

            if(_beginStep == nullptr)
            {
                bool continueLoop = true;

                QListIterator<CT_VirtualAbstractStep*> it(_stepRootList);

                while(continueLoop
                      && (restartFromStep == nullptr)
                      && it.hasNext())
                {
                    continueLoop = recursiveExecuteStep(*it.next(), restartFromStep, false);
                }

                _beginStep = restartFromStep;
                forceExecution = (restartFromStep != nullptr);
            }
            else
            {
                recursiveExecuteStep(*_beginStep, restartFromStep, _force | forceExecution);
                _beginStep = restartFromStep;
                forceExecution = (restartFromStep != nullptr);
            }

        }while(restartFromStep != nullptr);

        m_debugAutoMode = false;
        _debugMode = false;

        // remove the backup of the script
        //QFile(CDM_SCRIPT_BACKUP_FILEPATH).remove();
    }

    emit completed(true);
}

////////////// PRIVATE //////////////

bool CDM_StepManager::recursiveExecuteStep(CT_VirtualAbstractStep &step, CT_VirtualAbstractStep*& restartFromStep, bool force)
{

    bool continueLoop = true;
    bool forceAfterExecute = force;

    // si l'etape a subit des changements
    // ou si elle n'a aucun resultat de sortie et qu'elle n'a pas ete lancee
    // ou si elle en a un mais qu'il a ete supprime de la memoire
    if(forceAfterExecute
        || step.isSettingsModified()
        || ((step.nOutResult() == 0) && (step.progressValue() < 100))
        || ((step.nOutResult() > 0) && (step.outResultAt(0)->isClearedFromMemory())))
    {
        // on lance l'etape seulement si elle a besoin d'un resultat
        // et qu'il y en a un
        // OU
        // que l'etape n'a pas besoin de resultat

        if((step.needInputResults()
            && (step.parentStep() != nullptr)
            && !step.parentStep()->isStopped()
            && (step.parentStep()->errorCode() == 0))
            || !step.needInputResults())
        {
            _mutex.lock();
            m_currentStep = &step;
            connectStepBeforeRunning(&step);

            if(step.isDebugModeOn())
                step.setGuiContext(m_guiContext);

            _mutex.unlock();

            emit stepBeginExecuted(&step);

            // lance l'tape
            if((_action == ExecuteModifyStep) && (_beginStep != nullptr) && (&step == _beginStep) && step.isModifiable())
                step.executeModify();
            else
                step.execute();

            emit stepFinishExecuted(&step);

            _mutex.lock();
            disconnectStepAfterRunning(&step);

            step.setGuiContext(nullptr);

            m_currentStep = nullptr;
            _mutex.unlock();

            forceAfterExecute = true;

            if(step.errorCode() != 0)
                continueLoop = false;
        }
        else
        {
            continueLoop = false;
        }
    }

    if(continueLoop)
        continueLoop = !step.isStopped();

    if(continueLoop)
    {
        restartFromStep = step.restartComputeFromStep();

        if(restartFromStep != nullptr)
            return true;
    }

    // si on a pas stoppé l'étape ou si il n'y a pas eu d'erreur
    if(continueLoop)
    {
        // on continu avec les tapes filles
        step.visitChildrens([this, &continueLoop, &restartFromStep, &forceAfterExecute](const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep* child) -> bool {
            continueLoop = this->recursiveExecuteStep(*const_cast<CT_VirtualAbstractStep*>(child), restartFromStep, forceAfterExecute);
            return continueLoop && (restartFromStep == nullptr);
        });

        if(restartFromStep == nullptr)
        {
            // si on doit supprimer de la mmoire les rsultats des tapes et
            // que la sauvegarde automatique n'est pas active
            if(_options.isAutoClearResultFromMemoryEnable())
                step.clearOutResultFromMemory(); // on peut commencer  dtruire les rsultats
        }
    }

    return continueLoop;
}

void CDM_StepManager::recursiveClearResult(CT_VirtualAbstractStep &step)
{
    step.visitChildrens([this](const CT_VirtualAbstractStep*, const CT_VirtualAbstractStep* child) -> bool {
        this->recursiveClearResult(*const_cast<CT_VirtualAbstractStep*>(child));
        return !_stop;
    });


    if(!_stop)
    {
        step.clearOutResult();
    }

}

/*QThread* CDM_StepManager::createThread(CT_VirtualAbstractStep &step)
{
    QThread *thread = step.thread();

    if(!_threadCreated.contains(thread))
    {
        thread = new QThread();
        step.moveToThread(thread);

        connect(&step, SIGNAL(isCompleted()), thread, SLOT(quit()), Qt::DirectConnection);
        connect(thread, SIGNAL(started()), &step, SLOT(execute()), Qt::QueuedConnection);

        _threadCreated.append(thread);
    }

    return thread;
}*/

bool CDM_StepManager::ackDebugMode(int jumpNStep, bool callPostWait)
{
    QMutexLocker locker(&_mutex);

    if(m_currentStep != nullptr)
    {
        if(callPostWait)
            m_currentStep->postWaitForAckIfInDebugMode();

        if(jumpNStep < 0)
            m_currentStep->ackDebugMode(100000);
        else
            m_currentStep->ackDebugMode(jumpNStep);

        locker.unlock();

        emit stepWaitForAckInDebugMode(false);

        return true;
    }

    return false;
}

void CDM_StepManager::slotStepWaitForAckInDebugMode()
{
    if(_debugMode) {
        m_currentStep->preWaitForAckIfInDebugMode();

        if(m_debugAutoMode)
            QTimer::singleShot(_options.getTimeToSleepInAutoDebugMode(), this, SLOT(autoAckDebugMode()));
        else
            emit stepWaitForAckInDebugMode(true);
    } else {
        ackDebugMode(-1, false);
    }
}

void CDM_StepManager::slotStepRequiredManualMode()
{
    CT_VirtualAbstractStep *step = (CT_VirtualAbstractStep*)sender();

    if(step != nullptr)
    {
        // si on ne connait pas le contexte donc on
        // tourne en mode batch
        if(m_guiContext == nullptr)
        {
            step->quitManualMode(); // on quitte le mode manuel car il n'est pas possible
        }
        else
        {
            emit stepRequiredManualMode(step);
            emit stepInManualMode(true);

            // sinon si on n'a pas déjà donné le contexte à l'étape
            if(step->guiContext() != m_guiContext)
                step->setGuiContext(m_guiContext); // on lui donne

            // on informe l'étape qu'on a bien activé le mode manuel
            step->ackManualMode();
        }
    }
}

void CDM_StepManager::slotStepManualModeCompleted()
{
    CT_VirtualAbstractStep *step = (CT_VirtualAbstractStep*)sender();

    if(step != nullptr)
    {
        emit stepInManualMode(false);
        emit stepQuitManualMode(step);
    }
}

void CDM_StepManager::setDefaultQLocale(QString name)
{
    QLocale locale(name);
    locale.setNumberOptions(QLocale::OmitGroupSeparator);
    QLocale::setDefault(locale);
}

void CDM_StepManager::slotRemoveActionsAfterStepExecuted()
{
    if(m_actionsManager != nullptr)
        m_actionsManager->clearActions();
}

void CDM_StepManager::autoAckDebugMode()
{
    ackDebugMode(_options.getNJumpInAutoDebugMode());
}

