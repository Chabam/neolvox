#ifndef CT_LASHEADER_H
#define CT_LASHEADER_H

#include "ctliblas/ctliblas_global.h"
#include "ct_itemdrawable/ct_indexablepointfileheader.h"

#include <QDataStream>

class CTLIBLAS_EXPORT CT_LASHeader : public CT_IndexablePointFileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_LASHeader, CT_IndexablePointFileHeader, LAS Header)

    using SuperClass = CT_IndexablePointFileHeader;

public:
    enum CompareOption {
        CO_Format = 1,
        CO_Version = 2,
        CO_Offset = 4,
        CO_Scale = 8,
        CO_All = 15
    };

    CT_LASHeader();

    CT_LASHeader(const CT_LASHeader& other) = default;

    size_t nPoints() const override {return getPointsRecordCount();}


    CT_ITEM_COPY_IMP(CT_LASHeader)

    char        m_fileSignature[4];
    quint16     m_fileSourceID;
    quint16     m_globalEncoding;
    quint32     m_projectIDGuidData1;
    quint16     m_projectIDGuidData2;
    quint16     m_projectIDGuidData3;
    quint64     m_projectIDGuidData4;
    quint8      m_versionMajor;
    quint8      m_versionMinor;
    char        m_systemID[32];
    char        m_sofwareID[32];
    quint16     m_fileCreationDayOfYear;
    quint16     m_fileCreationYear;
    quint16     m_headerSize;
    quint32     m_offsetToPointData;
    quint32     m_numberOfVariableLengthRecords;
    quint8      m_pointDataRecordFormat;
    quint16     m_pointDataRecordLength;
    quint32     m_legacyNumberOfPointRecord;
    quint32     m_legacyNumberOfPointsByReturn[5];
    double      m_xScaleFactor;
    double      m_yScaleFactor;
    double      m_zScaleFactor;
    double      m_xOffset;
    double      m_yOffset;
    double      m_zOffset;
    quint64     m_startOfWaveformDataPacketRecord;
    quint64     m_startOfFirstExtendedVariableLengthRecord;
    quint32     m_numberOfExtendedVariableLengthRecords;
    quint64     m_numberOfPointRecords;
    quint64     m_numberOfPointsByReturn[15];
    Eigen::Vector3d     m_minCoordinates;
    Eigen::Vector3d     m_maxCoordinates;

    /**
     * @brief Returns the size in bytes
     */
    quint16 sizeInBytes() const;

    /**
     * @brief Returns the number of points to read
     */
    size_t getPointsRecordCount() const;

    /**
     * @brief Returns true if points must be scaled or translated
     */
    bool mustTransformPoints() const;

    /**
     * @brief Transform the point passed in parameter (when you read a point)
     */
    void transformPoint(const qint32 &x, const qint32 &y, const qint32 &z, double &xc, double &yc, double &zc) const;

    /**
     * @brief Transform the point passed in parameter (when you want to write a point)
     */
    void inverseTransformPoint(const double &x, const double &y, const double &z, qint32 &xc, qint32 &yc, qint32 &zc) const;

    /**
     * @brief Read the header
     */
    bool read(QDataStream &stream, QString &error);

    /**
     * @brief Returns true if the header in parameter share the same point format and if points written with it
     *        has same coodinates that points written with this header.
     */
    bool isHeaderAlmostSimilar(const CT_LASHeader &header, int compareOptions = CO_All) const;

    /**
     * @brief Call it before write
     */
    bool finalizeBeforeWrite();

    /**
     * @brief Write the header
     */
    bool write(QDataStream &stream, QString &error) const;

    /**
     * @brief Merge this header with the header in parameter. Return false if it was not compatible.
     */
    bool mergeWith(const CT_LASHeader& header);

    /**
     * @brief Change the scale factor by compute bounding box
     */
    void updateScaleFactorFromBoundingBox();

    /**
     * @brief Returns all information of this header in a string
     */
    QString toString() const;

    bool hasBoundingBox() const final {return true;}
    void boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const final;
    void setBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max);

    inline QString get_fileSignature() const {return QString(m_fileSignature);} //
    inline quint16 get_fileSourceID() const {return m_fileSourceID;}
    inline quint16 get_globalEncoding() const {return m_globalEncoding;}
    inline quint32 get_projectIDGuidData1() const {return m_projectIDGuidData1;}
    inline quint16 get_projectIDGuidData2() const {return m_projectIDGuidData2;}
    inline quint16 get_projectIDGuidData3() const {return m_projectIDGuidData3;}
    inline quint64 get_projectIDGuidData4() const {return m_projectIDGuidData4;}
    inline quint16 get_versionMajor() const {return m_versionMajor;}
    inline quint16 get_versionMinor() const {return m_versionMinor;}
    inline QString get_systemID() const {return QString(m_systemID);} //
    inline QString get_sofwareID() const {return QString(m_sofwareID);} //
    inline quint16 get_fileCreationDayOfYear() const {return m_fileCreationDayOfYear;}
    inline quint16 get_fileCreationYear() const {return m_fileCreationYear;}
    inline quint16 get_headerSize() const {return m_headerSize;}
    inline quint32 get_offsetToPointData() const {return m_offsetToPointData;}
    inline quint32 get_numberOfVariableLengthRecords() const {return m_numberOfVariableLengthRecords;}
    inline quint16 get_pointDataRecordFormat() const {return m_pointDataRecordFormat;}
    inline quint16 get_pointDataRecordLength() const {return m_pointDataRecordLength;}
    inline quint32 get_legacyNumberOfPointRecord() const {return m_legacyNumberOfPointRecord;}
    inline QString get_legacyNumberOfPointsByReturn() const {return m_legacyNumberOfPointsByReturn_ToString();} //
    inline double get_xScaleFactor() const {return m_xScaleFactor;}
    inline double get_yScaleFactor() const {return m_yScaleFactor;}
    inline double get_zScaleFactor() const {return m_zScaleFactor;}
    inline double get_xOffset() const {return m_xOffset;}
    inline double get_yOffset() const {return m_yOffset;}
    inline double get_zOffset() const {return m_zOffset;}
    inline double get_maxX() const {return m_maxCoordinates(0);}
    inline double get_minX() const {return m_minCoordinates(0);}
    inline double get_maxY() const {return m_maxCoordinates(1);}
    inline double get_minY() const {return m_minCoordinates(1);}
    inline double get_maxZ() const {return m_maxCoordinates(2);}
    inline double get_minZ() const {return m_minCoordinates(2);}
    inline quint64 get_startOfWaveformDataPacketRecord() const {return m_startOfWaveformDataPacketRecord;}
    inline quint64 get_startOfFirstExtendedVariableLengthRecord() const {return m_startOfFirstExtendedVariableLengthRecord;}
    inline quint32 get_numberOfExtendedVariableLengthRecords() const {return m_numberOfExtendedVariableLengthRecords;}
    inline quint64 get_numberOfPointRecords() const {return m_numberOfPointRecords;}
    inline QString get_numberOfPointsByReturn() const {return m_numberOfPointsByReturn_ToString();} //

