#ifndef LVOX3_GRID3DEXPORTER_H
#define LVOX3_GRID3DEXPORTER_H

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "tools/3dgrid/abstract/lvox3_abstractgrid3d.h"

class LVOX3_Grid3DExporter : public CT_AbstractExporter
{
    Q_OBJECT

public:
    LVOX3_Grid3DExporter(int subMenuLevel = 0);
    LVOX3_Grid3DExporter(const LVOX3_Grid3DExporter& other);

    QString displayableName() const final;

    bool isExportEachItemInSeparateFileOptionnal() const final;

    void setItemsToExport(const QList<const LVOX3_AbstractGrid3D*>& items);

    CT_EXPORTER_DECL_COPY(LVOX3_Grid3DExporter)

    protected:
        void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

        ExportReturn internalExportToFile() final;

        void clearIterators() final;

        void clearAttributesClouds() final;

private:
    using HandleItemType = CT_HandleInSingularItem<LVOX3_AbstractGrid3D>;

    CT_HandleInStdGroup<>               m_hInGroup;
    HandleItemType                      m_hInItem;

    HandleItemType::const_iterator      mIteratorItemBegin;
    HandleItemType::const_iterator      mIteratorItemEnd;

    QList<const LVOX3_AbstractGrid3D*>  mItems;

    bool exportItem(const LVOX3_AbstractGrid3D* item, const QString& prePath, const QString& indice);

};

#endif // LVOX3_GRID3DEXPORTER_
