#ifndef PB_CSVEXPORTER_H
#define PB_CSVEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"

class PB_CSVExporterConfiguration;
class PB_CSVExporterColumn;
class CT_OutAbstractSingularItemModel;
class CT_OutAbstractItemAttributeModel;

class CTLIBIO_EXPORT PB_CSVExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_CSVExporter(int subMenuLevel = 0);
    PB_CSVExporter(const PB_CSVExporter& other);
    ~PB_CSVExporter() final;

    QString displayableName() const override;

    void setItemsToExport(const QList<const CT_AbstractSingularItemDrawable*>& list);

    bool configure() override;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    CT_EXPORTER_DECL_COPY(PB_CSVExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_AbstractSingularItemDrawable>;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_hInItem;

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_AbstractSingularItemDrawable*>                   mItems;
    QList<const CT_OutAbstractSingularItemModel*>                   mItemsModels;
    QMultiHash<const CT_OutAbstractSingularItemModel*, const CT_AbstractSingularItemDrawable*>  mItemsToExportByModel;
    PB_CSVExporterConfiguration                                                         *_configuration;

    void constructItemsModels();
    void constructItemsToExport();

    QList<PB_CSVExporterColumn*> writeHeader(QTextStream& stream);

    const CT_OutAbstractSingularItemModel* itemModelByUniqueIndex(const CT_OutAbstractModel::UniqueIndexType& uid) const;
    const CT_OutAbstractItemAttributeModel* itemAttributeModelByUniqueIndex(const CT_OutAbstractSingularItemModel *sItem, const CT_OutAbstractModel::UniqueIndexType& uid) const;
};

#endif // PB_CSVEXPORTER_H
