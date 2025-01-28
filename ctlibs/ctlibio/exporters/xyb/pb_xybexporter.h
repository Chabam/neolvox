#ifndef PB_XYBEXPORTER_H
#define PB_XYBEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"

#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"
#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"

#include <QMutex>

class CTLIBIO_EXPORT PB_XYBExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_XYBExporter(int subMenuLevel = 0);
    PB_XYBExporter(const PB_XYBExporter& other);

    QString displayableName() const override;

    bool setPointsToExport(const QList<CT_AbstractCloudIndex*> &list) override;

    void setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                          const CT_Scanner* scanner = nullptr,
                          const CT_AbstractPointsAttributes* pointsAttributeOfTypeColorOrScalar = nullptr);

    bool canExportPieceByPiece() const override;
    CT_AbstractPieceByPieceExporter* createPieceByPieceExporter(const QString& outputFilepath) override;

    CT_EXPORTER_DECL_COPY(PB_XYBExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    ExportReturn internalExportOnePiece(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters) override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    friend class PB_XYBPieceByPiecePrivateExporter;

    using HandleItemType = CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_hInItem;
    CT_HandleInSingularItem<CT_Scanner, 0>                          m_hInScanner; // optionnal
    CT_HandleInAbstractPointAttribute<0>                            m_hInPointsAttribute; // optionnal

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_AbstractItemDrawableWithPointCloud*>             m_items;
    const CT_Scanner*                                               mScanner;
    const CT_AbstractPointsAttributes*                              mPointsAttributeOfTypeColorOrScalar;

    void exportPoints(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters,
                      const CT_AbstractPointCloudIndex* constPCIndex);
};

#endif // PB_XYBEXPORTER_H
