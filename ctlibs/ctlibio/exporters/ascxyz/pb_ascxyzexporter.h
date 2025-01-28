#ifndef PB_ASCXYZEXPORTER_H
#define PB_ASCXYZEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"

class CTLIBIO_EXPORT PB_ASCXYZExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_ASCXYZExporter(int subMenuLevel = 0);
    PB_ASCXYZExporter(const PB_ASCXYZExporter& other);

    QString displayableName() const override;

    void setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items);

    CT_EXPORTER_DECL_COPY(PB_ASCXYZExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_hInItem;

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_AbstractItemDrawableWithPointCloud*>             mItems;

    void exportItem(const CT_AbstractItemDrawableWithPointCloud* item, QTextStream& stream, const int& nExported, const int& totalToExport);
};

#endif // PB_ASCXYZEXPORTER_H
