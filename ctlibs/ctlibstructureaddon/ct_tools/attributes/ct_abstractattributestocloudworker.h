#ifndef CT_ABSTRACTATTRIBUTESTOCLOUDWORKER_H
#define CT_ABSTRACTATTRIBUTESTOCLOUDWORKER_H

#include <QObject>
#include "ctlibstructureaddon_global.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractAttributesToCloudWorker : public QObject
{
    Q_OBJECT

public:
    CT_AbstractAttributesToCloudWorker();
    virtual ~CT_AbstractAttributesToCloudWorker() {}

public slots:
    virtual void apply() = 0;
    virtual void cancel() = 0;

signals:
    void progressChanged(int progress);
    void finished();
};

#endif // CT_ABSTRACTATTRIBUTESTOCLOUDWORKER_H
