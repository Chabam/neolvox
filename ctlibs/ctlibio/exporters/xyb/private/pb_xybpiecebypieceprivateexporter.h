#ifndef PB_XYBPIECEBYPIECEPRIVATEEXPORTER_H
#define PB_XYBPIECEBYPIECEPRIVATEEXPORTER_H

#include <QFile>

#include "exporters/xyb/pb_xybexporter.h"
#include "ct_exporter/abstract/ct_abstractpiecebypieceexporter.h"
#include "ct_itemdrawable/ct_scanner.h"

class PB_XYBPieceByPiecePrivateExporter : public CT_AbstractPieceByPieceExporter
{
    Q_OBJECT

private:
    friend class PB_XYBExporter;
    using HandleItemType = PB_XYBExporter::HandleItemType;

    PB_XYBExporter&                     mExporter;
    QString                             mFilePath;
    QFile                               mFile;
    QDataStream*                        mStream;

    PB_XYBPieceByPiecePrivateExporter(PB_XYBExporter& exporter, const QString& filePath);
    ~PB_XYBPieceByPiecePrivateExporter() final;

    bool internalCreateFile() final;

    bool internalOpenFileInAppendMode() final;

    void exportPoint(const size_t& globalIndex, const CT_Point& point, quint16 intensity);

    void internalCloseFile() final;

    bool internalFinalizeFile() final;
};

#endif // PB_XYBPIECEBYPIECEPRIVATEEXPORTER_H
