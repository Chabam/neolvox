#ifndef PB_PROFILEEXPORTER_H
#define PB_PROFILEEXPORTER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/abstract/ct_abstractprofile.h"
#include "ct_exporter/abstract/ct_abstractexporter.h"

class CTLIBIO_EXPORT PB_ProfileExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_ProfileExporter(int subMenuLevel = 0);
    PB_ProfileExporter(const PB_ProfileExporter& other);

    QString displayableName() const final;

    bool isExportEachItemInSeparateFileOptionnal() const final;

    void setItemsToExport(const QList<const CT_AbstractProfile*>& items);

    CT_EXPORTER_DECL_COPY(PB_ProfileExporter)

protected:
        void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

        ExportReturn internalExportToFile() final;

        void clearIterators() final;

        void clearAttributesClouds() final;

    private:
        using HandleItemType = CT_HandleInSingularItem<CT_AbstractProfile>;

        CT_HandleInStdGroup<>               m_hInGroup;
        HandleItemType                      m_hInItem;

        HandleItemType::const_iterator      mIteratorItemBegin;
        HandleItemType::const_iterator      mIteratorItemEnd;

        QList<const CT_AbstractProfile*>    mItems;

        bool exportItem(const CT_AbstractProfile* item, const QString& prePath, const QString& indice);
};

#endif // PB_PROFILEEXPORTER_H