private:
    QString m_legacyNumberOfPointsByReturn_ToString() const;
    QString m_numberOfPointsByReturn_ToString() const;

    CT_DEFAULT_IA_BEGIN(CT_LASHeader)
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_fileSignature, QObject::tr("File Signature"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_fileSourceID, QObject::tr("File Source ID"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_globalEncoding, QObject::tr("Global encoding"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_projectIDGuidData1, QObject::tr("Project ID - GUID data 1"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_projectIDGuidData2, QObject::tr("Project ID - GUID data 2"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_projectIDGuidData3, QObject::tr("Project ID - GUID data 3"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_projectIDGuidData4, QObject::tr("Project ID - GUID data 4"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_versionMajor, QObject::tr("Version Major"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_versionMinor, QObject::tr("Version Minor"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_systemID, QObject::tr("System Identifier"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_sofwareID, QObject::tr("Generating Software"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_fileCreationDayOfYear, QObject::tr("File Creation DoY"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_fileCreationYear, QObject::tr("File Creation Year"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_headerSize, QObject::tr("Header Size"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_offsetToPointData, QObject::tr("Offset to point data"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_numberOfVariableLengthRecords, QObject::tr("Number of Variable Length Records"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_pointDataRecordFormat, QObject::tr("Point Data Record Format"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_pointDataRecordLength, QObject::tr("Point Data Record Length"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_legacyNumberOfPointRecord, QObject::tr("Legacy Number of point records"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_legacyNumberOfPointsByReturn, QObject::tr("Legacy Number of points by return"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_xScaleFactor, QObject::tr("X scale factor"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_yScaleFactor, QObject::tr("Y scale factor"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_zScaleFactor, QObject::tr("Z scale factor"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataX(), &CT_LASHeader::get_xOffset, QObject::tr("X offset"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataY(), &CT_LASHeader::get_yOffset, QObject::tr("Y offset"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataZ(), &CT_LASHeader::get_zOffset, QObject::tr("Z offset"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataX(), &CT_LASHeader::get_maxX, QObject::tr("Max X"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataX(), &CT_LASHeader::get_minX, QObject::tr("Min X"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataY(), &CT_LASHeader::get_maxY, QObject::tr("Max Y"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataY(), &CT_LASHeader::get_minY, QObject::tr("Min Y"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataZ(), &CT_LASHeader::get_maxZ, QObject::tr("Max Z"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataZ(), &CT_LASHeader::get_minZ, QObject::tr("Min Z"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_startOfWaveformDataPacketRecord, QObject::tr("Start of Waveform Data Packet Record"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_startOfFirstExtendedVariableLengthRecord, QObject::tr("Start of first Extended Variable Length Record"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_numberOfExtendedVariableLengthRecords, QObject::tr("Number of Extended Variable Length Records"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_numberOfPointRecords, QObject::tr("Number of point records"))
    CT_DEFAULT_IA_V2(CT_LASHeader, CT_AbstractCategory::staticInitDataValue(), &CT_LASHeader::get_numberOfPointsByReturn, QObject::tr("Number of points by return"))
    CT_DEFAULT_IA_END(CT_LASHeader)

    double computeOffset(double minVal, double maxVal);
};

#endif // CT_LASHEADER_H
