#ifndef PB_GRID3DEXPORTER_H
#define PB_GRID3DEXPORTER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractgrid3d.h"

class CTLIBIO_EXPORT PB_Grid3DExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_Grid3DExporter(int subMenuLevel = 0);
    PB_Grid3DExporter(const PB_Grid3DExporter& other);

    QString displayableName() const final;

    bool isExportEachItemInSeparateFileOptionnal() const final;

    void setItemsToExport(const QList<const CT_AbstractGrid3D*>& items);

    CT_EXPORTER_DECL_COPY(PB_Grid3DExporter)

protected:
        void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

        ExportReturn internalExportToFile() final;

        void clearIterators() final;

        void clearAttributesClouds() final;

    private:
        using HandleItemType = CT_HandleInSingularItem<CT_AbstractGrid3D>;

        CT_HandleInStdGroup<>               m_hInGroup;
        HandleItemType                      m_hInItem;

        HandleItemType::const_iterator      mIteratorItemBegin;
        HandleItemType::const_iterator      mIteratorItemEnd;

        QList<const CT_AbstractGrid3D*>    mItems;

        bool exportItem(const CT_AbstractGrid3D* item, const QString& prePath, const QString& indice);
};

#endif // PB_GRID3DEXPORTER_H
