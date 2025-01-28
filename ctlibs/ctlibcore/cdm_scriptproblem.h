#ifndef CDM_SCRIPTPROBLEM_H
#define CDM_SCRIPTPROBLEM_H

#include "computreeCore_global.h"

#include "ct_step/abstract/ct_virtualabstractstep.h"

class CDM_PluginManager;

class COMPUTREECORESHARED_EXPORT CDM_ScriptProblem
{
public:
    enum TypeOfProblem {
        TOP_PluginNotInformed,                      // The name of the plugin was not informed (typically in very old script). Solution is to inform the plugin to use, use method "setSolutionPluginToUse" or leave it empty and use method "setSolutionKeepSteps" to inform what to do.
        TOP_PluginNotFound,                         // The plugin was not found. Solution is to inform the plugin to use, use method "setSolutionPluginToUse" or to set if the previous steps are kept or if the entire script is not loaded. To do this last solution use method "setSolutionKeepSteps" to inform what to do.
        TOP_StepNotFound,                           // The step was not found. Solution is to set the step to use in place of the not founded step, use method "setSolutionUseStep". If you leave it empty use the method "setSolutionKeepSteps" to inform what to do.
        TOP_StepCanNotBeConfigured,                 // The step can not be configured with the script, use method "solutionConfigureStep" and configure the step manually. If you don't want to configure the step use the method "setSolutionKeepSteps" to inform what to do.
        TOP_NoSolution,                             // Errors that no have solution. Use method "setSolutionKeepSteps" to inform what to do.
        TOP_StepPreParamsCanNotBeConfigured,        // The step can not be pre-configured with the script, use method "solutionConfigureStep" and configure the step manually. If you don't want to configure the step use the method "setSolutionKeepSteps" to inform what to do.
        TOP_StepInputParamsCanNotBeConfigured,      // The step can not be configured with the script, use method "solutionConfigureStep" and configure the step manually. If you don't want to configure the step use the method "setSolutionKeepSteps" to inform what to do.
        TOP_StepPostParamsCanNotBeConfigured        // The step can not be configured with the script, use method "solutionConfigureStep" and configure the step manually. If you don't want to configure the step use the method "setSolutionKeepSteps" to inform what to do.

    };

    CDM_ScriptProblem(CDM_PluginManager &pluginManager,
                      TypeOfProblem type,
                      QString error,
                      CT_VirtualAbstractStep *configureStep,
                      CT_VirtualAbstractStep *parentStep,
                      int currentPluginIndexUsed,
                      QString pluginName = "",
                      QString stepName = "");

    /**
     * @brief Return the type of problem
     */
    TypeOfProblem getProblemType() const;

    /**
     * @brief Returns a displayable string that inform the problem
     */
    QString getProblemString() const;

    /**
     * @brief Return the plugin's name if problem is different of type TOP_NoSolution
     */
    QString getProblemPluginName() const;

    /**
     * @brief Return the plugin's name if problem is of type TOP_StepNotFound or TOP_StepCanNotBeConfigured
     */
    QString getProblemStepName() const;

    /**
     * @brief Use this method for any type of problem if you do not use the first solution.
     * @param enable : set to true to keep previous steps loaded or false if you don't want to load the script. FALSE by default.
     */
    void setSolutionKeepSteps(bool enable);
    bool getSolutionKeepSteps() const;

    /**
     * @brief Set the solution if TypeOfProblem is "TOP_PluginNotInformed"
     * @param pluginIndex : set the index of the plugin to use. If you leave it empty use the method "setSolutionKeepSteps" to inform what to do.
     * @return false if index is invalid
     */
    bool setSolutionPluginToUse(int pluginIndex);
    int getSolutionPluginToUse() const;

    /**
     * @brief Set the solution if TypeOfProblem is "TOP_StepNotFound"
     * @param step : the step to use
     */
    void setSolutionUseStep(CT_VirtualAbstractStep *step);
    CT_VirtualAbstractStep* getSolutionUseStep() const;

    /**
     * @brief Returns the parent step if problem is "TOP_StepNotFound" or "TOP_StepCanNotBeConfigured" of the step that have a problem
     */
    CT_VirtualAbstractStep* getParentStep() const;

    /**
     * @brief Call the method to configure step if TypeOfProblem is "TOP_StepCanNotBeConfigured"
     * @return true if step was configured correctly
     */
    bool solutionConfigureStep();

    /**
     * @brief Returns true if the step to configure has been well configured
     */
    bool isConfigureStepWellConfigured() const;

private:
    const CDM_PluginManager &m_pluginManager;
    TypeOfProblem           m_problem;
    QString                 m_error;
    bool                    m_keepSteps;
    int                     m_pluginIndex;
    QString                 m_pluginName;
    QString                 m_stepName;

    CT_VirtualAbstractStep  *m_useStep;
    CT_VirtualAbstractStep  *m_configureStep;
    bool                    m_configureStepWellConfigured;
    CT_VirtualAbstractStep  *m_parentStep;

    /**
     * @brief Call the method to configure step if TypeOfProblem is "TOP_StepPreParamsCanNotBeConfigured"
     * @return true if user has not canceled the configuration
     */
    bool solutionConfigureStepPreParams();

    /**
     * @brief Call the method to configure step if TypeOfProblem is "TOP_StepInputParamsCanNotBeConfigured"
     * @return true if user has not canceled the configuration
     */
    bool solutionConfigureStepInputParams();

    /**
     * @brief Call the method to configure step if TypeOfProblem is "TOP_StepPostParamsCanNotBeConfigured"
     * @return true if user has not canceled the configuration
     */
    bool solutionConfigureStepPostParams();
};

#endif // CDM_SCRIPTPROBLEM_H
