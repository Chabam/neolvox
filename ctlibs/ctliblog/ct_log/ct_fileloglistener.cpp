#include "ct_fileloglistener.h"

#include <QTextStream>

CT_FileLogListener::CT_FileLogListener() : CT_AbstractLogListener()
{
}

void CT_FileLogListener::setFilePath(const QString &filepath)
{
    m_filepath = filepath;
    m_ok = false;
    m_file.setFileName(m_filepath);

    if(m_file.open(QFile::WriteOnly))
    {
        m_ok = true;
        m_file.close();
    }
}

const QString& CT_FileLogListener::filepath() const
{
    return m_filepath;
}

void CT_FileLogListener::addMessage(const int &severity, const int &type, const QString &s, const QString &filter)
{
    if(m_ok)
    {
        if(acceptMessage(severity, type, filter))
        {
            if(m_file.open(QFile::Append))
            {
                m_ok = true;

                QTextStream stream(&m_file);

                stream << s << "\r\n";

                m_file.close();
            }
            else
            {
                m_ok = false;
            }
        }
    }
}
