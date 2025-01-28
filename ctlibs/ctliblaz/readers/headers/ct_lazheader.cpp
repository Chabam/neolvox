#include "ct_lazheader.h"

#include "readers/ct_laszip_reader.h"

#include "ct_log/ct_logmanager.h"
#include <QDate>

#define readRawDataAndCheck(Param, N, Err) if(stream.readRawData(Param, N) != N) { error = Err; return false; } else { filePos += N; }
#define readData(Param, Err) if(stream.atEnd()) { error = Err; return false; } else { stream >> Param; filePos += sizeof(Param); }

#define checkHeaderSize if(filePos > m_headerSize) { /*error = QObject::tr("L'élément lu dépasse la taille de l'en-tête défini dans le fichier"); return false;*/ }
#define readRawDataAndCheckSize(Param, N, Err) if(stream.readRawData(Param, N) != N) { error = Err; return false; } else { filePos += N; checkHeaderSize; }
#define readDataAndCheckSize(Param, Err) if(stream.atEnd()) { error = Err; return false; } else { stream >> Param; filePos += sizeof(Param); checkHeaderSize; }

CT_LAZHeader::CT_LAZHeader() : SuperClass()
{
    for (int i = 0; i < 4; ++i)
    {
        m_fileSignature[i] = ' ';
    }

    m_fileSourceID = 0;
    m_globalEncoding = 0;

    m_projectIDGuidData1 = 0;
    m_projectIDGuidData2 = 0;
    m_projectIDGuidData3 = 0;
    m_projectIDGuidData4 = 0;

    m_versionMajor = 0;
    m_versionMinor = 0;

    for(int i = 0; i < 32; ++i)
    {
        m_systemID[i] = ' ';
    }

    for(int i = 0; i < 32; ++i)
    {
        m_sofwareID[i] = ' ';
    }

    m_fileCreationDayOfYear = 0;
    m_fileCreationYear = 0;
    m_headerSize = 0;
    m_offsetToPointData = 0;
    m_numberOfVariableLengthRecords = 0;
    m_pointDataRecordFormat = 0;
    m_pointDataRecordLength = 0;
    m_legacyNumberOfPointRecord = 0;

    for(int i = 0; i < 5; ++i)
    {
        m_legacyNumberOfPointsByReturn[i] = 0;
    }

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

    for(int i = 0; i < 15; ++i)
    {
        m_numberOfPointsByReturn[i] = 0;
    }
}

bool CT_LAZHeader::readzip(const QString &filepath, QString &error)
{
    error = "";
    bool valid = true;

    CT_LASZIP_Reader lasZipReader(filepath);

    if (lasZipReader.openFile())
    {
        laszip_header* header;

        // get a pointer to the header of the reader that was just populated
        if (lasZipReader.initHeaderPointer(&header))
        {
            // LASZIP to CT conversion of header data
            m_fileSourceID = header->file_source_ID;
            m_globalEncoding = header->global_encoding;

            m_projectIDGuidData1 = header->project_ID_GUID_data_1;
            m_projectIDGuidData2 = header->project_ID_GUID_data_2;
            m_projectIDGuidData3 = header->project_ID_GUID_data_3;
            std::memcpy(&m_projectIDGuidData4, header->project_ID_GUID_data_4, 8);

            m_versionMajor = header->version_major;
            m_versionMinor = header->version_minor;

            std::memcpy(&m_systemID, header->system_identifier, 32);
            std::memcpy(&m_sofwareID, header->generating_software, 32);

            m_fileCreationDayOfYear = header->file_creation_day;
            m_fileCreationYear = header->file_creation_year;

            m_headerSize = header->header_size;
            m_offsetToPointData = header->offset_to_point_data;

            if(m_offsetToPointData < m_headerSize)
            {
                error = QObject::tr("The offset to the start of points data (%1) is smaller than the header size (%2).").arg(m_offsetToPointData).arg(m_headerSize);
                lasZipReader.closeFile();
                return false;
            }

            m_numberOfVariableLengthRecords = header->number_of_variable_length_records;
            m_pointDataRecordFormat = header->point_data_format;
            m_pointDataRecordLength = header->point_data_record_length;
            m_legacyNumberOfPointRecord = header->number_of_point_records;

            for(int i=0; i<5; ++i)
            {
                m_legacyNumberOfPointsByReturn[i] = header->number_of_points_by_return[i];
            }

            m_xScaleFactor = header->x_scale_factor;
            m_yScaleFactor = header->y_scale_factor;
            m_zScaleFactor = header->z_scale_factor;
            m_xOffset = header->x_offset;
            m_yOffset = header->y_offset;
            m_zOffset = header->z_offset;
            m_maxCoordinates(0) = header->max_x;
            m_minCoordinates(0) = header->min_x;
            m_maxCoordinates(1) = header->max_y;
            m_minCoordinates(1) = header->min_y;
            m_maxCoordinates(2) = header->max_z;
            m_minCoordinates(2) = header->min_z;

            // End of header for LAS <=1.2
            if((m_versionMajor == 1) && (m_versionMinor <= 2))
            {
                lasZipReader.closeFile();
                return true;
            }

            m_startOfWaveformDataPacketRecord = header->start_of_waveform_data_packet_record;

            // End of header for LAS 1.3
            if((m_versionMajor == 1) && (m_versionMinor <= 3))
            {
                lasZipReader.closeFile();
                return true;
            }

            m_startOfFirstExtendedVariableLengthRecord = header->start_of_first_extended_variable_length_record;
            m_numberOfExtendedVariableLengthRecords = header->number_of_extended_variable_length_records;
            m_numberOfPointRecords = header->extended_number_of_point_records;

            for(int i=0; i<15; ++i)
            {
                m_numberOfPointsByReturn[i] = header->extended_number_of_points_by_return[i];
            }

        } else {
            valid = false;
        }
    } else {
        valid = false;
    }

    lasZipReader.closeFile();

    return valid;
}

