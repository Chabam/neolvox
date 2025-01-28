#ifndef CDM_STDIOLISTENER_H
#define CDM_STDIOLISTENER_H

#include "cdm_stdioparser.h"

#include <QObject>
#include <QTextStream>
#include <QSocketNotifier>

class COMPUTREECORESHARED_EXPORT CDM_StdIOListener : public QObject
{
    Q_OBJECT
public:
    explicit CDM_StdIOListener(QObject *parent = 0);
    ~CDM_StdIOListener();

    void setParser(CDM_StdIOParser *parser);

private:
    QSocketNotifier *_notifier;
    QTextStream     *_cin;
    CDM_StdIOParser *_parser;

    void parseData(const QString &s);

public slots:
    void writeStdout(const QString &s);

private slots:
    void readStdin( int );
    
signals:

    void readedLine(const QString &line);
    void writedLine(const QString &line);
};

#endif // CDM_STDIOLISTENER_H
