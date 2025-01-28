#include "ct_lasheader.h"

#include <QDate>

#define readRawDataAndCheck(Param, N, Err) if(stream.readRawData(Param, N) != N) { error = Err; return false; } else { filePos += N; }
#define readData(Param, Err) if(stream.atEnd()) { error = Err; return false; } else { stream >> Param; filePos += sizeof(Param); }

#define checkHeaderSize if(filePos > m_headerSize) { /*error = QObject::tr("L'élément lu dépasse la taille de l'en-tête défini dans le fichier"); return false;*/ }
#define readRawDataAndCheckSize(Param, N, Err) if(stream.readRawData(Param, N) != N) { error = Err; return false; } else { filePos += N; checkHeaderSize; }
#define readDataAndCheckSize(Param, Err) if(stream.atEnd()) { error = Err; return false; } else { stream >> Param; filePos += sizeof(Param); checkHeaderSize; }

CT_DEFAULT_IA_INIT(CT_LASHeader)

CT_LASHeader::CT_LASHeader() : SuperClass()
{
    for(int i=0; i<4; ++i)
        m_fileSignature[i] = ' ';

    m_fileSourceID = 0;
    m_globalEncoding = 0;

    m_projectIDGuidData1 = 0;
    m_projectIDGuidData2 = 0;
    m_projectIDGuidData3 = 0;
    m_projectIDGuidData4 = 0;

    m_versionMajor = 0;
    m_versionMinor = 0;

    for(int i=0; i<32; ++i)
        m_systemID[i] = ' ';

    for(int i=0; i<32; ++i)
        m_sofwareID[i] = ' ';

    m_fileCreationDayOfYear = 0;
    m_fileCreationYear = 0;
    m_headerSize = 0;
    m_offsetToPointData = 0;
    m_numberOfVariableLengthRecords = 0;
    m_pointDataRecordFormat = 0;
    m_pointDataRecordLength = 0;
    m_legacyNumberOfPointRecord = 0;

    for(int i=0; i<5; ++i)
        m_legacyNumberOfPointsByReturn[i] = 0;

    m_xScaleFactor = 1;
    m_yScaleFactor = 1;
    m_zScaleFactor = 1;
    m_xOffset = 0;
    m_yOffset = 0;
    m_zOffset = 0;
    m_startOfWaveformDataPacketRecord = 0;
    m_startOfFirstExtendedVariableLengthRecord = 0;
    m_numberOfExtendedVariableLengthRecords = 0;
    m_numberOfPointRecords = 0;

    for(int i=0; i<15; ++i)
        m_numberOfPointsByReturn[i] = 0;
}

quint16 CT_LASHeader::sizeInBytes() const
{
    quint16 s = sizeof(m_fileSignature) +
            sizeof(m_fileSourceID) +
            sizeof(m_globalEncoding) +
            sizeof(m_projectIDGuidData1) +
            sizeof(m_projectIDGuidData2) +
            sizeof(m_projectIDGuidData3) +
            sizeof(m_projectIDGuidData4) +
            sizeof(m_versionMajor) +
            sizeof(m_versionMinor) +
            sizeof(m_systemID) +
            sizeof(m_sofwareID) +
            sizeof(m_fileCreationDayOfYear) +
            sizeof(m_fileCreationYear) +
            sizeof(m_headerSize) +
            sizeof(m_offsetToPointData) +
            sizeof(m_numberOfVariableLengthRecords) +
            sizeof(m_pointDataRecordFormat) +
            sizeof(m_pointDataRecordLength) +
            sizeof(m_legacyNumberOfPointRecord) +
            sizeof(m_legacyNumberOfPointsByReturn) +
            sizeof(m_xScaleFactor) +
            sizeof(m_yScaleFactor) +
            sizeof(m_zScaleFactor) +
            sizeof(m_xOffset) +
            sizeof(m_yOffset) +
            sizeof(m_zOffset) +
            sizeof(m_maxCoordinates(0)) +
            sizeof(m_minCoordinates(0)) +
            sizeof(m_maxCoordinates(1)) +
            sizeof(m_minCoordinates(1)) +
            sizeof(m_maxCoordinates(2)) +
            sizeof(m_minCoordinates(2));

    if((m_versionMajor == 1) && (m_versionMinor <= 2))
        return s;

    s += sizeof(m_startOfWaveformDataPacketRecord);

    if((m_versionMajor == 1) && (m_versionMinor <= 3))
        return s;

    s += sizeof(m_startOfFirstExtendedVariableLengthRecord) +
         sizeof(m_numberOfExtendedVariableLengthRecords) +
         sizeof(m_numberOfPointRecords) +
         sizeof(m_numberOfPointsByReturn);

    return s;
}

