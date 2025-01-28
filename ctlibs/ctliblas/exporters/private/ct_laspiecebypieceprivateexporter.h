#ifndef CT_LASPIECEBYPIECEPRIVATEEXPORTER_H
#define CT_LASPIECEBYPIECEPRIVATEEXPORTER_H

#include "exporters/ct_exporter_las.h"
#include "ct_exporter/abstract/ct_abstractpiecebypieceexporter.h"

class CT_LASPieceByPiecePrivateExporter : public CT_AbstractPieceByPieceExporter
{
    Q_OBJECT

public:
    ~CT_LASPieceByPiecePrivateExporter() final;

protected:
    bool internalCreateFile() final;

    bool internalOpenFileInAppendMode() final;

    bool initializeHeader(const quint8 format, const quint16 pointDataLength);

    void computePointForHeader(const CT_AbstractPointAttributesScalar* returnNumberValues, const size_t& gi, const CT_Point& point);

    bool finalizeHeaderAndWritePoints();

    void internalCloseFile() final;

    bool internalFinalizeFile() final;

private:
    friend class CT_Exporter_LAS;
    using HandleItemType = CT_Exporter_LAS::HandleItemType;

    struct HeaderBackup {
        HeaderBackup() : header(nullptr), nFiles(0) {}
        ~HeaderBackup() { delete header; }

        bool isHeaderSameForExport(const CT_LASHeader* header) const;
        QString previousFilePath() const;
        QString currentFilePath() const;
        QString generateFilePath(int n) const;

        const CT_LASHeader*         header;
        QString                     dirPath;
        int                         nFiles;
    };

    CT_Exporter_LAS&                            mExporter;
    QString                                     mFilePath;
    QString                                     mBaseFilePath;
    QList<HeaderBackup*>                        mHeaders;

    bool                                        mReturnNumberSet;
    CT_LASHeader*                               mHeader;

    CT_PointCloudIndexVector                    mPointsToWrite;

    CT_LASPieceByPiecePrivateExporter(CT_Exporter_LAS& exporter, const QString& filePath);

    /**
     * @brief Write the header of the file
     * @param stream : the data stream of the file (it will be seek to 0 automatically and set to LittleEndian)
     * @param header : the header to write
     */
    bool writeHeader(QDataStream& stream,
                     CT_LASHeader *header);

    /**
     * @brief Search an header in the list of header backuped and if has found one return the backup that match with it
     */
    HeaderBackup* createOrGetHeaderBackupForHeader(const CT_LASHeader* header) const;

    /**
     * @brief When you have finished to write the file call this method to save or delete the header backup
     */
    void saveOrDeleteHeaderBackup(HeaderBackup* backup, CT_LASHeader* header, bool writeOfFileIsOk);

    /**
     * @brief Clear all backuped headers
     */
    void clearAllBackupedHeaders();
};

#endif // CT_LASPIECEBYPIECEPRIVATEEXPORTER_H
