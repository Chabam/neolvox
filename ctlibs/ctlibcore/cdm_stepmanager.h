#ifndef CDM_STEPMANAGER_H
#define CDM_STEPMANAGER_H

#include "computreeCore_global.h"

#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "cdm_stepmanageroptions.h"

#include <QThread>
#include <QMutex>
#include <QObjectUserData>

class CDM_ScriptManagerAbstract;
class CDM_ActionsManager;

/**
 * @brief This is the class that can manage a tree of steps and their execution
 */
class COMPUTREECORESHARED_EXPORT CDM_StepManager : public QThread
{
    Q_OBJECT

public:

    enum ActionType
    {
        ExecuteStep,
        ExecuteModifyStep,
        None
    };

    CDM_StepManager(const CDM_ScriptManagerAbstract *scriptManager,
                    const CDM_ActionsManager *actionManager);
    ~CDM_StepManager();

    void setGuiContext(const GuiContextInterface *guiContext);

    /*!
     *  \brief Definir le manageur de script qui sera utilise pour sauvegarder les scripts.
     *
     * Info : Le stepManager ne supprime pas le scriptManager de la mémoire.
     */
    void setScriptManager(const CDM_ScriptManagerAbstract *scriptManager);

    /*!
     *  \brief Retourne le manageur de script utilise.
     */
    CDM_ScriptManagerAbstract* getScriptManager() const;

    /*!
     *  \brief Ajout d'une etape
     *
     *  \param step : L'etape a ajouter.
     *  \param parent : L'etape parente a laquelle ajouter cette etape, ou nullptr si l'ajout de l'etape doit se faire a la racine
     *
     *  \return true si l'ajout est effectif, false sinon.
     */
    bool addStep(CT_VirtualAbstractStep *step, CT_VirtualAbstractStep *parent = nullptr);

    /*!
     *  \brief Suppression d'une etape
     *
     *  \param step : L'etape a supprimer.
     *
     *  \return true si la suppression est effective, false sinon.
     */
    bool removeStep(CT_VirtualAbstractStep *step);

    /**
     * @brief Remove all steps
     */
    bool clearStep();

    /**
     * @brief Activate or not the debug mode of the step
     */
    void setStepDebugModeOn(CT_VirtualAbstractStep *step, bool debugModeOn);

    /*!
     *  \brief Retourne la liste des etapes du tronc de l'arbre
     */
    QList<CT_VirtualAbstractStep*> getStepRootList() const;

    /*!
     *  \brief Changer les options
     */
    bool setOptions(CDM_StepManagerOptions options);

    /*!
     *  \brief Retourne les options
     */
    CDM_StepManagerOptions getOptions() const;

    /*!
     *  \brief Retourne le type d'action que le gestionnaire est en train d'effectuer (execution, etc...)
     */
    CDM_StepManager::ActionType getAction() const;

    /*!
     *  \brief Retourne vrai si au moins une etape fille de l'etape passe en paramtre est en mode debug (recursif). L'etape en parametre
     *         peut etre nullptr, alors le test s'effectura a partir des etapes du tronc de l'arbre.
     */
    bool checkOneStepIsInDebugModeFromStep(CT_VirtualAbstractStep *step) const;

    /**
     * @brief Return true if the current step is in manual mode
     */
    bool isInManualMode() const;

    /**
     * @brief Returns true if a script backup was available, this is true when the application crash
     */
    bool isScriptBackupAvailable() const;

    /**
     * @brief Restore the last configuration backuped
     */
    void restoreScriptBackup();

    /**
     * @brief Returns the script backup filepath if you want to know it's path. Please don't use it to load the script (use the méthod "restoreScriptBackup")
     */
    QString getScriptBackupFilePath() const;

public slots:

    /*!
     *  \brief Lancement des traitements
     *
     *  \param beginStep : L'etape a partir de laquelle lancer les traitements. nullptr si on desire lancer les traitements
     *                     a partir de la premiere etape qui a subit des modifications.
     *
     *  \return false si des traitements sont deja en cours, true si le lancement c'est bien passe.
     */
    bool executeStep(CT_VirtualAbstractStep *beginStep = nullptr);

    /*!
     *  \brief Lancement des traitements manuels
     *
     *  \param beginStep : L'etape a partir de laquelle lancer les traitements. Ne peut être nullptr.
     *
     *  \return false si des traitements sont deja en cours, true si le lancement c'est bien passe.
     */
    bool executeModifyStep(CT_VirtualAbstractStep *beginStep);

    /*!
     *  \brief Lancement des traitements en mode debug OU avancer de 1 pas
     *
     *  \param beginStep : L'etape a partir de laquelle lancer les traitements. nullptr si on desire lancer les traitements
     *                     a partir de la premiere etape qui a subit des modifications. EVIDEMMENT : Si une etape attend un ack de debug ce parametre
     *                     n'est pas pris en compte.
     *
     *  \return false si aucun traitements n'attend de ack ou si un lancement en mode non debug a deja ete effectue, true si le lancement c'est bien passe.
     */
    bool executeOrForwardStepInDebugMode(CT_VirtualAbstractStep *beginStep = nullptr);

