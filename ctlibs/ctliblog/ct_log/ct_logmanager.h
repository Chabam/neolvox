#ifndef CT_LOGMANAGER_H
#define CT_LOGMANAGER_H

#include "ctliblog_global.h"

#include "ct_temporarylog.h"

#ifndef PS_LOG
#define PS_LOG CT_LogManager::staticInstance()
#endif

class CTLIBLOG_EXPORT CT_LogManager
{
public:
    static CT_LogManager* staticInstance()
    {
        if(UNIQUE_INSTANCE == nullptr)
            UNIQUE_INSTANCE = new CT_LogManager();

        return UNIQUE_INSTANCE;
    }

    void setLogInterfaceToUse(LogInterface* log);

    LogInterface* currentLogInterface() const;

    /**
     * @brief add a message
     *
     * @param severity : the severity (trace, debug, info, etc...)
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addMessage(const int& severity, const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a trace message (hte severity is automatically set to "trace")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addTraceMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a debug message (hte severity is automatically set to "debug")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addDebugMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a info message (hte severity is automatically set to "info")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addInfoMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a warning message (hte severity is automatically set to "warning")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addWarningMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a error message (hte severity is automatically set to "error")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addErrorMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief add a fatal message (hte severity is automatically set to "fatal")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addFatalMessage(const int& type, const QString& s, const QString& filter = "");

    /**
     * @brief Add log listener that must receive message in prioritary and immediately (Elements like File, std::cout, etc...)
     * @warning Add a log listener to this method can reduce the performance of your application if the message is not compute fast
     */
    void addPrioritaryLogListener(const CT_AbstractLogListener* logl);

    /**
     * @brief Add a log listener that must receive message in normal mode (delayed) (Elements of ui like QTextEdit, etc...)
     */
    void addNormalLogListener(const CT_AbstractLogListener* logl);

    /**
     * @brief Remove a log listener (prioritary or normal)
     */
    void removeLogListener(CT_AbstractLogListener* logl);

private:
    static CT_LogManager*       UNIQUE_INSTANCE;

    CT_TemporaryLog             m_tmpLog;
    LogInterface*               m_log;

    CT_LogManager();
};

#endif // CT_LOGMANAGER_H
