#include "cdm_stdiolistener.h"

#include <stdio.h>
#include <QProcess>

CDM_StdIOListener::CDM_StdIOListener(QObject *parent) :
    QObject(parent)
{
    _parser = nullptr;

    _cin  = new QTextStream(stdin, QIODevice::ReadOnly);

    // Demand notification when there is data to be read from stdin
    _notifier = new QSocketNotifier(
#ifdef WIN32
                _fileno(stdin)
#else
                fileno(stdin)
#endif
                , QSocketNotifier::Read);
    connect(_notifier, SIGNAL(activated(int)), this, SLOT(readStdin(int)));
}

CDM_StdIOListener::~CDM_StdIOListener()
{
    delete _cin;
    delete _notifier;
}

void CDM_StdIOListener::setParser(CDM_StdIOParser *parser)
{
    if(_parser != nullptr)
        disconnect(_parser, nullptr, this, nullptr);

    _parser = parser;

    connect(_parser, SIGNAL(asyncMessage(QString)), this, SLOT(writeStdout(QString)));
}

void CDM_StdIOListener::readStdin( int )
{
    _notifier->setEnabled(false);

    // Read the data
    QString line;

    int n = _cin->device()->bytesAvailable();

    if(n > 0)
    {
        if(!(line = _cin->readLine()).isEmpty())
        {
            // Parse received data
            parseData(line.remove('\r').remove('\n'));
        }
    }

    _notifier->setEnabled(true);
}

void CDM_StdIOListener::writeStdout(const QString &s)
{
    _notifier->setEnabled(false);

    QString sR = s + "\r\n";

    fprintf(stderr, "%s", sR.toLatin1().constData());

    _notifier->setEnabled(true);

    emit writedLine(s);
}

void CDM_StdIOListener::parseData(const QString &s)
{
    emit readedLine(s);

    if(_parser != nullptr)
    {
        QString response;

        response = _parser->parseLine(s);

        if(!response.isEmpty())
        {
            writeStdout(response);
        }
    }
}
