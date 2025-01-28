#ifndef CDM_LOG_H
#define CDM_LOG_H

#include "loginterface.h"

#include <QMutex>
#include <QSemaphore>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QPair>

class CDM_LogProcess : public QObject
{
    Q_OBJECT

public:
    CDM_LogProcess() { m_finished = true; }

    void init();
    bool isFinished() const;
    void appendMessage(const int &severity, const int &type, const QString &s, const QString &filter);
    int size() const;
    bool isEmpty() const;

    void addLogListener(const CT_AbstractLogListener *logl);
    void removeLogListener(CT_AbstractLogListener *logl);

private:

    struct LogMessage
    {
        LogMessage(const int &severity, const int &type, const QString &s, const QString &filter)
        {
            m_severity = severity;
            m_type = type;
            m_message = s;
            m_filter = filter;
        }

        int m_severity;
        int m_type;
        QString m_message;
        QString m_filter;
    };

    bool                        m_finished;
    bool                        m_cancel;
    QList<LogMessage>           m_messages;
    QList<CT_AbstractLogListener*>        m_otherLogListener;
    mutable QMutex              m_messMutex;
    mutable QMutex              m_logMutex;
    QSemaphore                  m_messWait;

public slots:
    void process();
    void cancel();

signals:
    void finished();
};

#define CDM_DECLARE_NORMAL_MESSAGE void addMessage(const int &severity, const int &type, const QString &s, const QString &filter = ""); /*\
void addMessage(const int &severity, CT_VirtualAbstractStep *type, const QString &s, const QString &filter = ""); \
void addMessage(const int &severity, CT_AbstractAction *type, const QString &s, const QString &filter = ""); \
void addMessage(const int &severity, PluginEntryInterface *type, const QString &s, const QString &filter = ""); \
void addMessage(const int &severity, CT_AbstractStepPlugin *type, const QString &s, const QString &filter = ""); \
void addMessage(const int &severity, CT_AbstractResult *type, const QString &s, const QString &filter = ""); \
void addMessage(const int &severity, CT_AbstractItemDrawable *type, const QString &s, const QString &filter = "");*/

#define CDM_DECLARE_SEVERITY_MESSAGE(_M_TYPE_) void _M_TYPE_(const int &type, const QString &s, const QString &filter = ""); /*\
void _M_TYPE_(CT_VirtualAbstractStep *type, const QString &s, const QString &filter = ""); \
void _M_TYPE_(CT_AbstractAction *type, const QString &s, const QString &filter = ""); \
void _M_TYPE_(PluginEntryInterface *type, const QString &s, const QString &filter = ""); \
void _M_TYPE_(CT_AbstractStepPlugin *type, const QString &s, const QString &filter = ""); \
void _M_TYPE_(CT_AbstractResult *type, const QString &s, const QString &filter = ""); \
void _M_TYPE_(CT_AbstractItemDrawable *type, const QString &s, const QString &filter = "");*/

class CDM_Log : public LogInterface
{
public:
    CDM_Log();
    ~CDM_Log();

    /**
     * @brief Add log listener that must receive message in prioritary and immediately (Elements like File, std::cout, etc...)
     * @warning Add a log listener to this method can reduce the performance of your application if the message is not compute fast
     */
    void addPrioritaryLogListener(const CT_AbstractLogListener *logl);

    /**
     * @brief Add a log listener that must receive message in normal mode (Elements of ui like QTextEdit, etc...)
     */
    void addNormalLogListener(const CT_AbstractLogListener *logl);

    /**
     * @brief Remove a log listener
     */
    void removeLogListener(CT_AbstractLogListener *logl);

    /**
     * @brief Add a message to all listener that accept the severity
     */
    CDM_DECLARE_NORMAL_MESSAGE

    CDM_DECLARE_SEVERITY_MESSAGE(addTraceMessage)
    CDM_DECLARE_SEVERITY_MESSAGE(addDebugMessage)
    CDM_DECLARE_SEVERITY_MESSAGE(addInfoMessage)
    CDM_DECLARE_SEVERITY_MESSAGE(addWarningMessage)
    CDM_DECLARE_SEVERITY_MESSAGE(addErrorMessage)
    CDM_DECLARE_SEVERITY_MESSAGE(addFatalMessage)

private:
    QList<CT_AbstractLogListener*>    m_prioritaryLogListener;
    QMutex                  m_mutex;
    QThread                 m_thread;
    CDM_LogProcess          *m_logProcess;
};

#endif // CDM_LOG_H
