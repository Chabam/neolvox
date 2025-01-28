#ifndef CT_STANDARDEXPORTERSEPARATOR_P_H
#define CT_STANDARDEXPORTERSEPARATOR_P_H

#include "ct_exporter/abstract/ct_abstractexporter.h"

class CT_StandardExporterSeparatorPrivate
{
public:
    CT_StandardExporterSeparatorPrivate() {}

    ~CT_StandardExporterSeparatorPrivate()
    {
        qDeleteAll(_exporters.begin(), _exporters.end());
    }

    QString                         _title;
    QList<CT_AbstractExporter*>     _exporters;
};

#endif // CT_STANDARDEXPORTERSEPARATOR_P_H
