#include "ct_abstractloglistener.h"

CT_AbstractLogListener::CT_AbstractLogListener()
{
}

void CT_AbstractLogListener::setSeverityAccepted(const QVector<int> &sev)
{
    m_sev = sev;
}

const QVector<int>& CT_AbstractLogListener::severityAccepted() const
{
    return m_sev;
}

void CT_AbstractLogListener::setTypeAccepted(const QVector<int> &sev)
{
    m_type = sev;
}

const QVector<int> &CT_AbstractLogListener::typeAccepted() const
{
    return m_type;
}

void CT_AbstractLogListener::setFilter(const QString &filter)
{
    m_filter = filter;
}

const QString& CT_AbstractLogListener::filter() const
{
    return m_filter;
}

bool CT_AbstractLogListener::acceptMessage(const int &severity, const int &type, const QString &filter) const
{
    Q_UNUSED(type)

    return (m_sev.isEmpty() || m_sev.contains(severity)) && (m_type.isEmpty() || m_type.contains(type)) && (m_filter.isEmpty() || (m_filter == filter));
}
