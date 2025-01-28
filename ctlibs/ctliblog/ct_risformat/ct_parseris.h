#ifndef PARSERIS_H
#define PARSERIS_H

#include "ctliblog_global.h"

#include <QObject>

class CTLIBLOG_EXPORT CT_ParseRIS : public QObject
{
    Q_OBJECT
public:

    static QString parseRIS(QString ris);

};

#endif // PARSERIS_H
