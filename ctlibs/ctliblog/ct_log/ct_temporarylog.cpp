#include "ct_temporarylog.h"

void CT_TemporaryLog::addMessage(const int &severity, const int &type, const QString &s, const QString &filter)
{
    CT_TemporaryLogMessage m;
    m.m_severity = severity;
    m.m_type = type;
    m.m_message = s;
    m.m_filter = filter;

    m_messages.append(m);
}

void CT_TemporaryLog::addTraceMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::trace, type, s, filter); }
void CT_TemporaryLog::addDebugMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::debug, type, s, filter); }
void CT_TemporaryLog::addInfoMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::info, type, s, filter); }
void CT_TemporaryLog::addWarningMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::warning, type, s, filter); }
void CT_TemporaryLog::addErrorMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::error, type, s, filter); }
void CT_TemporaryLog::addFatalMessage(const int &type, const QString &s, const QString &filter) { addMessage(LogInterface::fatal, type, s, filter); }

void CT_TemporaryLog::addPrioritaryLogListener(const CT_AbstractLogListener* logl)
{
    m_prioritaryLogListener.append(const_cast<CT_AbstractLogListener*>(logl));
}

void CT_TemporaryLog::addNormalLogListener(const CT_AbstractLogListener* logl)
{
    m_normalLogListener.append(const_cast<CT_AbstractLogListener*>(logl));
}

void CT_TemporaryLog::removeLogListener(CT_AbstractLogListener* logl)
{
    m_prioritaryLogListener.removeOne(logl);
    m_normalLogListener.removeOne(logl);
}

const QList<CT_AbstractLogListener*>& CT_TemporaryLog::prioritaryLogListener() const
{
    return m_prioritaryLogListener;
}

const QList<CT_AbstractLogListener*>& CT_TemporaryLog::normalLogListener() const
{
    return m_normalLogListener;
}

const QList<CT_TemporaryLogMessage> &CT_TemporaryLog::messages() const
{
    return m_messages;
}

void CT_TemporaryLog::clear()
{
    m_messages.clear();
    m_prioritaryLogListener.clear();
    m_normalLogListener.clear();
}