size_t CT_LASHeader::getPointsRecordCount() const
{
    if(m_legacyNumberOfPointRecord == 0)
        return m_numberOfPointRecords;

    return m_legacyNumberOfPointRecord;
}

bool CT_LASHeader::mustTransformPoints() const
{
    return (!qFuzzyIsNull(m_xOffset) || !qFuzzyIsNull(m_yOffset) || !qFuzzyIsNull(m_zOffset) || !qFuzzyCompare(m_xScaleFactor, 1) || !qFuzzyCompare(m_yScaleFactor, 1) || !qFuzzyCompare(m_zScaleFactor, 1));
}

void CT_LASHeader::transformPoint(const qint32 &x, const qint32 &y, const qint32 &z, double &xc, double &yc, double &zc) const
{
    xc = (double(x)*m_xScaleFactor) + m_xOffset;
    yc = (double(y)*m_yScaleFactor) + m_yOffset;
    zc = (double(z)*m_zScaleFactor) + m_zOffset;
}

void CT_LASHeader::inverseTransformPoint(const double &x, const double &y, const double &z, qint32 &xc, qint32 &yc, qint32 &zc) const
{
    xc = qint32(std::round((double(x - m_xOffset)) / double(m_xScaleFactor)));
    yc = qint32(std::round((double(y - m_yOffset)) / double(m_yScaleFactor)));
    zc = qint32(std::round((double(z - m_zOffset)) / double(m_zScaleFactor)));
}

