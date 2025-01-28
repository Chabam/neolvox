#ifndef PB_POLYGON2DEXPORTER_H
#define PB_POLYGON2DEXPORTER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/ct_polygon2d.h"

class CTLIBIO_EXPORT PB_Polygon2DExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_Polygon2DExporter(int subMenuLevel = 0);
    PB_Polygon2DExporter(const PB_Polygon2DExporter& other);

    QString displayableName() const final;

    void setItemsToExport(const QList<const CT_Polygon2D*>& items);

    CT_EXPORTER_DECL_COPY(PB_Polygon2DExporter)

protected:
        void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) final;

        ExportReturn internalExportToFile() final;

        void clearIterators() final;

        void clearAttributesClouds() final;

    private:
        using HandleItemType = CT_HandleInSingularItem<CT_Polygon2D>;

        CT_HandleInStdGroup<>               m_hInGroup;
        HandleItemType                      m_hInItem;

        HandleItemType::const_iterator      mIteratorItemBegin;
        HandleItemType::const_iterator      mIteratorItemEnd;

        QList<const CT_Polygon2D*>    mItems;

        void exportItem(const CT_Polygon2D* item, QTextStream& stream, const int& nExported, const int& totalToExport);
};

#endif // PB_POLYGON2DEXPORTER_H
