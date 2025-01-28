#ifndef CT_LOGMANAGERADAPTERFORSTEP_H
#define CT_LOGMANAGERADAPTERFORSTEP_H

#include "ctlibstep_global.h"

#include <QString>

#define STEP_LOG CT_LogManagerAdapterForStep::staticInstance()

class CTLIBSTEP_EXPORT CT_LogManagerAdapterForStep
{
public:
    static CT_LogManagerAdapterForStep* staticInstance()
    {
        if(UNIQUE_INSTANCE == nullptr)
            UNIQUE_INSTANCE = new CT_LogManagerAdapterForStep();

        return UNIQUE_INSTANCE;
    }

    /**
     * @brief add a message
     *
     * @param severity : the severity (trace, debug, info, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addMessage(const int& severity, const QString& s, const QString& filter = "");

    /**
     * @brief add a trace message (the severity is automatically set to "trace")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addTraceMessage(const QString& s, const QString& filter = "");

    /**
     * @brief add a debug message (the severity is automatically set to "debug")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addDebugMessage(const QString& s, const QString& filter = "");

    /**
     * @brief add a info message (the severity is automatically set to "info")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addInfoMessage(const QString& s, const QString& filter = "");

    /**
     * @brief add a warning message (the severity is automatically set to "warning")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addWarningMessage(const QString& s, const QString& filter = "");

    /**
     * @brief add a error message (the severity is automatically set to "error")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addErrorMessage(const QString& s, const QString& filter = "");

    /**
     * @brief add a fatal message (the severity is automatically set to "fatal")
     *
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    void addFatalMessage(const QString& s, const QString& filter = "");

private:
    static CT_LogManagerAdapterForStep*        UNIQUE_INSTANCE;
};

#endif // CT_LOGMANAGERADAPTERFORSTEP_H
