#ifndef CT_TEMPORARYLOG_H
#define CT_TEMPORARYLOG_H

#include "loginterface.h"

#include <QList>

class CT_TemporaryLogMessage
{
public:
    int         m_severity;
    int         m_type;
    QString     m_message;
    QString     m_filter;
};

class CT_TemporaryLog : public LogInterface
{
public:
    void addMessage(const int& severity, const int& type, const QString& s, const QString& filter = "") override;

    void addTraceMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addDebugMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addInfoMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addWarningMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addErrorMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addFatalMessage(const int& type, const QString& s, const QString& filter = "") override;

    void addPrioritaryLogListener(const CT_AbstractLogListener* logl) override;

    void addNormalLogListener(const CT_AbstractLogListener* logl) override;

    void removeLogListener(CT_AbstractLogListener* logl) override;

    const QList<CT_AbstractLogListener*>& prioritaryLogListener() const;
    const QList<CT_AbstractLogListener*>& normalLogListener() const;
    const QList<CT_TemporaryLogMessage>& messages() const;

    void clear();

private:
    QList<CT_AbstractLogListener*>  m_prioritaryLogListener;
    QList<CT_AbstractLogListener*>  m_normalLogListener;
    QList<CT_TemporaryLogMessage>   m_messages;
};

#endif // CT_TEMPORARYLOG_H
