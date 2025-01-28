#include "ct_standardexporterseparator.h"

#include "private/ct_standardexporterseparator_p.h"

CT_StandardExporterSeparator::CT_StandardExporterSeparator(const QString &title) :
    d_ptr(new CT_StandardExporterSeparatorPrivate)
{
    Q_D(CT_StandardExporterSeparator);
    d->_title = title;
}

CT_StandardExporterSeparator::~CT_StandardExporterSeparator()
{
}

void CT_StandardExporterSeparator::addExporter(CT_AbstractExporter* exporter)
{
    Q_D(CT_StandardExporterSeparator);
    d->_exporters.append(exporter);
}

QString CT_StandardExporterSeparator::title() const
{
    Q_D(const CT_StandardExporterSeparator);
    return d->_title;
}

const QList<CT_AbstractExporter*>& CT_StandardExporterSeparator::exporters() const
{
    Q_D(const CT_StandardExporterSeparator);
    return (const QList<CT_AbstractExporter*>&)d->_exporters;
}
