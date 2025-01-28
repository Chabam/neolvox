#include "cdm_log.h"

#include "ct_log/abstract/ct_abstractloglistener.h"

#include <QMutex>
#include <QWaitCondition>

#define CDM_IMPLEMENT_NORMAL_MESSAGE
/*
void CDM_Log::addMessage(const int &severity, CT_VirtualAbstractStep *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::step, s, filter); } \
void CDM_Log::addMessage(const int &severity, CT_AbstractAction *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::action, s, filter); } \
void CDM_Log::addMessage(const int &severity, PluginEntryInterface *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::plugin, s, filter); } \
void CDM_Log::addMessage(const int &severity, CT_AbstractStepPlugin *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::plugin, s, filter); } \
void CDM_Log::addMessage(const int &severity, CT_AbstractResult *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::result, s, filter); } \
void CDM_Log::addMessage(const int &severity, CT_AbstractItemDrawable *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(severity, LogInterface::itemdrawable, s, filter); }
*/
#define CDM_IMPLEMENT_SEVERITY_MESSAGE(_M_TYPE_, SEVTYPE) void CDM_Log::_M_TYPE_(const int &type, const QString &s, const QString &filter) { addMessage(SEVTYPE, type, s, filter); }
/*
void CDM_Log::_M_TYPE_(CT_VirtualAbstractStep *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::step, s, filter); } \
void CDM_Log::_M_TYPE_(CT_AbstractAction *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::action, s, filter); } \
void CDM_Log::_M_TYPE_(PluginEntryInterface *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::plugin, s, filter); } \
void CDM_Log::_M_TYPE_(CT_AbstractStepPlugin *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::plugin, s, filter); } \
void CDM_Log::_M_TYPE_(CT_AbstractResult *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::result, s, filter); } \
void CDM_Log::_M_TYPE_(CT_AbstractItemDrawable *type, const QString &s, const QString &filter) { Q_UNUSED(type) addMessage(SEVTYPE, LogInterface::itemdrawable, s, filter); }
*/
CDM_Log::CDM_Log()
{
    m_logProcess = new CDM_LogProcess();
    m_logProcess->moveToThread(&m_thread);
    m_logProcess->init();

    QObject::connect(&m_thread, SIGNAL(started()), m_logProcess, SLOT(process()));
    QObject::connect(m_logProcess, SIGNAL(finished()), &m_thread, SLOT(quit()));

    m_thread.start();
}

CDM_Log::~CDM_Log()
{
    m_logProcess->cancel();

    QMutex mutex;

    while(!m_thread.isFinished()
          && !m_logProcess->isFinished())
    {
        mutex.lock();
        QWaitCondition().wait(&mutex, 100);
        mutex.unlock();
    }

    m_thread.quit();
    m_thread.wait();

    delete m_logProcess;
}

void CDM_Log::addPrioritaryLogListener(const CT_AbstractLogListener *logl)
{
    QMutexLocker locker(&m_mutex);

    m_prioritaryLogListener.append((CT_AbstractLogListener*)logl);
}

void CDM_Log::addNormalLogListener(const CT_AbstractLogListener *logl)
{
    m_logProcess->addLogListener(logl);
}

void CDM_Log::removeLogListener(CT_AbstractLogListener *logl)
{
    QMutexLocker locker(&m_mutex);

    if(m_prioritaryLogListener.removeOne(logl))
        return;

    m_logProcess->removeLogListener(logl);
}

void CDM_Log::addMessage(const int &severity, const int &type, const QString &s, const QString &filter)
{
    QMutexLocker locker(&m_mutex);

    if(!m_prioritaryLogListener.isEmpty())
    {
        QListIterator<CT_AbstractLogListener*> it(m_prioritaryLogListener);

        while(it.hasNext())
        {
            CT_AbstractLogListener *log = it.next();

            log->addMessage(severity, type, s, filter);
        }
    }

    m_logProcess->appendMessage(severity, type, s, filter);
}

CDM_IMPLEMENT_NORMAL_MESSAGE

CDM_IMPLEMENT_SEVERITY_MESSAGE(addTraceMessage, LogInterface::trace)
CDM_IMPLEMENT_SEVERITY_MESSAGE(addDebugMessage, LogInterface::debug)
CDM_IMPLEMENT_SEVERITY_MESSAGE(addInfoMessage, LogInterface::info)
CDM_IMPLEMENT_SEVERITY_MESSAGE(addWarningMessage, LogInterface::warning)
CDM_IMPLEMENT_SEVERITY_MESSAGE(addErrorMessage, LogInterface::error)
CDM_IMPLEMENT_SEVERITY_MESSAGE(addFatalMessage, LogInterface::fatal)

void CDM_LogProcess::init()
{
    m_cancel = false;
    m_finished = true;
}

bool CDM_LogProcess::isFinished() const
{
    return m_finished;
}

void CDM_LogProcess::appendMessage(const int &severity, const int &type, const QString &s, const QString &filter)
{
    QMutexLocker locker(&m_messMutex);

    if(!m_otherLogListener.isEmpty())
    {
        m_messages.append(LogMessage(severity, type, s, filter));
        m_messWait.release();
    }
}

int CDM_LogProcess::size() const
{
    QMutexLocker locker(&m_messMutex);

    return m_messages.size();
}

bool CDM_LogProcess::isEmpty() const
{
    QMutexLocker locker(&m_messMutex);

    return m_messages.isEmpty();
}

void CDM_LogProcess::addLogListener(const CT_AbstractLogListener *logl)
{
    QMutexLocker locker(&m_logMutex);

    m_otherLogListener.append((CT_AbstractLogListener*)logl);
}

void CDM_LogProcess::removeLogListener(CT_AbstractLogListener *logl)
{
    QMutexLocker locker(&m_logMutex);

    m_otherLogListener.removeOne(logl);
}

void CDM_LogProcess::process()
{
    m_finished = false;

    QListIterator<CT_AbstractLogListener*> it(m_otherLogListener);

    while(!m_cancel)
    {
        // wait while a new message is available
        m_messWait.acquire();

        if(!m_cancel)
        {
            m_messMutex.lock();
            const LogMessage &lm = m_messages.takeFirst();
            m_messMutex.unlock();

            m_logMutex.lock();
            it = m_otherLogListener;

            while(it.hasNext()
                  && !m_cancel)
            {
                CT_AbstractLogListener *log = it.next();

                log->addMessage(lm.m_severity, lm.m_type, lm.m_message, lm.m_filter);
            }
            m_logMutex.unlock();
        }
    }

    emit finished();
    m_finished = true;
}

void CDM_LogProcess::cancel()
{
    m_cancel = true;
    m_messWait.release();
}
