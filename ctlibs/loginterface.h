#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

#include <QString>

class CT_AbstractLogListener;

/**
 * @brief The log interface
 */
class LogInterface
{
public:

    enum DefaultSeverity
    {
        trace = 0,
        debug,
        info,
        warning,
        error,
        fatal
    };

    enum DefaultType
    {
        core = 0,
        gui,
        plugin,
        step,
        result,
        itemdrawable,
        action,
        reader,
        exporter,
        filter,
        metric,
        unknow
    };

    virtual ~LogInterface() {}

    /**
     * @brief add a message
     *
     * @param severity : the severity (trace, debug, info, etc...)
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addMessage(const int& severity, const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a trace message (hte severity is automatically set to "trace")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addTraceMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a debug message (hte severity is automatically set to "debug")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addDebugMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a info message (hte severity is automatically set to "info")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addInfoMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a warning message (hte severity is automatically set to "warning")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addWarningMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a error message (hte severity is automatically set to "error")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addErrorMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief add a fatal message (hte severity is automatically set to "fatal")
     *
     * @param type : type of element that add a message to the log (action, step, etc...)
     * @param s : message
     * @param filter : a filter if you want to filter message for your plugin per example
     */
    virtual void addFatalMessage(const int& type, const QString& s, const QString& filter = "") = 0;

    /**
     * @brief Add log listener that must receive message in prioritary and immediately (Elements like File, std::cout, etc...)
     * @warning Add a log listener to this method can reduce the performance of your application if the message is not compute fast
     */
    virtual void addPrioritaryLogListener(const CT_AbstractLogListener* logl) = 0;

    /**
     * @brief Add a log listener that must receive message in normal mode (delayed) (Elements of ui like QTextEdit, etc...)
     */
    virtual void addNormalLogListener(const CT_AbstractLogListener* logl) = 0;

    /**
     * @brief Remove a log listener (prioritary or normal)
     */
    virtual void removeLogListener(CT_AbstractLogListener* logl) = 0;
};

#endif // LOGINTERFACE_H
