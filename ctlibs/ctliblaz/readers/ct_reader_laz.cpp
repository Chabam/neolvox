#include "ct_reader_laz.h"

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_iterator/ct_mutablepointiterator.h"

#include "readers/ct_laszip_reader.h"

#include "ct_log/ct_logmanager.h"

#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QDate>
#include <QDebug>

#include "laszip/laszip_api.h"

CT_Reader_LAZ::CT_Reader_LAZ(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{    
    m_headerFromConfiguration = nullptr;

    addNewReadableFormat(FileFormat("laz", tr("Fichiers LAZ .laz")));

    setToolTip(tr("Charge des points depuis un fichier au format LAZ (rapidlasso GmbH)<br>https://laszip.org/"));
}

CT_Reader_LAZ::CT_Reader_LAZ(const CT_Reader_LAZ &other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{
    m_headerFromConfiguration = nullptr;

    if(other.m_headerFromConfiguration != nullptr)
        m_headerFromConfiguration = static_cast<CT_LAZHeader*>(other.m_headerFromConfiguration->copy(nullptr, nullptr));
}

CT_Reader_LAZ::~CT_Reader_LAZ()
{
    delete m_headerFromConfiguration;
}

QString CT_Reader_LAZ::displayableName() const
{
    return tr("Points, format LAZ");
}

bool CT_Reader_LAZ::setFilePath(const QString& filepath)
{

    QString err;
    CT_LAZHeader *header = static_cast<CT_LAZHeader*>(internalReadHeader(filepath, err));

    if(header != nullptr) {
        if(CT_AbstractReader::setFilePath(filepath)) {
            setToolTip(header->toString());
            delete header;
            return true;
        }
    } else if(!err.isEmpty()) {
        PS_LOG->addErrorMessage(LogInterface::reader, err);
    }

    delete header;
    return false;
}

bool CT_Reader_LAZ::configure()
{
    QString err;
    CT_LAZHeader *header = static_cast<CT_LAZHeader*>(internalReadHeader(filepath(), err));

    if(header != nullptr) {
        delete m_headerFromConfiguration;
        m_headerFromConfiguration = header;
        return true;
    }

    delete header;
    return false;
}

CT_FileHeader* CT_Reader_LAZ::createHeaderPrototype() const
{
    return new CT_LAZHeader();
}

bool CT_Reader_LAZ::restoreSettings(SettingsReaderInterface &reader)
{
    delete m_headerFromConfiguration;
    m_headerFromConfiguration = nullptr;

    if(SuperClass::restoreSettings(reader)) {
        QString error;
        m_headerFromConfiguration = static_cast<CT_LAZHeader*>(internalReadHeader(filepath(), error));
    }

    return (m_headerFromConfiguration != nullptr);
}

void CT_Reader_LAZ::internalDeclareOutputModels(CT_ReaderOutModelStructureManager &manager)
{
    manager.addItem(m_hOutScene, tr("Scène"));
    manager.addItem(m_hOutAllAtt, tr("Attributs LAS"));

    manager.addPointAttribute(m_hOutReturnNumber, tr("Return Number"));
    manager.addPointAttribute(m_hOutNumberOfReturn, tr("Number of Returns"));
    manager.addPointAttribute(m_hOutClassificationFlag, tr("Classification Flags"));
    manager.addPointAttribute(m_hOutScannerChannel, tr("Scanner Channel"));
    manager.addPointAttribute(m_hOutScanDirectionFlag, tr("Scan Direction Flag"));
    manager.addPointAttribute(m_hOutEdgeOfFlightLine, tr("Edge of Flight Line"));

    manager.addPointAttribute(m_hOutIntensity, tr("Intensity"));
    manager.addPointAttribute(m_hOutClassification, tr("Classification"));
    manager.addPointAttribute(m_hOutUserData, tr("User Data"));
    manager.addPointAttribute(m_hOutPointSourceID, tr("Point Source "));

    manager.addPointAttribute(m_hOutScanAngle, tr("Scan Angle"));

    manager.addPointAttribute(m_hOutGPSTime, tr("GPS Time"));

    manager.addPointAttribute(m_hOutColor, tr("Color"));
    manager.addPointAttribute(m_hOutRed, tr("Red"));
    manager.addPointAttribute(m_hOutGreen, tr("Green"));
    manager.addPointAttribute(m_hOutBlue, tr("Blue"));

    manager.addPointAttribute(m_hOutWavePacketDescriptorIndex, tr("Wave Packet Descriptor Index"));
    manager.addPointAttribute(m_hOutByteOffsetToWaveformData, tr("Byte Offset To Waveform Data"));
    manager.addPointAttribute(m_hOutWaveformPacketSizeInBytes, tr("Waveform Packet Size In Bytes"));
    manager.addPointAttribute(m_hOutReturnPointWaveformLocation, tr("Return Point Waveform Location"));

    manager.addPointAttribute(m_hOutNIR, tr("NIR"));
}

CT_FileHeader *CT_Reader_LAZ::internalReadHeader(const QString &filepath, QString &error) const
{
    CT_LAZHeader *header = nullptr;

    // Test File validity
    if(QFile::exists(filepath))
    {
        header = new CT_LAZHeader();
        header->setFilePath(filepath);

        if(!header->readzip(filepath, error)) {
            delete header;
            header = nullptr;
        }
    }

    return header;
}

bool CT_Reader_LAZ::internalReadFile(CT_StandardItemGroup *group)
{
    QString error;
    bool valid = true;

    CT_LAZHeader *header = static_cast<CT_LAZHeader*>(internalReadHeader(filepath(), error));
    if(header == nullptr)
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Impossible de lire l'en-tête du fichier (null) %1").arg(filepath()));
        return false;
    }

    size_t nPoints = header->getPointsRecordCount();
    if(nPoints == 0)
    {
        PS_LOG->addWarningMessage(LogInterface::reader, tr("Aucun points contenu dans le fichier (n = 0) %1").arg(filepath()));
        delete header;
        return true;
    }

    auto filepath = CT_AbstractReader::filepath();
    CT_LASZIP_Reader lasZipReader(filepath);

    if (lasZipReader.openFile())
    {
        // get a pointer to the points that will be read
        if (lasZipReader.initPointPointer())
        {
            setToolTip(header->toString());
            bool mustTransformPoint = header->mustTransformPoints();

            // If filtering function set, do a first scan of the file to count filtered point number
            size_t numberOfFilteredPoints = 0;
            if (filterSet())
            {
                double xc = 0;
                double yc = 0;
                double zc = 0;
                CT_Point pAdded;

                for (size_t i = 0; i < nPoints; ++i)
                {
                    // READ THE POINT
                    if (lasZipReader.readPoint())
                    {
                        laszip_point* point = lasZipReader.point();

                        qint32 x = point->X;
                        qint32 y = point->Y;
                        qint32 z = point->Z;

                        // CONVERT POINT
                        if(mustTransformPoint)
                        {
                            header->transformPoint(x, y, z, xc, yc, zc);
                            pAdded(0) = xc;
                            pAdded(1) = yc;
                            pAdded(2) = zc;
                        } else
                        {
                            pAdded(0) = x;
                            pAdded(1) = y;
                            pAdded(2) = z;
                        }

                        if (isPointFiltered(pAdded)) {++numberOfFilteredPoints;}
                    }
                }

                // restore first point position for real data reading
                lasZipReader.seek(0);
            }

            size_t cloudSize = nPoints - numberOfFilteredPoints;
            CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(cloudSize);

            // VECTOR
            auto scanAngleSetter = m_hOutScanAngle.createAttributesSetter(pcir);
            auto core6_10Setter = mCore6_10Manager.createAttributesSetter(pcir);

            // Vector in all point data format :
            auto intensitySetter = m_hOutIntensity.createAttributesSetter(pcir);
            auto classificationSetter = m_hOutClassification.createAttributesSetter(pcir);
            auto userDataSetter = m_hOutUserData.createAttributesSetter(pcir);
            auto pointSourceIDSetter = m_hOutPointSourceID.createAttributesSetter(pcir);

            // Vector not in all point data format :
            decltype (m_hOutGPSTime)::SetterPtr gpsTimeSetter = nullptr;
            decltype (m_hOutColor)::SetterPtr colorSetter = nullptr;
            decltype (m_hOutRed)::SetterPtr redSetter = nullptr;
            decltype (m_hOutGreen)::SetterPtr greenSetter = nullptr;
            decltype (m_hOutBlue)::SetterPtr blueSetter = nullptr;
            decltype (m_hOutWaveformPacketSizeInBytes)::SetterPtr wpsibSetter = nullptr;
            decltype (m_hOutByteOffsetToWaveformData)::SetterPtr botwdSetter = nullptr;
            decltype (m_hOutWavePacketDescriptorIndex)::SetterPtr wpdiSetter = nullptr;
            decltype (m_hOutReturnPointWaveformLocation)::SetterPtr rpwlSetter = nullptr;
            decltype (m_hOutNIR)::SetterPtr nirSetter = nullptr;

            if((m_headerFromConfiguration->m_pointDataRecordFormat == 1)
               || (m_headerFromConfiguration->m_pointDataRecordFormat > 2))
            {
                gpsTimeSetter = m_hOutGPSTime.createAttributesSetterPtr(pcir);
            }

            if((m_headerFromConfiguration->m_pointDataRecordFormat == 2)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 3)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 5)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 7)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 8)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 10))
            {
                colorSetter = m_hOutColor.createAttributesSetterPtr(pcir);
                redSetter = m_hOutRed.createAttributesSetterPtr(pcir);
                greenSetter = m_hOutGreen.createAttributesSetterPtr(pcir);
                blueSetter = m_hOutBlue.createAttributesSetterPtr(pcir);
            }

            if((m_headerFromConfiguration->m_pointDataRecordFormat == 8)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 10))
            {
                nirSetter = m_hOutNIR.createAttributesSetterPtr(pcir);
            }

            if((m_headerFromConfiguration->m_pointDataRecordFormat == 4)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 5)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 9)
               || (m_headerFromConfiguration->m_pointDataRecordFormat == 10))
            {
                wpdiSetter = m_hOutWavePacketDescriptorIndex.createAttributesSetterPtr(pcir);
                botwdSetter = m_hOutByteOffsetToWaveformData.createAttributesSetterPtr(pcir);
                wpsibSetter = m_hOutWaveformPacketSizeInBytes.createAttributesSetterPtr(pcir);
                rpwlSetter = m_hOutReturnPointWaveformLocation.createAttributesSetterPtr(pcir);
            }

            CT_MutablePointIterator it(pcir);

            int currentLocalIndex = 0;
            // read the points and convert them from LASZIP to CT format
            for(size_t i = 0; i < nPoints; i++)
            {
                // READ THE POINT
                if (lasZipReader.readPoint())
                {
                    laszip_point* point = lasZipReader.point();

                    // READ ALL ATTRIBUTES
                    qint32 x = point->X;
                    qint32 y = point->Y;
                    qint32 z = point->Z;

                    // CONVERT POINT
                    CT_Point pAdded;
                    if(mustTransformPoint)
                    {
                        double xc = 0;
                        double yc = 0;
                        double zc = 0;
                        header->transformPoint(x, y, z, xc, yc, zc);
                        pAdded(0) = xc;
                        pAdded(1) = yc;
                        pAdded(2) = zc;
                    }
                    else
                    {
                        pAdded(0) = x;
                        pAdded(1) = y;
                        pAdded(2) = z;
                    }

                    if (!isPointFiltered(pAdded))
                    {
                        it.next().replaceCurrentPoint(pAdded);

                        intensitySetter.setValueWithLocalIndex(currentLocalIndex, point->intensity);

                        PointCore6_10 valuePointCore6_10;
                        if (header->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            valuePointCore6_10.entire = point->return_number +
                                    (point->number_of_returns << 3) +
                                    (point->scan_direction_flag << 6) +
                                    (point->edge_of_flight_line << 7) +
                                    (point->synthetic_flag << 8) +
                                    (point->keypoint_flag << 9) +
                                    (point->withheld_flag << 10);
                        }
                        else // LAS 1.4
                        {
                            valuePointCore6_10.entire = (quint16)point->extended_return_number +
                                                        ((quint16)point->extended_number_of_returns << 4) +
                                                        ((quint16)point->extended_classification_flags << 8) +
                                                        ((quint16)point->extended_scanner_channel << 12) +
                                                        ((quint16)point->scan_direction_flag << 14) +
                                                        ((quint16)point->edge_of_flight_line << 15);
                        }

                        core6_10Setter.setValueWithLocalIndex(currentLocalIndex, valuePointCore6_10);

                        if (header->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            classificationSetter.setValueWithLocalIndex(currentLocalIndex, point->classification);
                        }
                        else // LAS 1.4
                        {
                            classificationSetter.setValueWithLocalIndex(currentLocalIndex, point->extended_classification);
                        }

                        if(header->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            scanAngleSetter.setValueWithLocalIndex(currentLocalIndex, point->scan_angle_rank);
                            userDataSetter.setValueWithLocalIndex(currentLocalIndex, point->user_data);
                        }
                        else // LAS 1.4
                        {
                            // "user data" is before "scan angle" in version 6 to 10
                            userDataSetter.setValueWithLocalIndex(currentLocalIndex, point->user_data);
                            scanAngleSetter.setValueWithLocalIndex(currentLocalIndex, point->extended_scan_angle);
                        }

                        pointSourceIDSetter.setValueWithLocalIndex(currentLocalIndex, point->point_source_ID);

                        // gps time is always after pointSourceID
                        if(gpsTimeSetter)
                            gpsTimeSetter->setValueWithLocalIndex(currentLocalIndex, point->gps_time);

                        // color is always after gpsTime if exist otherwise after pointSourceID
                        if(colorSetter)
                        {
                            redSetter->setValueWithLocalIndex(currentLocalIndex, point->rgb[0]);
                            greenSetter->setValueWithLocalIndex(currentLocalIndex, point->rgb[1]);
                            blueSetter->setValueWithLocalIndex(currentLocalIndex, point->rgb[2]);

                            CT_Color colRGBA;
                            colRGBA.r() = point->rgb[0]/256;
                            colRGBA.g() = point->rgb[1]/256;
                            colRGBA.b() = point->rgb[2]/256;
                            colRGBA.a() = 255;

                            colorSetter->setValueWithLocalIndex(currentLocalIndex, colRGBA);
                        }

                        // NIR is always after colors if exist
                        if(nirSetter)
                            nirSetter->setValueWithLocalIndex(currentLocalIndex, point->rgb[3]);

                        // wave packet is always after NIR if exist
                        if(wpdiSetter)
                        {
                            quint8 wpdi = point->wave_packet[0];
                            quint64 botwd = (quint64)point->wave_packet[1] +
                                            ((quint64)point->wave_packet[2] << 8) +
                                    ((quint64)point->wave_packet[3] << 16) +
                                    ((quint64)point->wave_packet[4] << 24) +
                                    ((quint64)point->wave_packet[5] << 32) +
                                    ((quint64)point->wave_packet[6] << 40) +
                                    ((quint64)point->wave_packet[7] << 48) +
                                    ((quint64)point->wave_packet[8] << 56);
                            quint32 wpsi = (quint32)point->wave_packet[9] +
                                           ((quint32)point->wave_packet[10] << 8) +
                                    ((quint32)point->wave_packet[11] << 16) +
                                    ((quint32)point->wave_packet[12] << 24);
                            float rpwl = static_cast<float>((quint32)point->wave_packet[13] +
                                         ((quint32)point->wave_packet[14] << 8) +
                                    ((quint32)point->wave_packet[15] << 16) +
                                    ((quint32)point->wave_packet[16] << 24));

                            wpdiSetter->setValueWithLocalIndex(currentLocalIndex, wpdi);
                            botwdSetter->setValueWithLocalIndex(currentLocalIndex, botwd);
                            wpsibSetter->setValueWithLocalIndex(currentLocalIndex, wpsi);
                            rpwlSetter->setValueWithLocalIndex(currentLocalIndex, rpwl);
                        }

                        ++currentLocalIndex;
                    }
                }

                setProgress(int((100.0*i)/nPoints));
            }

            CT_Scene *scene = new CT_Scene(pcir);
            scene->updateBoundingBox();

            m_readScene = scene;

            // add the scene
            group->addSingularItem(m_hOutScene, scene);

            // add attributes
            CT_StdLASPointsAttributesContainer *container = new CT_StdLASPointsAttributesContainer();

            // ItemDrawable 6_10
            CT_AbstractPointAttributesScalar* rn6_10 = nullptr;
            CT_AbstractPointAttributesScalar* nor6_10 = nullptr;
            CT_AbstractPointAttributesScalar* sdf6_10 = nullptr;
            CT_AbstractPointAttributesScalar* efl6_10 =  nullptr;
            CT_AbstractPointAttributesScalar* cf = nullptr;
            CT_AbstractPointAttributesScalar* sc = nullptr;

            if (header->m_pointDataRecordFormat < 6)
            {
                /* 0b 000 0 0 000 111 */ rn6_10  = m_hOutReturnNumber     .createInstance(  7, 0, pcir, mCore6_10Manager);
                /* 0b 000 0 0 111 000 */ nor6_10 = m_hOutNumberOfReturn   .createInstance( 56, 3, pcir, mCore6_10Manager);
                /* 0b 000 0 1 000 000 */ sdf6_10 = m_hOutScanDirectionFlag.createInstance( 64, 6, pcir, mCore6_10Manager);
                /* 0b 000 1 00 00 000 */ efl6_10 = m_hOutEdgeOfFlightLine .createInstance(128, 7, pcir, mCore6_10Manager);

                // set classification flag
                /* 0b 111 0 0 000 000 */ cf      = m_hOutClassificationFlag.createInstance(1792,  8, pcir, mCore6_10Manager);
            }
            else
            {
                /* 0b0000 0000 0000 1111 */ rn6_10  = m_hOutReturnNumber      .createInstance(   15,  0, pcir, mCore6_10Manager);
                /* 0b0000 0000 1111 0000 */ nor6_10 = m_hOutNumberOfReturn    .createInstance(  240,  4, pcir, mCore6_10Manager);
                /* 0b0000 1111 0000 0000 */ cf      = m_hOutClassificationFlag.createInstance( 3840,  8, pcir, mCore6_10Manager);
                /* 0b0011 0000 0000 0000 */ sc      = m_hOutScannerChannel    .createInstance(12288, 12, pcir, mCore6_10Manager);
                /* 0b0100 0000 0000 0000 */ sdf6_10 = m_hOutScanDirectionFlag .createInstance(16384, 14, pcir, mCore6_10Manager);
                /* 0b1000 0000 0000 0000 */ efl6_10 = m_hOutEdgeOfFlightLine  .createInstance(32768, 15, pcir, mCore6_10Manager);
            }

            CT_AbstractPointAttributesScalar* attReturnNumber = rn6_10;
            CT_AbstractPointAttributesScalar* attNumberOfReturn = nor6_10;
            CT_AbstractPointAttributesScalar* attClassificationFlag = cf;
            CT_AbstractPointAttributesScalar* attScannerChannel = sc;
            CT_AbstractPointAttributesScalar* attScanDirectionFlag = sdf6_10;
            CT_AbstractPointAttributesScalar* attEdgeOfFlightLine = efl6_10;
            CT_AbstractPointAttributesScalar* attIntensity = m_hOutIntensity.createAttributeInstance(pcir);
            CT_AbstractPointAttributesScalar* attClassification = m_hOutClassification.createAttributeInstance(pcir);
            CT_AbstractPointAttributesScalar* attUserData = m_hOutUserData.createAttributeInstance(pcir);
            CT_AbstractPointAttributesScalar* attPointSourceID = m_hOutPointSourceID.createAttributeInstance(pcir);
            CT_AbstractPointAttributesScalar* attScanAngle = m_hOutScanAngle.createAttributeInstance(pcir);
            CT_AbstractPointAttributesScalar* attGPSTime = (gpsTimeSetter ? m_hOutGPSTime.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attRed = (redSetter ? m_hOutRed.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attGreen = (greenSetter ? m_hOutGreen.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attBlue = (blueSetter ? m_hOutBlue.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attWavePacketDescriptorIndex = (wpdiSetter ? m_hOutWavePacketDescriptorIndex.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attByteOffsetToWaveformData = (botwdSetter ? m_hOutByteOffsetToWaveformData.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attWaveformPacketSizeInBytes = (wpsibSetter ? m_hOutWaveformPacketSizeInBytes.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attReturnPointWaveformLocation = (rpwlSetter ? m_hOutReturnPointWaveformLocation.createAttributeInstance(pcir) : nullptr);
            CT_AbstractPointAttributesScalar* attNIR = (nirSetter ? m_hOutNIR.createAttributeInstance(pcir) : nullptr);

            if (container != nullptr) {group->addSingularItem(m_hOutAllAtt, container);}
            if (attIntensity != nullptr) {group->addSingularItem(m_hOutIntensity, attIntensity);}
            if (attReturnNumber != nullptr) {group->addSingularItem(m_hOutReturnNumber, attReturnNumber);}
            if (attNumberOfReturn != nullptr) {group->addSingularItem(m_hOutNumberOfReturn, attNumberOfReturn);}
            if (attClassificationFlag != nullptr) {group->addSingularItem(m_hOutClassificationFlag, attClassificationFlag);}
            if (attScannerChannel != nullptr) {group->addSingularItem(m_hOutScannerChannel, attScannerChannel);}
            if (attScanDirectionFlag != nullptr) {group->addSingularItem(m_hOutScanDirectionFlag, attScanDirectionFlag);}
            if (attEdgeOfFlightLine != nullptr) {group->addSingularItem(m_hOutEdgeOfFlightLine, attEdgeOfFlightLine);}
            if (attClassification != nullptr) {group->addSingularItem(m_hOutClassification, attClassification);}
            if (attScanAngle != nullptr) {group->addSingularItem(m_hOutScanAngle, attScanAngle);}
            if (attUserData != nullptr) {group->addSingularItem(m_hOutUserData, attUserData);}
            if (attPointSourceID != nullptr) {group->addSingularItem(m_hOutPointSourceID, attPointSourceID);}
            if (attGPSTime != nullptr) {group->addSingularItem(m_hOutGPSTime, attGPSTime);}
            if (colorSetter) {group->addSingularItem(m_hOutColor, m_hOutColor.createAttributeInstance(pcir));}
            if (attRed != nullptr) {group->addSingularItem(m_hOutRed, attRed);}
            if (attGreen != nullptr) {group->addSingularItem(m_hOutGreen, attGreen);}
            if (attBlue != nullptr) {group->addSingularItem(m_hOutBlue, attBlue);}
            if (attWavePacketDescriptorIndex != nullptr) {group->addSingularItem(m_hOutWavePacketDescriptorIndex, attWavePacketDescriptorIndex);}
            if (attByteOffsetToWaveformData != nullptr) {group->addSingularItem(m_hOutByteOffsetToWaveformData, attByteOffsetToWaveformData);}
            if (attWaveformPacketSizeInBytes != nullptr) {group->addSingularItem(m_hOutWaveformPacketSizeInBytes, attWaveformPacketSizeInBytes);}
            if (attReturnPointWaveformLocation != nullptr) {group->addSingularItem(m_hOutReturnPointWaveformLocation, attReturnPointWaveformLocation);}
            if (attNIR != nullptr) {group->addSingularItem(m_hOutNIR, attNIR);}

            container->insertPointsAttributesAt(CT_LasDefine::Intensity, attIntensity);
            container->insertPointsAttributesAt(CT_LasDefine::Return_Number, attReturnNumber);
            container->insertPointsAttributesAt(CT_LasDefine::Number_of_Returns, attNumberOfReturn);
            container->insertPointsAttributesAt(CT_LasDefine::Classification_Flags, attClassificationFlag);
            container->insertPointsAttributesAt(CT_LasDefine::Scanner_Channel, attScannerChannel);
            container->insertPointsAttributesAt(CT_LasDefine::Scan_Direction_Flag, attScanDirectionFlag);
            container->insertPointsAttributesAt(CT_LasDefine::Edge_of_Flight_Line, attEdgeOfFlightLine);
            container->insertPointsAttributesAt(CT_LasDefine::Classification, attClassification);
            container->insertPointsAttributesAt(CT_LasDefine::Scan_Angle_Rank, attScanAngle);
            container->insertPointsAttributesAt(CT_LasDefine::User_Data, attUserData);
            container->insertPointsAttributesAt(CT_LasDefine::Point_Source_ID, attPointSourceID);
            container->insertPointsAttributesAt(CT_LasDefine::GPS_Time, attGPSTime);
            container->insertPointsAttributesAt(CT_LasDefine::Red, attRed);
            container->insertPointsAttributesAt(CT_LasDefine::Green, attGreen);
            container->insertPointsAttributesAt(CT_LasDefine::Blue, attBlue);
            container->insertPointsAttributesAt(CT_LasDefine::NIR, attNIR);
            container->insertPointsAttributesAt(CT_LasDefine::Wave_Packet_Descriptor_Index, attWavePacketDescriptorIndex);
            container->insertPointsAttributesAt(CT_LasDefine::Byte_offset_to_waveform_data, attByteOffsetToWaveformData);
            container->insertPointsAttributesAt(CT_LasDefine::Waveform_packet_size_in_bytes, attWaveformPacketSizeInBytes);
            container->insertPointsAttributesAt(CT_LasDefine::Return_Point_Waveform_Location, attReturnPointWaveformLocation);

        } else {
            valid = false;
            PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_Reader_LAZ::internalReadFile : Impossible de lire les points"));
        }
    }

    lasZipReader.closeFile();

    delete header;
    return valid;
}

bool CT_Reader_LAZ::internalReadMultiFile(CT_StandardItemGroup *group)
{
    bool valid = true;
    QString error;

    const QList<CT_AbstractReader::FileBuffer>& fileBufferList = multipleFilepath();

    qint64 nPoints = 0;

    // Loop over file headers
    for(auto &fileBuffer: fileBufferList)
    {
        nPoints += fileBuffer.nPoints;
    }

    // Check that there are points
    if(nPoints == 0)
    {
        PS_LOG->addWarningMessage(LogInterface::reader, tr("Aucun points contenu dans les fichiers."));
        return true;
    }

    // Load the ref file header for information (filepath is the ref file or the first one, if there is no ref file)
    CT_LAZHeader *header = static_cast<CT_LAZHeader*>(internalReadHeader(filepath(), error));

    setToolTip(header->toString());

    // Create PCIR and associated data
    CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(nPoints);

    CT_MutablePointIterator it(pcir);

    auto scanAngleSetter = m_hOutScanAngle.createAttributesSetter(pcir);
    auto core6_10Setter = mCore6_10Manager.createAttributesSetter(pcir);

    auto intensitySetter = m_hOutIntensity.createAttributesSetter(pcir);
    auto classificationSetter = m_hOutClassification.createAttributesSetter(pcir);
    auto userDataSetter = m_hOutUserData.createAttributesSetter(pcir);
    auto pointSourceIDSetter = m_hOutPointSourceID.createAttributesSetter(pcir);

    decltype (m_hOutGPSTime)::SetterPtr gpsTimeSetter = nullptr;
    decltype (m_hOutColor)::SetterPtr colorSetter = nullptr;
    decltype (m_hOutRed)::SetterPtr redSetter = nullptr;
    decltype (m_hOutGreen)::SetterPtr greenSetter = nullptr;
    decltype (m_hOutBlue)::SetterPtr blueSetter = nullptr;
    decltype (m_hOutWaveformPacketSizeInBytes)::SetterPtr wpsibSetter = nullptr;
    decltype (m_hOutByteOffsetToWaveformData)::SetterPtr botwdSetter = nullptr;
    decltype (m_hOutWavePacketDescriptorIndex)::SetterPtr wpdiSetter = nullptr;
    decltype (m_hOutReturnPointWaveformLocation)::SetterPtr rpwlSetter = nullptr;
    decltype (m_hOutNIR)::SetterPtr nirSetter = nullptr;

    if((header->m_pointDataRecordFormat == 1)
       || (header->m_pointDataRecordFormat > 2))
    {
        gpsTimeSetter = m_hOutGPSTime.createAttributesSetterPtr(pcir);
    }

    if((header->m_pointDataRecordFormat == 2)
       || (header->m_pointDataRecordFormat == 3)
       || (header->m_pointDataRecordFormat == 5)
       || (header->m_pointDataRecordFormat == 7)
       || (header->m_pointDataRecordFormat == 8)
       || (header->m_pointDataRecordFormat == 10))
    {
        colorSetter = m_hOutColor.createAttributesSetterPtr(pcir);
        redSetter = m_hOutRed.createAttributesSetterPtr(pcir);
        greenSetter = m_hOutGreen.createAttributesSetterPtr(pcir);
        blueSetter = m_hOutBlue.createAttributesSetterPtr(pcir);
    }

    if((header->m_pointDataRecordFormat == 8)
       || (header->m_pointDataRecordFormat == 10))
    {
        nirSetter = m_hOutNIR.createAttributesSetterPtr(pcir);
    }

    if((header->m_pointDataRecordFormat == 4)
       || (header->m_pointDataRecordFormat == 5)
       || (header->m_pointDataRecordFormat == 9)
       || (header->m_pointDataRecordFormat == 10))
    {
        wpdiSetter = m_hOutWavePacketDescriptorIndex.createAttributesSetterPtr(pcir);
        botwdSetter = m_hOutByteOffsetToWaveformData.createAttributesSetterPtr(pcir);
        wpsibSetter = m_hOutWaveformPacketSizeInBytes.createAttributesSetterPtr(pcir);
        rpwlSetter = m_hOutReturnPointWaveformLocation.createAttributesSetterPtr(pcir);
    }

    // Loop over files and points to extract data
    size_t k = 0;
    for(auto &fileBuffer: fileBufferList)
    {
        CT_LAZHeader *header_temp = static_cast<CT_LAZHeader*>(internalReadHeader(fileBuffer.filename, error));
        auto filepath = header_temp->filePath();

        CT_LASZIP_Reader lasZipReader(filepath);

        if (lasZipReader.openFile())
        {
            // get a pointer to the points that will be read
            if (lasZipReader.initPointPointer())
            {
                for(const qint64 &i: fileBuffer.indexList)
                {
                    // Seek to wanted position
                    lasZipReader.seek(i);

                    // READ THE POINT
                    if (lasZipReader.readPoint())
                    {
                        laszip_point* point = lasZipReader.point();

                        // READ ALL ATTRIBUTES
                        qint32 x = point->X;
                        qint32 y = point->Y;
                        qint32 z = point->Z;

                        intensitySetter.setValueWithLocalIndex(k, point->intensity);

                        PointCore6_10 valuePointCore6_10;
                        if (header_temp->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            valuePointCore6_10.entire = point->return_number +
                                                        (point->number_of_returns << 3) +
                                                        (point->scan_direction_flag << 6) +
                                                        (point->edge_of_flight_line << 7);
                        }
                        else // LAS 1.4
                        {
                            valuePointCore6_10.entire = (quint16)point->extended_return_number +
                                                        ((quint16)point->extended_number_of_returns << 4) +
                                                        ((quint16)point->extended_classification_flags << 8) +
                                                        ((quint16)point->extended_scanner_channel << 12) +
                                                        ((quint16)point->scan_direction_flag << 14) +
                                                        ((quint16)point->edge_of_flight_line << 15);
                        }

                        core6_10Setter.setValueWithLocalIndex(k, valuePointCore6_10);

                        if (header_temp->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            classificationSetter.setValueWithLocalIndex(k, point->classification);
                        }
                        else // LAS 1.4
                        {
                            classificationSetter.setValueWithLocalIndex(k, point->extended_classification);
                        }

                        if(header_temp->m_pointDataRecordFormat < 6) // LAS <= 1.3
                        {
                            scanAngleSetter.setValueWithLocalIndex(k, point->scan_angle_rank);
                            userDataSetter.setValueWithLocalIndex(k, point->user_data);
                        }
                        else // LAS 1.4
                        {
                            // "user data" is before "scan angle" in version 6 to 10
                            userDataSetter.setValueWithLocalIndex(k, point->user_data);
                            scanAngleSetter.setValueWithLocalIndex(k, point->extended_scan_angle);
                        }

                        pointSourceIDSetter.setValueWithLocalIndex(k, point->point_source_ID);

                        // gps time is always after pointSourceID
                        if(gpsTimeSetter)
                            gpsTimeSetter->setValueWithLocalIndex(k, point->gps_time);

                        // color is always after gpsTime if exist otherwise after pointSourceID
                        if(colorSetter)
                        {
                            redSetter->setValueWithLocalIndex(k, point->rgb[0]);
                            greenSetter->setValueWithLocalIndex(k, point->rgb[1]);
                            blueSetter->setValueWithLocalIndex(k, point->rgb[2]);

                            CT_Color colRGBA;
                            colRGBA.r() = point->rgb[0]/256;
                            colRGBA.g() = point->rgb[1]/256;
                            colRGBA.b() = point->rgb[2]/256;
                            colRGBA.a() = 255;

                            colorSetter->setValueWithLocalIndex(k, colRGBA);
                        }

                        // NIR is always after colors if exist
                        if(nirSetter)
                            nirSetter->setValueWithLocalIndex(k, point->rgb[3]);

                        // wave packet is always after NIR if exist
                        if(wpdiSetter)
                        {
                            quint8 wpdi = point->wave_packet[0];
                            quint64 botwd = (quint64)point->wave_packet[1] +
                                            ((quint64)point->wave_packet[2] << 8) +
                                    ((quint64)point->wave_packet[3] << 16) +
                                    ((quint64)point->wave_packet[4] << 24) +
                                    ((quint64)point->wave_packet[5] << 32) +
                                    ((quint64)point->wave_packet[6] << 40) +
                                    ((quint64)point->wave_packet[7] << 48) +
                                    ((quint64)point->wave_packet[8] << 56);
                            quint32 wpsi = (quint32)point->wave_packet[9] +
                                           ((quint32)point->wave_packet[10] << 8) +
                                    ((quint32)point->wave_packet[11] << 16) +
                                    ((quint32)point->wave_packet[12] << 24);
                            float rpwl = static_cast<float>((quint32)point->wave_packet[13] +
                                         ((quint32)point->wave_packet[14] << 8) +
                                    ((quint32)point->wave_packet[15] << 16) +
                                    ((quint32)point->wave_packet[16] << 24));

                            wpdiSetter->setValueWithLocalIndex(k, wpdi);
                            botwdSetter->setValueWithLocalIndex(k, botwd);
                            wpsibSetter->setValueWithLocalIndex(k, wpsi);
                            rpwlSetter->setValueWithLocalIndex(k, rpwl);
                        }

                        // CONVERT POINT
                        CT_Point pAdded;
                        if(header_temp->mustTransformPoints())
                        {
                            double xc = 0;
                            double yc = 0;
                            double zc = 0;
                            header_temp->transformPoint(x, y, z, xc, yc, zc);

                            pAdded(0) = xc;
                            pAdded(1) = yc;
                            pAdded(2) = zc;
                        }
                        else
                        {
                            pAdded(0) = x;
                            pAdded(1) = y;
                            pAdded(2) = z;
                        }

                        it.next().replaceCurrentPoint(pAdded);

                        k++;
                        setProgress(int((100.0*k)/nPoints));
                    }
                }
            }
        }

        lasZipReader.closeFile();
        delete header_temp;
    }

    // Finalize attribute assignment between PCIR and values
    CT_Scene *scene = new CT_Scene(pcir);
    group->addSingularItem(m_hOutScene, scene);

    scene->updateBoundingBox();

    CT_StdLASPointsAttributesContainer *container = new CT_StdLASPointsAttributesContainer();

    CT_AbstractPointAttributesScalar* rn6_10 = nullptr;
    CT_AbstractPointAttributesScalar* nor6_10 = nullptr;
    CT_AbstractPointAttributesScalar* sdf6_10 = nullptr;
    CT_AbstractPointAttributesScalar* efl6_10 =  nullptr;
    CT_AbstractPointAttributesScalar* cf = nullptr;
    CT_AbstractPointAttributesScalar* sc = nullptr;

    if (header->m_pointDataRecordFormat < 6)
    {
        /* 0b00000111 */ rn6_10  = m_hOutReturnNumber     .createInstance(  7, 0, pcir, mCore6_10Manager);
        /* 0b00111000 */ nor6_10 = m_hOutNumberOfReturn   .createInstance( 56, 3, pcir, mCore6_10Manager);
        /* 0b01000000 */ sdf6_10 = m_hOutScanDirectionFlag.createInstance( 64, 6, pcir, mCore6_10Manager);
        /* 0b10000000 */ efl6_10 = m_hOutEdgeOfFlightLine .createInstance(128, 7, pcir, mCore6_10Manager);
    }
    else
    {
        /* 0b0000 0000 0000 1111 */ rn6_10  = m_hOutReturnNumber      .createInstance(   15,  0, pcir, mCore6_10Manager);
        /* 0b0000 0000 1111 0000 */ nor6_10 = m_hOutNumberOfReturn    .createInstance(  240,  4, pcir, mCore6_10Manager);
        /* 0b0000 1111 0000 0000 */ cf      = m_hOutClassificationFlag.createInstance( 3840,  8, pcir, mCore6_10Manager);
        /* 0b0011 0000 0000 0000 */ sc      = m_hOutScannerChannel    .createInstance(12288, 12, pcir, mCore6_10Manager);
        /* 0b0100 0000 0000 0000 */ sdf6_10 = m_hOutScanDirectionFlag .createInstance(16384, 14, pcir, mCore6_10Manager);
        /* 0b1000 0000 0000 0000 */ efl6_10 = m_hOutEdgeOfFlightLine  .createInstance(32768, 15, pcir, mCore6_10Manager);
    }

    CT_AbstractPointAttributesScalar* attReturnNumber = rn6_10;
    CT_AbstractPointAttributesScalar* attNumberOfReturn = nor6_10;
    CT_AbstractPointAttributesScalar* attClassificationFlag = cf;
    CT_AbstractPointAttributesScalar* attScannerChannel = sc;
    CT_AbstractPointAttributesScalar* attScanDirectionFlag = sdf6_10;
    CT_AbstractPointAttributesScalar* attEdgeOfFlightLine = efl6_10;
    CT_AbstractPointAttributesScalar* attIntensity = m_hOutIntensity.createAttributeInstance(pcir);
    CT_AbstractPointAttributesScalar* attClassification = m_hOutClassification.createAttributeInstance(pcir);
    CT_AbstractPointAttributesScalar* attUserData = m_hOutUserData.createAttributeInstance(pcir);
    CT_AbstractPointAttributesScalar* attPointSourceID = m_hOutPointSourceID.createAttributeInstance(pcir);
    CT_AbstractPointAttributesScalar* attScanAngle = m_hOutScanAngle.createAttributeInstance(pcir);
    CT_AbstractPointAttributesScalar* attGPSTime = (gpsTimeSetter ? m_hOutGPSTime.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attRed = (redSetter ? m_hOutRed.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attGreen = (greenSetter ? m_hOutGreen.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attBlue = (blueSetter ? m_hOutBlue.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attWavePacketDescriptorIndex = (wpdiSetter ? m_hOutWavePacketDescriptorIndex.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attByteOffsetToWaveformData = (botwdSetter ? m_hOutByteOffsetToWaveformData.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attWaveformPacketSizeInBytes = (wpsibSetter ? m_hOutWaveformPacketSizeInBytes.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attReturnPointWaveformLocation = (rpwlSetter ? m_hOutReturnPointWaveformLocation.createAttributeInstance(pcir) : nullptr);
    CT_AbstractPointAttributesScalar* attNIR = (nirSetter ? m_hOutNIR.createAttributeInstance(pcir) : nullptr);

    if (container != nullptr) {group->addSingularItem(m_hOutAllAtt, container);}
    if (attIntensity != nullptr) {group->addSingularItem(m_hOutIntensity, attIntensity);}
    if (attReturnNumber != nullptr) {group->addSingularItem(m_hOutReturnNumber, attReturnNumber);}
    if (attNumberOfReturn != nullptr) {group->addSingularItem(m_hOutNumberOfReturn, attNumberOfReturn);}
    if (attClassificationFlag != nullptr) {group->addSingularItem(m_hOutClassificationFlag, attClassificationFlag);}
    if (attScannerChannel != nullptr) {group->addSingularItem(m_hOutScannerChannel, attScannerChannel);}
    if (attScanDirectionFlag != nullptr) {group->addSingularItem(m_hOutScanDirectionFlag, attScanDirectionFlag);}
    if (attEdgeOfFlightLine != nullptr) {group->addSingularItem(m_hOutEdgeOfFlightLine, attEdgeOfFlightLine);}
    if (attClassification != nullptr) {group->addSingularItem(m_hOutClassification, attClassification);}
    if (attScanAngle != nullptr) {group->addSingularItem(m_hOutScanAngle, attScanAngle);}
    if (attUserData != nullptr) {group->addSingularItem(m_hOutUserData, attUserData);}
    if (attPointSourceID != nullptr) {group->addSingularItem(m_hOutPointSourceID, attPointSourceID);}
    if (attGPSTime != nullptr) {group->addSingularItem(m_hOutGPSTime, attGPSTime);}
    if (colorSetter) {group->addSingularItem(m_hOutColor, m_hOutColor.createAttributeInstance(pcir));}
    if (attRed != nullptr) {group->addSingularItem(m_hOutRed, attRed);}
    if (attGreen != nullptr) {group->addSingularItem(m_hOutGreen, attGreen);}
    if (attBlue != nullptr) {group->addSingularItem(m_hOutBlue, attBlue);}
    if (attWavePacketDescriptorIndex != nullptr) {group->addSingularItem(m_hOutWavePacketDescriptorIndex, attWavePacketDescriptorIndex);}
    if (attByteOffsetToWaveformData != nullptr) {group->addSingularItem(m_hOutByteOffsetToWaveformData, attByteOffsetToWaveformData);}
    if (attWaveformPacketSizeInBytes != nullptr) {group->addSingularItem(m_hOutWaveformPacketSizeInBytes, attWaveformPacketSizeInBytes);}
    if (attReturnPointWaveformLocation != nullptr) {group->addSingularItem(m_hOutReturnPointWaveformLocation, attReturnPointWaveformLocation);}
    if (attNIR != nullptr) {group->addSingularItem(m_hOutNIR, attNIR);}

    container->insertPointsAttributesAt(CT_LasDefine::Intensity, attIntensity);
    container->insertPointsAttributesAt(CT_LasDefine::Return_Number, attReturnNumber);
    container->insertPointsAttributesAt(CT_LasDefine::Number_of_Returns, attNumberOfReturn);
    container->insertPointsAttributesAt(CT_LasDefine::Classification_Flags, attClassificationFlag);
    container->insertPointsAttributesAt(CT_LasDefine::Scanner_Channel, attScannerChannel);
    container->insertPointsAttributesAt(CT_LasDefine::Scan_Direction_Flag, attScanDirectionFlag);
    container->insertPointsAttributesAt(CT_LasDefine::Edge_of_Flight_Line, attEdgeOfFlightLine);
    container->insertPointsAttributesAt(CT_LasDefine::Classification, attClassification);
    container->insertPointsAttributesAt(CT_LasDefine::Scan_Angle_Rank, attScanAngle);
    container->insertPointsAttributesAt(CT_LasDefine::User_Data, attUserData);
    container->insertPointsAttributesAt(CT_LasDefine::Point_Source_ID, attPointSourceID);
    container->insertPointsAttributesAt(CT_LasDefine::GPS_Time, attGPSTime);
    container->insertPointsAttributesAt(CT_LasDefine::Red, attRed);
    container->insertPointsAttributesAt(CT_LasDefine::Green, attGreen);
    container->insertPointsAttributesAt(CT_LasDefine::Blue, attBlue);
    container->insertPointsAttributesAt(CT_LasDefine::NIR, attNIR);
    container->insertPointsAttributesAt(CT_LasDefine::Wave_Packet_Descriptor_Index, attWavePacketDescriptorIndex);
    container->insertPointsAttributesAt(CT_LasDefine::Byte_offset_to_waveform_data, attByteOffsetToWaveformData);
    container->insertPointsAttributesAt(CT_LasDefine::Waveform_packet_size_in_bytes, attWaveformPacketSizeInBytes);
    container->insertPointsAttributesAt(CT_LasDefine::Return_Point_Waveform_Location, attReturnPointWaveformLocation);

    delete header;
    return valid;
}


void CT_Reader_LAZ::getPointIndicesInside2DShape(QList<CandidateShape2D> &candidateShapes) const
{
    QString error;
    CT_LAZHeader *header = static_cast<CT_LAZHeader*>(internalReadHeader(filepath(), error));

    if(header == nullptr)
    {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("CT_Reader_LAZ::getPointIndicesInside2DShape : Impossible de lire l'en-tête du fichier %1").arg(filepath()));
        return;
    }

    qint64 nPoints = qint64(header->getPointsRecordCount());
    if(nPoints == 0)
    {
        PS_LOG->addWarningMessage(LogInterface::reader, tr("CT_Reader_LAZ::getPointIndicesInside2DShape : Aucun points contenu dans le fichier %1").arg(filepath()));
        delete header;
        return;
    }

    auto filepath = CT_AbstractReader::filepath();

    CT_LASZIP_Reader lasZipReader(filepath);

    if (lasZipReader.openFile())
    {
        // get a pointer to the points that will be read
        if (lasZipReader.initPointPointer())
        {
                bool mustTransformPoint = header->mustTransformPoints();

                qint32 x, y, z;
                double xc = 0;
                double yc = 0;
                double zc = 0;

                for (CandidateShape2D& sh : candidateShapes)
                {
                    sh._all = true;
                }

                for (qint64 i = 0; i < nPoints; ++i)
                {
                    // READ THE POINT
                    if (lasZipReader.readPoint())
                    {
                        laszip_point* point = lasZipReader.point();

                        x = point->X;
                        y = point->Y;
                        z = point->Z;

                        // CONVERT POINT
                        if(mustTransformPoint)
                        {
                            header->transformPoint(x, y, z, xc, yc, zc);
                        } else
                        {
                            xc = x;
                            yc = y;
                        }

                        for (CandidateShape2D& sh : candidateShapes)
                        {
                            if (sh._area2D->contains(xc, yc))
                            {
                                if (sh._all)
                                {
                                    sh._lastIncludedIndex = i; // avoid list filling if all points included
                                } else {
                                    sh._indicesAfterLastIncludedIndex.push_back(i);
                                }
                            } else {
                                sh._all = false;
                            }

                        }
                    }
                }

                for (CandidateShape2D& sh : candidateShapes)
                {
                    if (sh._all || sh._lastIncludedIndex > -1 || sh._indicesAfterLastIncludedIndex.size() > 0)
                    {
                        sh._asPointsInside = true;
                    } else {
                        sh._asPointsInside = false;
                    }
                }
        }
    }

    lasZipReader.closeFile();
    delete header;
}

QString CT_Reader_LAZ::getFormatCode() const
{
    return "LAZ";
}