bool CT_LASHeader::read(QDataStream &stream, QString &error)
{
    error = "";

    stream.device()->seek(0);
    stream.setByteOrder(QDataStream::LittleEndian);

    // used in #define (readRawDataAndCheck and readData)
    size_t filePos = 0;

    // File Signature
    readRawDataAndCheck(m_fileSignature, 4, QObject::tr("File Signature invalid"));

    if((m_fileSignature[0] != 'L')
            || (m_fileSignature[1] != 'A')
            || (m_fileSignature[2] != 'S')
            || (m_fileSignature[3] != 'F')) {
        error = QObject::tr("Not a LAS File");
        return false;
    }

    // File Source ID
    readData(m_fileSourceID, QObject::tr("File Source ID invalid"));

    // Global Encoding
    readData(m_globalEncoding, QObject::tr("Global Encoding invalid"));

    // Project ID
    readData(m_projectIDGuidData1, QObject::tr("Project ID 1 invalid"));
    readData(m_projectIDGuidData2, QObject::tr("Project ID 2 invalid"));
    readData(m_projectIDGuidData3, QObject::tr("Project ID 3 invalid"));
    readData(m_projectIDGuidData4, QObject::tr("Project ID 4 invalid"));

    // Version Major
    readData(m_versionMajor, QObject::tr("Version Major invalid"));

    // Version Minor
    readData(m_versionMinor, QObject::tr("Version Minor invalid"));

    // System Identifier
    readRawDataAndCheck(m_systemID, 32, QObject::tr("System Identifier invalid"));

    // Sofware Identifier
    readRawDataAndCheck(m_sofwareID, 32, QObject::tr("Sofware Identifier invalid"));

    // File Creation Day of Year
    readData(m_fileCreationDayOfYear, QObject::tr("File Creation Day of Year invalid"));

    // File Creation Year
    readData(m_fileCreationYear, QObject::tr("File Creation Year invalid"));

    // Header Size
    readData(m_headerSize, QObject::tr("Header Size invalid"));

    // Offset To Point Data
    readData(m_offsetToPointData, QObject::tr("Offset To Point Data invalid"));

    if(m_offsetToPointData < m_headerSize) {

        error = QObject::tr("The offset to the start of points data (%1) is smaller than the header size (%2).").arg(m_offsetToPointData).arg(m_headerSize);
        return false;
    }

    // Number of Variable Length Records
    readData(m_numberOfVariableLengthRecords, QObject::tr("Number of Variable Length Records invalid"));

    // Point Data Record Format
    readData(m_pointDataRecordFormat, QObject::tr("Point Data Record Format invalid"));

    // Point Data Record Length
    readData(m_pointDataRecordLength, QObject::tr("Point Data Record Length invalid"));

    // Legacy Number of point records
    readData(m_legacyNumberOfPointRecord, QObject::tr("Legacy Number of point records invalid"));

    // Legacy Number of points by return
    for(int i=0; i<5; ++i) {
        readData(m_legacyNumberOfPointsByReturn[i], QObject::tr("Legacy Number of points by return (%1) invalid").arg(i));
    }

    readData(m_xScaleFactor, QObject::tr("X Scale Factor invalid"));
    readData(m_yScaleFactor, QObject::tr("Y Scale Factor invalid"));
    readData(m_zScaleFactor, QObject::tr("Z Scale Factor invalid"));
    readData(m_xOffset, QObject::tr("X Offset invalid"));
    readData(m_yOffset, QObject::tr("Y Offset invalid"));
    readData(m_zOffset, QObject::tr("Z Offset invalid"));
    readData(m_maxCoordinates(0), QObject::tr("Max X invalid"));
    readData(m_minCoordinates(0), QObject::tr("Min X invalid"));
    readData(m_maxCoordinates(1), QObject::tr("Max Y invalid"));
    readData(m_minCoordinates(1), QObject::tr("Min Y invalid"));
    readData(m_maxCoordinates(2), QObject::tr("Max Z invalid"));
    readData(m_minCoordinates(2), QObject::tr("Min Z invalid"));

    if((m_versionMajor == 1) && (m_versionMinor <= 2))
        return true;

    readDataAndCheckSize(m_startOfWaveformDataPacketRecord, QObject::tr("Start of Waveform Data Packet Record invalid"));

    if((m_versionMajor == 1) && (m_versionMinor <= 3))
        return true;

    readDataAndCheckSize(m_startOfFirstExtendedVariableLengthRecord, QObject::tr("Start Of First Extended Variable Length Record invalid"));
    readDataAndCheckSize(m_numberOfExtendedVariableLengthRecords, QObject::tr("Number Of Extended Variable Length Records invalid"));
    readDataAndCheckSize(m_numberOfPointRecords, QObject::tr("Number Of Point Records invalid"));

    for(int i=0; i<15; ++i) {
        quint64 p;
        readDataAndCheckSize(p, QObject::tr("Number Of Points By Return (%1) invalid").arg(i));
        m_numberOfPointsByReturn[i] = p;
    }

    return true;
}

bool CT_LASHeader::isHeaderAlmostSimilar(const CT_LASHeader& header, int compareOptions) const
{
    if((compareOptions & CO_Format)
            && (this->m_pointDataRecordFormat != header.m_pointDataRecordFormat))
            return false;

    if((compareOptions & CO_Version)
            && ((this->m_versionMinor != header.m_versionMinor)
                || (this->m_versionMajor != header.m_versionMajor)))
        return false;

    if((compareOptions & CO_Offset)
            && (!qFuzzyCompare(this->m_xOffset, header.m_xOffset)
                || !qFuzzyCompare(this->m_yOffset, header.m_yOffset)
                || !qFuzzyCompare(this->m_zOffset, header.m_zOffset)))
        return false;

    if((compareOptions & CO_Scale)
            && (!qFuzzyCompare(this->m_xScaleFactor, header.m_xScaleFactor)
                || !qFuzzyCompare(this->m_yScaleFactor, header.m_yScaleFactor)
                || !qFuzzyCompare(this->m_zScaleFactor, header.m_zScaleFactor)))
        return false;

    return true;
}

bool CT_LASHeader::finalizeBeforeWrite()
{
    m_headerSize = quint16(sizeInBytes());
    m_offsetToPointData = m_headerSize;

    return m_headerSize > 0;
}