    /*!
     *  \brief Lancement des traitements en mode debug ou avancer de N pas
     *
     *  \param beginStep : L'etape a partir de laquelle lancer les traitements. nullptr si on desire lancer les traitements
     *                     a partir de la premiere tape qui a subit des modifications. EVIDEMMENT : Si une etape attend un ack de debug ce parametre
     *                     n'est pas pris en compte.
     *
     *  \return false si aucun traitements n'attend de ack ou si un lancement en mode non debug a deja ete effectue, true si le lancement c'est bien passe.
     */
    bool executeOrForwardStepFastInDebugMode(CT_VirtualAbstractStep *beginStep = nullptr);

    /*!
     *  \brief Lancement des traitements en mode debug OU avancer de 1 pas en mode avance automatique
     *
     *  \param beginStep : L'etape a partir de laquelle lancer les traitements. nullptr si on desire lancer les traitements
     *                     a partir de la premiere etape qui a subit des modifications. EVIDEMMENT : Si une etape attend un ack de debug ce parametre
     *                     n'est pas pris en compte.
     *
     *  \return false si aucun traitements n'attend de ack ou si un lancement en mode non debug a deja ete effectue, true si le lancement c'est bien passe.
     */
    bool executeOrForwardStepAutoInDebugMode(CT_VirtualAbstractStep *beginStep = nullptr);

    /**
     * @brief Quit the manual mode of the current step
     */
    bool quitManualMode();

    /*!
     *  \brief Stop tous les traitements
     */
    void stop();

    /*!
     *  \brief Change le nombre de pas a sauter lors de la lecture rapide en mode debug
     */
    bool setFastForwardJumpInDebugMode(int value);

    /**
     * @brief Time to sleep in ms between two call of ack debug mode
     */
    void setTimeToSleepInAutoDebugMode(int timeInMs);

    /**
     * @brief Number of jump to next valid waitForAckIfInDebugMode()
     */
    void setNJumpInAutoDebugMode(int n);

private:

    bool internalExecuteStep(CT_VirtualAbstractStep *beginStep, bool debugMode);
    bool internalExecuteModifyStep(CT_VirtualAbstractStep *beginStep, bool debugMode);

    /*!
     * \brief Connexion/Deconnexion des signaux de l'etape aux signaux/slot de cet objet
     */
    void connectStep(CT_VirtualAbstractStep *step);
    void connectStepBeforeRunning(CT_VirtualAbstractStep *step);
    void disconnectStepAfterRunning(CT_VirtualAbstractStep *step);

    void run();

    CDM_ScriptManagerAbstract   *_scripManager;
    CDM_ActionsManager          *m_actionsManager;
    GuiContextInterface         *m_guiContext;

    CDM_StepManagerOptions      _options;

    ActionType                  _action;
    CT_VirtualAbstractStep                        *_beginStep;
    bool                        _stop;
    bool                        _debugMode;
    bool                        m_debugAutoMode;
    bool                        _force;

    QList<CT_VirtualAbstractStep*>                _stepRootList;
    CT_VirtualAbstractStep*                       m_currentStep;


    QMutex                      _mutex;

    bool recursiveExecuteStep(CT_VirtualAbstractStep &step, CT_VirtualAbstractStep*& restartFromStep, bool force = false);

    void recursiveClearResult(CT_VirtualAbstractStep &step);

    bool ackDebugMode(int jumpNStep, bool callPostWait = true);

private slots:

    void slotStepWaitForAckInDebugMode();
    void slotStepRequiredManualMode();
    void slotStepManualModeCompleted();

    void setDefaultQLocale(QString name);
    void slotRemoveActionsAfterStepExecuted();

    /**
     * @brief Called by a timer to go to next step automatically in debug mode
     */
    void autoAckDebugMode();

signals:

    void stepAdded(CT_VirtualAbstractStep *step);
    void stepInserted(int n, CT_VirtualAbstractStep *step);
    void stepToBeRemoved(CT_VirtualAbstractStep *step);
    void stepBeginExecuted(CT_VirtualAbstractStep *step);
    void stepFinishExecuted(CT_VirtualAbstractStep *step);
    void stepWaitForAckInDebugMode(bool aStepWait);
    void stepRequiredManualMode(CT_VirtualAbstractStep *step);
    void stepInManualMode(bool val);
    void stepQuitManualMode(CT_VirtualAbstractStep *step);
    void stepNeedShowMessage(QString message);

    void resultAdded(const CT_AbstractResult *res);
    void resultToBeClearedFromMemory(const CT_AbstractResult *res);
    void resultToBeRemoved(const CT_AbstractResult *res);

    void started(bool started);
    void loadResultBegin();
    void loadResultInProgress(int progress);
    void completed(bool completed = true);
};

#endif // CDM_STEPMANAGER_H
