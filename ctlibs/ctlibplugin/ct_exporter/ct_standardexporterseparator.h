#ifndef CT_STANDARDEXPORTERSEPARATOR_H
#define CT_STANDARDEXPORTERSEPARATOR_H

#include "ctlibplugin_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"

class CT_StandardExporterSeparatorPrivate;

class CTLIBPLUGIN_EXPORT CT_StandardExporterSeparator
{
public:
    CT_StandardExporterSeparator(const QString& title);
    ~CT_StandardExporterSeparator();

    /**
      * \brief Ajoute un exportateur à la liste
      */
    void addExporter(CT_AbstractExporter* exporter);

    QString title() const;

    const QList<CT_AbstractExporter*>& exporters() const;

protected:
    const QScopedPointer<CT_StandardExporterSeparatorPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(CT_StandardExporterSeparator)
};

#endif // CT_STANDARDEXPORTERSEPARATOR_H