bool CT_LASHeader::write(QDataStream &stream, QString &error) const
{
    error = "";

    if(m_headerSize == 0)
        return false;

    stream.device()->seek(0);
    stream.setByteOrder(QDataStream::LittleEndian);

    // File Signature
    stream.writeRawData("LASF", 4);

    // File Source ID
    stream << m_fileSourceID;

    // Global Encoding
    stream << m_globalEncoding;

    // Project ID
    stream << m_projectIDGuidData1;
    stream << m_projectIDGuidData2;
    stream << m_projectIDGuidData3;
    stream << m_projectIDGuidData4;

    // Version Major
    stream << m_versionMajor;

    // Version Minor
    stream << m_versionMinor;

    // System Identifier
    stream.writeRawData(m_systemID, sizeof(m_systemID));

    // Sofware Identifier
    stream.writeRawData(m_sofwareID, sizeof(m_sofwareID));

    // File Creation Day of Year
    stream << m_fileCreationDayOfYear;

    // File Creation Year
    stream << m_fileCreationYear;

    // Header Size
    stream << m_headerSize;

    // Offset To Point Data
    stream << m_offsetToPointData;

    // Number of Variable Length Records
    stream << m_numberOfVariableLengthRecords;

    // Point Data Record Format
    stream << m_pointDataRecordFormat;

    // Point Data Record Length
    stream << m_pointDataRecordLength;

    // Legacy Number of point records
    stream << m_legacyNumberOfPointRecord;

    // Legacy Number of points by return
    for(int i=0; i<5; ++i) {
        stream << m_legacyNumberOfPointsByReturn[i];
    }

    stream << m_xScaleFactor;
    stream << m_yScaleFactor;
    stream << m_zScaleFactor;

    stream << m_xOffset;
    stream << m_yOffset;
    stream << m_zOffset;

    stream << ceil(get_maxX()/m_xScaleFactor)*m_xScaleFactor;
    stream << floor(get_minX()/m_xScaleFactor)*m_xScaleFactor;
    stream << ceil(get_maxY()/m_yScaleFactor)*m_yScaleFactor;
    stream << floor(get_minY()/m_yScaleFactor)*m_yScaleFactor;
    stream << ceil(get_maxZ()/m_zScaleFactor)*m_zScaleFactor;
    stream << floor(get_minZ()/m_zScaleFactor)*m_zScaleFactor;

    if((m_versionMajor == 1) && (m_versionMinor <= 2))
        return true;

    stream << m_startOfWaveformDataPacketRecord;

    if((m_versionMajor == 1) && (m_versionMinor <= 3))
        return true;

    stream << m_startOfFirstExtendedVariableLengthRecord;
    stream << m_numberOfExtendedVariableLengthRecords;
    stream << m_numberOfPointRecords;

    for(int i=0; i<15; ++i) {
        stream << m_numberOfPointsByReturn[i];
    }

    return true;
}

bool CT_LASHeader::mergeWith(const CT_LASHeader &header)
{
    if(header.m_versionMajor > m_versionMajor) {
        m_versionMajor = header.m_versionMajor;
        m_versionMinor = header.m_versionMinor;
    } else if((header.m_versionMajor == m_versionMajor)
              && (header.m_versionMinor > m_versionMinor))
        m_versionMinor = header.m_versionMinor;

    if(header.m_pointDataRecordFormat > m_pointDataRecordFormat) {
        m_pointDataRecordFormat = header.m_pointDataRecordFormat;
        m_pointDataRecordLength = header.m_pointDataRecordLength;
    }

    Eigen::Vector3d oMin = header.m_minCoordinates;
    Eigen::Vector3d oMax = header.m_maxCoordinates;

    Eigen::Vector3d moMin = m_minCoordinates;
    Eigen::Vector3d moMax = m_maxCoordinates;

    for(quint8 i=0; i<3; ++i)
        oMin[i] = qMin(moMin[i], oMin[i]);

    for(quint8 i=0; i<3; ++i)
        oMax[i] = qMax(moMax[i], oMax[i]);

    m_minCoordinates = oMin;
    m_maxCoordinates = oMax;

    for(int i=0; i<15; ++i)
        m_numberOfPointsByReturn[i] += header.m_numberOfPointsByReturn[i];

    for(int i=0; i<5; ++i)
        m_legacyNumberOfPointsByReturn[i] += header.m_legacyNumberOfPointsByReturn[i];

    m_legacyNumberOfPointRecord += header.m_legacyNumberOfPointRecord;
    m_numberOfPointRecords += header.m_numberOfPointRecords;

    // scale factor merged here
    updateScaleFactorFromBoundingBox();

    // what to do with offset ?
    // now unchanged

    return true;
}

