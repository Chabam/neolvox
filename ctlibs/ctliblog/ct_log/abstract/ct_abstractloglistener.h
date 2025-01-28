#ifndef CT_ABSTRACTLOGLISTENER_H
#define CT_ABSTRACTLOGLISTENER_H

#include "ctliblog_global.h"

#include <QVector>
#include <QString>

/**
 * @brief Represent an object that can intercept message send to the LogInterface
 */
class CTLIBLOG_EXPORT CT_AbstractLogListener
{
public:
    CT_AbstractLogListener();

    /**
     * @brief Set the list of severity accepted. So message can be filtered. If
     *        list is empty all severity are accepted.
     */
    void setSeverityAccepted(const QVector<int> &sev);
    const QVector<int>& severityAccepted() const;

    /**
     * @brief Set the list of type accepted. So message can be filtered. If
     *        list is empty all type are accepted.
     */
    void setTypeAccepted(const QVector<int> &sev);
    const QVector<int>& typeAccepted() const;

    /**
     * @brief Set a filter. So message can be filtered. If
     *        filter is empty all message with filter empty or not is accepted.
     */
    void setFilter(const QString &filter);
    const QString& filter() const;

    /**
     * @brief Returns true if the message is accepted by this log listener
     */
    virtual bool acceptMessage(const int &severity, const int &type, const QString &filter) const;

    /**
     * @brief Called when a message is added to the log manager.
     * @warning This method can be called by a thread different than the gui thread !
     *
     * @param severity : the severity (trace, debug, info, etc...)
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addMessage(const int &severity, const int &type, const QString &s, const QString &filter = "") = 0;

private:
    QVector<int>        m_sev;
    QVector<int>        m_type;
    QString             m_filter;
};

#endif // CT_ABSTRACTLOGLISTENER_H
