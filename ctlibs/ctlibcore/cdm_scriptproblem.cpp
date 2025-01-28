#include "cdm_scriptproblem.h"

#include "cdm_pluginmanager.h"
#include "ct_abstractstepplugin.h"

CDM_ScriptProblem::CDM_ScriptProblem(CDM_PluginManager &pluginManager,
                                     TypeOfProblem type,
                                     QString error,
                                     CT_VirtualAbstractStep *configureStep,
                                     CT_VirtualAbstractStep *parentStep,
                                     int currentPluginIndexUsed,
                                     QString pluginName,
                                     QString stepName) : m_pluginManager(pluginManager)
{
    m_problem = type;
    m_error = error;
    m_configureStep = configureStep;
    m_configureStepWellConfigured = false;
    m_parentStep = parentStep;

    m_keepSteps = true;
    m_pluginIndex = currentPluginIndexUsed;
    m_useStep = nullptr;

    m_pluginName = pluginName;
    m_stepName = stepName;

    if(m_pluginName.isEmpty()) {
        if(m_configureStep != nullptr)
            m_pluginName = pluginManager.getPluginName(static_cast<CT_AbstractStepPlugin*>(m_configureStep->plugin()));
    }

    if(m_stepName.isEmpty()) {
        if(m_configureStep != nullptr)
            m_stepName = m_configureStep->name();
    }
}

CDM_ScriptProblem::TypeOfProblem CDM_ScriptProblem::getProblemType() const
{
    return m_problem;
}

QString CDM_ScriptProblem::getProblemString() const
{
    return m_error;
}

QString CDM_ScriptProblem::getProblemPluginName() const
{
    return m_pluginName;
}

QString CDM_ScriptProblem::getProblemStepName() const
{
    return m_stepName;
}

void CDM_ScriptProblem::setSolutionKeepSteps(bool enable)
{
    m_keepSteps = enable;
}

bool CDM_ScriptProblem::getSolutionKeepSteps() const
{
    return m_keepSteps;
}

bool CDM_ScriptProblem::setSolutionPluginToUse(int pluginIndex)
{
    if((pluginIndex == -1) || ((pluginIndex >= 0) && (pluginIndex < m_pluginManager.countPluginLoaded()))) {
        m_pluginIndex = pluginIndex;
        return true;
    }

    return false;
}

int CDM_ScriptProblem::getSolutionPluginToUse() const
{
    return m_pluginIndex;
}

void CDM_ScriptProblem::setSolutionUseStep(CT_VirtualAbstractStep *step)
{
    m_useStep = step;
}

CT_VirtualAbstractStep *CDM_ScriptProblem::getSolutionUseStep() const
{
    return m_useStep;
}

CT_VirtualAbstractStep *CDM_ScriptProblem::getParentStep() const
{
    return m_parentStep;
}

bool CDM_ScriptProblem::solutionConfigureStep()
{
    if(getProblemType() == TOP_StepCanNotBeConfigured) {
        if(solutionConfigureStepPreParams()) {
            m_configureStepWellConfigured = m_configureStep->finalizeConfiguration();
            return m_configureStepWellConfigured;
        }
    } else if(getProblemType() == TOP_StepPreParamsCanNotBeConfigured)
        return solutionConfigureStepPreParams();
    else if(getProblemType() == TOP_StepInputParamsCanNotBeConfigured)
        return solutionConfigureStepInputParams();
    else if(getProblemType() == TOP_StepPostParamsCanNotBeConfigured)
        return solutionConfigureStepPostParams();

    return false;
}

bool CDM_ScriptProblem::solutionConfigureStepPreParams()
{
    if(m_configureStep != nullptr)
    {
        if(m_configureStep->showPreInputConfigurationDialog())
        {
            if(m_configureStep->showInputResultConfigurationDialog())
            {
                if(m_configureStep->showPostConfigurationDialog())
                    m_configureStepWellConfigured = true;
            }
        }
    }

    return m_configureStepWellConfigured;
}

bool CDM_ScriptProblem::solutionConfigureStepInputParams()
{
    if(m_configureStep != nullptr)
    {
        if(m_configureStep->showInputResultConfigurationDialog())
        {
            if(m_configureStep->showPostConfigurationDialog())
                m_configureStepWellConfigured = true;
        }
    }

    return m_configureStepWellConfigured;
}

bool CDM_ScriptProblem::solutionConfigureStepPostParams()
{
    if(m_configureStep != nullptr)
    {
        if(m_configureStep->showPostConfigurationDialog())
            m_configureStepWellConfigured = true;
    }

    return m_configureStepWellConfigured;
}

bool CDM_ScriptProblem::isConfigureStepWellConfigured() const
{
    return m_configureStepWellConfigured;
}