void CT_LASHeader::updateScaleFactorFromBoundingBox()
{
    // maximum point coordinate
//        double scaleFactor = qMax(qMax(qAbs(max(0)), qAbs(max(1))), qAbs(max(2)));
//        scaleFactor = qMax(qMax(qMax(qAbs(min(0)), qAbs(min(1))), qAbs(min(2))), scaleFactor);

//        // scale factor is the optimized to convert a point to be the most accurate
//        scaleFactor = qMin((scaleFactor/(std::numeric_limits<qint32>::max()-1)), 1.0);

    m_xOffset = computeOffset(m_minCoordinates(0), m_maxCoordinates(0));
    m_yOffset = computeOffset(m_minCoordinates(1), m_maxCoordinates(1));
    m_zOffset = computeOffset(m_minCoordinates(2), m_maxCoordinates(2));

    if ((m_maxCoordinates(0) - m_minCoordinates(0)) < 1.0 && (m_maxCoordinates(1) - m_minCoordinates(1)) < 1.0)
    {
        m_xScaleFactor = 0.00000001;
    } else {
        m_xScaleFactor = 0.001;
    }
    m_zScaleFactor = 0.001;


    double maxCoordXY = qMax( qMax(qAbs(m_maxCoordinates(0) - m_xOffset), qAbs(m_maxCoordinates(1) - m_yOffset)), qMax(qAbs(m_minCoordinates(0) - m_xOffset),qAbs(m_minCoordinates(1) - m_yOffset)) );
//    m_xScaleFactor = 0.00000001;
//    if (maxCoordXY > 9.9) {m_xScaleFactor = 0.0000001;}
//    if (maxCoordXY > 99.9) {m_xScaleFactor = 0.000001;}
//    if (maxCoordXY > 999.9) {m_xScaleFactor = 0.00001;}
//    if (maxCoordXY > 9999.9) {m_xScaleFactor = 0.0001;}
//    if (maxCoordXY > 99999.9) {m_xScaleFactor = 0.001;}
    if (maxCoordXY > 999999.9) {m_xScaleFactor = 0.01;}
    if (maxCoordXY > 9999999.9) {m_xScaleFactor = 0.1;}
    if (maxCoordXY > 99999999.9) {m_xScaleFactor = 1.0;}

    m_yScaleFactor = m_xScaleFactor;


    double maxCoordZ = qMax(qAbs(m_maxCoordinates(2) - m_zOffset), qAbs(m_minCoordinates(2) - m_zOffset));
//    m_zScaleFactor = 0.00000001;
//    if (maxCoordZ > 9.9) {m_zScaleFactor = 0.0000001;}
//    if (maxCoordZ > 99.9) {m_zScaleFactor = 0.000001;}
//    if (maxCoordZ > 999.9) {m_zScaleFactor = 0.00001;}
//    if (maxCoordZ > 9999.9) {m_zScaleFactor = 0.0001;}
//    if (maxCoordZ > 99999.9) {m_zScaleFactor = 0.001;}
    if (maxCoordZ > 999999.9) {m_zScaleFactor = 0.01;}
    if (maxCoordZ > 9999999.9) {m_zScaleFactor = 0.1;}
    if (maxCoordZ > 99999999.9) {m_zScaleFactor = 1.0;}

    if (m_zScaleFactor < m_xScaleFactor) {m_zScaleFactor = m_xScaleFactor;}
}

double CT_LASHeader::computeOffset(double minVal, double maxVal)
{
    double meanVal = (maxVal + minVal) / 2.0;
    if (meanVal < 1000.0) {return std::round(meanVal);}

    return 1000.0*std::round(meanVal / 1000.0);
}