bool CT_LAZHeader::writezip(laszip_header &header) const
{
    // CT to LASZIP conversion of header data
    header.file_source_ID = m_fileSourceID;
    header.global_encoding = m_globalEncoding;

    header.project_ID_GUID_data_1 = m_projectIDGuidData1;
    header.project_ID_GUID_data_2 = m_projectIDGuidData2;
    header.project_ID_GUID_data_3 = m_projectIDGuidData3;
    std::memcpy(header.project_ID_GUID_data_4, &m_projectIDGuidData4, 8);

    header.version_major = m_versionMajor;
    header.version_minor = m_versionMinor;

    std::memcpy(header.system_identifier, &m_systemID, 32);
    std::memcpy(header.generating_software, &m_sofwareID, 32);

    header.file_creation_day = m_fileCreationDayOfYear;
    header.file_creation_year = m_fileCreationYear;

    header.header_size = m_headerSize;
    header.offset_to_point_data = m_offsetToPointData;

    header.number_of_variable_length_records = m_numberOfVariableLengthRecords;
    header.point_data_format = m_pointDataRecordFormat;
    header.point_data_record_length = m_pointDataRecordLength;
    header.number_of_point_records = m_legacyNumberOfPointRecord;

    for(int i=0; i<5; ++i)
    {
        header.number_of_points_by_return[i] = m_legacyNumberOfPointsByReturn[i];
    }

    header.x_scale_factor = m_xScaleFactor;
    header.y_scale_factor = m_yScaleFactor;
    header.z_scale_factor = m_zScaleFactor;
    header.x_offset = m_xOffset;
    header.y_offset = m_yOffset;
    header.z_offset = m_zOffset;
    header.max_x = m_maxCoordinates(0);
    header.min_x = m_minCoordinates(0);
    header.max_y = m_maxCoordinates(1);
    header.min_y = m_minCoordinates(1);
    header.max_z = m_maxCoordinates(2);
    header.min_z = m_minCoordinates(2);

    // End of header for LAS 1.3
    if((m_versionMajor == 1) && (m_versionMinor > 2))
    {
        header.start_of_waveform_data_packet_record = m_startOfWaveformDataPacketRecord;
    }

    // End of header for LAS 1.4
    if((m_versionMajor == 1) && (m_versionMinor > 3))
    {
        header.start_of_first_extended_variable_length_record = m_startOfFirstExtendedVariableLengthRecord;
        header.number_of_extended_variable_length_records = m_numberOfExtendedVariableLengthRecords;
        header.extended_number_of_point_records = m_numberOfPointRecords;

        for(int i=0; i<15; ++i)
        {
            header.extended_number_of_points_by_return[i] = m_numberOfPointsByReturn[i];
        }

        // Hack for 'laszip_set_header'
        header.user_data_in_header = 0;
        header.user_data_in_header_size = 0;
        header.vlrs = 0;
        header.user_data_after_header_size = 0;
        header.user_data_after_header = 0;
    }

    return true;
}
