#ifndef PB_GROUPDATAEXPORTER_H
#define PB_GROUPDATAEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"

class CTLIBIO_EXPORT PB_GroupDataExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_GroupDataExporter(int subMenuLevel = 0);
    PB_GroupDataExporter(const PB_GroupDataExporter& other);

    QString displayableName() const final;

    void setGroupsToExport(const QList<const CT_StandardItemGroup*>& list);

    CT_EXPORTER_DECL_COPY(PB_GroupDataExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

    ExportReturn internalExportToFile() final;

    void clearIterators() final;

    void clearAttributesClouds() final;

private:
    using HandleGroupType = CT_HandleInStdGroup<1,-1>;

    HandleGroupType                      m_hInGroup;

    HandleGroupType::const_iterator      mIteratorGroupBegin;
    HandleGroupType::const_iterator      mIteratorGroupEnd;


    QList<const CT_StandardItemGroup*>  mGroups;
};

#endif // PB_GROUPDATAEXPORTER_H