QString CT_LASHeader::toString() const
{
    QString str;

    QDate date(1,1,1);
    date = date.addYears(m_fileCreationYear-1);
    date = date.addDays(m_fileCreationDayOfYear-1);

    str += QObject::tr("File Signature : %1").arg(m_fileSignature) + "\r\n";
    str += QObject::tr("File Source ID : %1").arg(m_fileSourceID) + "\r\n";
    str += QObject::tr("Project ID - GUID data 1 : %1").arg(m_projectIDGuidData1) + "\r\n";
    str += QObject::tr("Project ID - GUID data 2 : %1").arg(m_projectIDGuidData2) + "\r\n";
    str += QObject::tr("Project ID - GUID data 3 : %1").arg(m_projectIDGuidData3) + "\r\n";
    str += QObject::tr("Project ID - GUID data 4 : %1").arg(m_projectIDGuidData4) + "\r\n";
    str += QObject::tr("Version Major : %1").arg(m_versionMajor) + "\r\n";
    str += QObject::tr("Version Minor : %1").arg(m_versionMinor) + "\r\n";
    str += QObject::tr("System Identifier : %1").arg(m_systemID) + "\r\n";
    str += QObject::tr("Generating Software : %1").arg(m_sofwareID) + "\r\n";
    str += QObject::tr("File Creation : %1").arg(date.toString(QObject::tr("dd/MM/yyyy"))) + "\r\n";
    str += QObject::tr("Header Size : %1").arg(m_headerSize) + "\r\n";
    str += QObject::tr("Offset to point data : %1").arg(m_offsetToPointData) + "\r\n";
    str += QObject::tr("Number of Variable Length Records : %1").arg(m_numberOfVariableLengthRecords) + "\r\n";
    str += QObject::tr("Point Data Record Format : %1").arg(m_pointDataRecordFormat) + "\r\n";
    str += QObject::tr("Point Data Record Length : %1").arg(m_pointDataRecordLength) + "\r\n";
    str += QObject::tr("Legacy Number of point records : %1").arg(m_legacyNumberOfPointRecord) + "\r\n";
    str += QObject::tr("Legacy Number of points by return : %1 / ...").arg(m_legacyNumberOfPointsByReturn[0]) + "\r\n";
    str += QObject::tr("X/Y/Z scale factor : %1 / %2 / %3").arg(m_xScaleFactor).arg(m_yScaleFactor).arg(m_zScaleFactor) + "\r\n";
    str += QObject::tr("X/Y/Z offset : %1 / %2 / %3").arg(m_xOffset).arg(m_yOffset).arg(m_zOffset) + "\r\n";
    str += QObject::tr("Min X/Y/Z : %1 / %2 / %3").arg(get_minX()).arg(get_minY()).arg(get_minZ()) + "\r\n";
    str += QObject::tr("Max X/Y/Z : %1 / %2 / %3").arg(get_maxX()).arg(get_maxY()).arg(get_maxZ()) + "\r\n";
    str += QObject::tr("Start of Waveform Data Packet Record : %1").arg(m_startOfWaveformDataPacketRecord) + "\r\n";
    str += QObject::tr("Start of first Extended Variable Length Record : %1").arg(m_startOfFirstExtendedVariableLengthRecord) + "\r\n";
    str += QObject::tr("Number of Extended Variable Length Records : %1").arg(m_numberOfExtendedVariableLengthRecords) + "\r\n";
    str += QObject::tr("Number of point records : %1").arg(m_numberOfPointRecords) + "\r\n";
    str += QObject::tr("Number of points by return : %1 / ...").arg(m_numberOfPointsByReturn[0]) + "\r\n";

    return str;
}

void CT_LASHeader::boundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const
{
    min = m_minCoordinates;
    max = m_maxCoordinates;
}

void CT_LASHeader::setBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max)
{
    m_minCoordinates = min;
    m_maxCoordinates = max;
}

QString CT_LASHeader::m_legacyNumberOfPointsByReturn_ToString() const
{
    QString str = "";
    for (int i = 0 ; i < 5 ; i++)
    {
        str.append(QString::number(m_legacyNumberOfPointsByReturn[i]));
        if (i < 4) {str.append(" ");}
    }
    return str;
}

QString CT_LASHeader::m_numberOfPointsByReturn_ToString() const
{
    QString str = "";
    for (int i = 0 ; i < 15 ; i++)
    {
        str.append(QString::number(m_numberOfPointsByReturn[i]));
        if (i < 14) {str.append(" ");}
    }
    return str;
}
