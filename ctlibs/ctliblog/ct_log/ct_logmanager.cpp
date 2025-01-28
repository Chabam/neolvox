#include "ct_logmanager.h"

CT_LogManager* CT_LogManager::UNIQUE_INSTANCE = nullptr;


CT_LogManager::CT_LogManager() :
    m_log(&m_tmpLog)
{
}

void CT_LogManager::setLogInterfaceToUse(LogInterface* log)
{
    if(log == nullptr) {
        m_log = &m_tmpLog;
        return;
    }

    if(m_log == &m_tmpLog) {
        for(const CT_AbstractLogListener* logl : m_tmpLog.prioritaryLogListener()) {
            log->addPrioritaryLogListener(logl);
        }

        for(const CT_AbstractLogListener* logl : m_tmpLog.normalLogListener()) {
            log->addNormalLogListener(logl);
        }

        for(const CT_TemporaryLogMessage& mess : m_tmpLog.messages()) {
            log->addMessage(mess.m_severity, mess.m_type, mess.m_message, mess.m_filter);
        }

        m_tmpLog.clear();
    }

    m_log = log;
}

LogInterface* CT_LogManager::currentLogInterface() const
{
    return m_log;
}

void CT_LogManager::addMessage(const int& severity, const int& type, const QString& s, const QString& filter)
{
    m_log->addMessage(severity, type, s, filter);
}

void CT_LogManager::addTraceMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addTraceMessage(type, s, filter);
}

void CT_LogManager::addDebugMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addDebugMessage(type, s, filter);
}

void CT_LogManager::addInfoMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addInfoMessage(type, s, filter);
}

void CT_LogManager::addWarningMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addWarningMessage(type, s, filter);
}

void CT_LogManager::addErrorMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addErrorMessage(type, s, filter);
}

void CT_LogManager::addFatalMessage(const int& type, const QString& s, const QString& filter)
{
    m_log->addFatalMessage(type, s, filter);
}

void CT_LogManager::addPrioritaryLogListener(const CT_AbstractLogListener* logl)
{
    m_log->addPrioritaryLogListener(logl);
}

void CT_LogManager::addNormalLogListener(const CT_AbstractLogListener* logl)
{
    m_log->addNormalLogListener(logl);
}

void CT_LogManager::removeLogListener(CT_AbstractLogListener* logl)
{
    m_log->removeLogListener(logl);
}
