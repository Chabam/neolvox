#ifndef CT_FILELOGLISTENER_H
#define CT_FILELOGLISTENER_H

#include "ctliblog_global.h"

#include "ct_log/abstract/ct_abstractloglistener.h"

#include <QVector>
#include <QFile>

/**
 * @brief A log listener that redirect message to a file
 */
class CTLIBLOG_EXPORT CT_FileLogListener : public CT_AbstractLogListener
{
public:
    CT_FileLogListener();

    /**
     * @brief Set the filepath of the file
     */
    void setFilePath(const QString &filepath);

    /**
     * @brief Filepath of the file
     */
    const QString& filepath() const;

    /**
     * @brief Called when a message is added to the log manager.
     */
    void addMessage(const int &severity, const int &type, const QString &s, const QString &filter);

private:
    QString             m_filepath;
    bool                m_ok;
    QFile               m_file;
};

#endif // CT_FILELOGLISTENER_H
