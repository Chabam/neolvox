#ifndef PB_GRID2DEXPORTER_H
#define PB_GRID2DEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"

class CTLIBIO_EXPORT PB_Grid2DExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_Grid2DExporter(int subMenuLevel = 0);
    PB_Grid2DExporter(const PB_Grid2DExporter& other);

    QString displayableName() const final;

    bool isExportEachItemInSeparateFileOptionnal() const final;

    void setItemsToExport(const QList<const CT_AbstractImage2D*>& items);

    CT_EXPORTER_DECL_COPY(PB_Grid2DExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

    ExportReturn internalExportToFile() final;

    void clearIterators() final;

    void clearAttributesClouds() final;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_AbstractImage2D>;

    CT_HandleInStdGroup<>               m_hInGroup;
    HandleItemType                      m_hInItem;

    HandleItemType::const_iterator      mIteratorItemBegin;
    HandleItemType::const_iterator      mIteratorItemEnd;

    QList<const CT_AbstractImage2D*>    mItems;

    bool exportItem(const CT_AbstractImage2D* item, const QString& prePath, const QString& indice);
};

#endif // PB_GRID2DEXPORTER_H
