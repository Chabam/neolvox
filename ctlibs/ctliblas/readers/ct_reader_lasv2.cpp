#include "ct_reader_lasv2.h"

#include "ct_colorcloud/ct_colorcloudstdvector.h"
#include "ct_iterator/ct_mutablepointiterator.h"

#include "ct_log/ct_logmanager.h"

#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QDate>

#include <QDebug>

CT_Reader_LASV2::CT_Reader_LASV2(int subMenuLevel) : SuperClass(subMenuLevel), CT_ReaderPointsFilteringExtension()
{    
    m_headerFromConfiguration = nullptr;

    addNewReadableFormat(FileFormat("las", tr("Fichiers LAS .las")));

    setToolTip(tr("Charge des points depuis un fichier au format LAS (ASPRS)<br>http://www.asprs.org/Committee-General/LASer-LAS-File-Format-Exchange-Activities.html"));
}

CT_Reader_LASV2::CT_Reader_LASV2(const CT_Reader_LASV2 &other) : SuperClass(other), CT_ReaderPointsFilteringExtension()
{
    m_headerFromConfiguration = nullptr;

    if(other.m_headerFromConfiguration != nullptr)
        m_headerFromConfiguration = static_cast<CT_LASHeader*>(other.m_headerFromConfiguration->copy(nullptr, nullptr));
}

CT_Reader_LASV2::~CT_Reader_LASV2()
{
    delete m_headerFromConfiguration;
}

QString CT_Reader_LASV2::displayableName() const
{
    return tr("Points, format LAS");
}

bool CT_Reader_LASV2::setFilePath(const QString& filepath)
{
    QString err;
    CT_LASHeader *header = static_cast<CT_LASHeader*>(internalReadHeader(filepath, err));

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

bool CT_Reader_LASV2::configure()
{
    QString err;
    CT_LASHeader *header = static_cast<CT_LASHeader*>(internalReadHeader(filepath(), err));

    if(header != nullptr) {
        delete m_headerFromConfiguration;
        m_headerFromConfiguration = header;
        return true;
    }

    delete header;
    return false;
}

CT_FileHeader* CT_Reader_LASV2::createHeaderPrototype() const
{
    return new CT_LASHeader();
}

bool CT_Reader_LASV2::restoreSettings(SettingsReaderInterface &reader)
{
    delete m_headerFromConfiguration;
    m_headerFromConfiguration = nullptr;

    if(SuperClass::restoreSettings(reader)) {
        QString error;
        m_headerFromConfiguration = static_cast<CT_LASHeader*>(internalReadHeader(filepath(), error));
    }

    return (m_headerFromConfiguration != nullptr);
}

void CT_Reader_LASV2::internalDeclareOutputModels(CT_ReaderOutModelStructureManager &manager)
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

CT_FileHeader *CT_Reader_LASV2::internalReadHeader(const QString &filepath, QString &error) const
{
    CT_LASHeader *header = nullptr;

    // Test File validity
    if(QFile::exists(filepath))
    {
        QFile f(filepath);

        if(f.open(QIODevice::ReadOnly))
        {
            QDataStream stream(&f);

            header = new CT_LASHeader();
            header->setFilePath(filepath);

            if(!header->read(stream, error)) {
                delete header;
                header = nullptr;
            }
        }

        f.close();
    }

    return header;
}

bool CT_Reader_LASV2::internalReadFile(CT_StandardItemGroup *group)
{
    bool ok = false;
    QString error;

    CT_LASHeader *header = static_cast<CT_LASHeader*>(internalReadHeader(filepath(), error));

    if(header == nullptr) {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Impossible de lire l'en-tête du fichier %1").arg(filepath()));
        return false;
    }

//    if(header->m_pointDataRecordFormat != m_headerFromConfiguration->m_pointDataRecordFormat) {
//        PS_LOG->addErrorMessage(LogInterface::reader, tr("Le format (%1) du fichier %2 ne correspond pas au format (%3) du fichier lu lors de la configuration").arg(header->m_pointDataRecordFormat)
//                                .arg(filepath())
//                                .arg(m_headerFromConfiguration->m_pointDataRecordFormat));
//        delete header;
//        return false;
//    }

    size_t nPoints = header->getPointsRecordCount();

    if(nPoints == 0) {
        PS_LOG->addWarningMessage(LogInterface::reader, tr("Aucun points contenu dans le fichier %1").arg(filepath()));
        delete header;
        return true;
    }

    QFile f(filepath());

    if(f.open(QIODevice::ReadOnly))
    {
        setToolTip(header->toString());

        bool mustTransformPoint = header->mustTransformPoints();

        // If filtering function set, do a first scan of the file to count filtered point number
        size_t numberOfFilteredPoints = 0;
        if (filterSet())
        {
            f.seek(header->m_offsetToPointData);

            QDataStream stream(&f);
            stream.setByteOrder(QDataStream::LittleEndian);

            qint64 pos = f.pos();

            qint32 x, y, z;
            double xc = 0;
            double yc = 0;
            double zc = 0;
            CT_Point pAdded;

            for (size_t i = 0; i < nPoints; ++i)
            {
                // READ ALL ATTRIBUTES
                stream >> x >> y >> z;

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

                pos += header->m_pointDataRecordLength;
                f.seek(pos);
            }
        }


        // read data
        f.seek(header->m_offsetToPointData);

        QDataStream stream(&f);
        stream.setByteOrder(QDataStream::LittleEndian);

        qint64 pos = f.pos();

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

        qint32 x, y, z;
        double xc = 0;
        double yc = 0;
        double zc = 0;
        CT_Point pAdded;
        quint16 valueUint16;
        quint8 valueUint8;
        qint8 valueInt8;
        PointCore6_10 valuePointCore6_10;
        CT_Color colRGBA;
        quint16 colR;
        quint16 colG;
        quint16 colB;

        int currentLocalIndex = 0;
        for (size_t i = 0; i < nPoints; ++i)
        {
            // READ ALL ATTRIBUTES
            stream >> x >> y >> z;

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

            if (!isPointFiltered(pAdded))
            {
                it.next().replaceCurrentPoint(pAdded);

                intensitySetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream);


                if (header->m_pointDataRecordFormat < 6)
                {
                    stream >> valueUint8;
                }
                else
                {
                    stream >> valueUint16;
                }

                //classificationSetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream); // old  version

                quint8 classificationBase;
                stream >> classificationBase;

                quint8 classificationFiltered;
                quint16 classifFlag_0_5 = 0;

                if(header->m_pointDataRecordFormat < 6)
                {
                    classificationFiltered = classificationBase & 31;

                    // compute classification scan from 3 left bits
                    classifFlag_0_5 = classificationBase & 224;
                    classifFlag_0_5 = classifFlag_0_5 >> 5;
                    classifFlag_0_5 = classifFlag_0_5 << 8;

                } else {
                    classificationFiltered = classificationBase;
                }

                classificationSetter.setValueWithLocalIndex(currentLocalIndex, classificationFiltered);


                if (header->m_pointDataRecordFormat < 6)
                {
                    valuePointCore6_10.entire = valueUint8;

                    // add classification flag data
                    valuePointCore6_10.entire = valuePointCore6_10.entire | classifFlag_0_5;

                    core6_10Setter.setValueWithLocalIndex(currentLocalIndex, valuePointCore6_10);
                }
                else
                {
                    valuePointCore6_10.entire = valueUint16;
                    core6_10Setter.setValueWithLocalIndex(currentLocalIndex, valuePointCore6_10);
                }



                if(header->m_pointDataRecordFormat < 6)
                {
                    stream >> valueInt8;
                    scanAngleSetter.setValueWithLocalIndex(currentLocalIndex, valueInt8);
                    userDataSetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                }
                else
                {
                    // "user data" is before "scan angle" in version 6 to 10
                    userDataSetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                    scanAngleSetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                }

                pointSourceIDSetter.setValueWithStreamWithLocalIndex(currentLocalIndex, stream);

                // gps time is always after pointSourceID
                if(gpsTimeSetter)
                    gpsTimeSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);

                // color is always after gpsTime if exist otherwise after pointSourceID
                if(colorSetter)
                {
                    stream >> colR >> colG >> colB;

                    redSetter->setValueWithLocalIndex(currentLocalIndex, colR);
                    greenSetter->setValueWithLocalIndex(currentLocalIndex, colG);
                    blueSetter->setValueWithLocalIndex(currentLocalIndex, colB);

                    colRGBA.r() = colR/256;
                    colRGBA.g() = colG/256;
                    colRGBA.b() = colB/256;
                    colRGBA.a() = 255;

                    colorSetter->setValueWithLocalIndex(currentLocalIndex, colRGBA);
                }

                // NIR is always after colors if exist
                if(nirSetter)
                    nirSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);

                // wave packet is always after NIR if exist
                if(wpdiSetter)
                {
                    wpdiSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                    botwdSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                    wpsibSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                    rpwlSetter->setValueWithStreamWithLocalIndex(currentLocalIndex, stream);
                }

                ++currentLocalIndex;
            }

            pos += header->m_pointDataRecordLength;
            f.seek(pos);

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
            /* 0b 000 1 0 000 000 */ efl6_10 = m_hOutEdgeOfFlightLine .createInstance(128, 7, pcir, mCore6_10Manager);

            // set classification flag
            /* 0b 111 0 0 000 000 */ cf      = m_hOutClassificationFlag.createInstance(1792,  8, pcir, mCore6_10Manager);
        }
        else
        {
            /* 0b 0000 0000 0000 1111 */ rn6_10  = m_hOutReturnNumber      .createInstance(   15,  0, pcir, mCore6_10Manager);
            /* 0b 0000 0000 1111 0000 */ nor6_10 = m_hOutNumberOfReturn    .createInstance(  240,  4, pcir, mCore6_10Manager);
            /* 0b 0000 1111 0000 0000 */ cf      = m_hOutClassificationFlag.createInstance( 3840,  8, pcir, mCore6_10Manager);
            /* 0b 0011 0000 0000 0000 */ sc      = m_hOutScannerChannel    .createInstance(12288, 12, pcir, mCore6_10Manager);
            /* 0b 0100 0000 0000 0000 */ sdf6_10 = m_hOutScanDirectionFlag .createInstance(16384, 14, pcir, mCore6_10Manager);
            /* 0b 1000 0000 0000 0000 */ efl6_10 = m_hOutEdgeOfFlightLine  .createInstance(32768, 15, pcir, mCore6_10Manager);
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

        ok = true;
    } else {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Error while loading the file ") + filepath());
    }

    f.close();

    delete header;

    return ok;

}

bool CT_Reader_LASV2::internalReadMultiFile(CT_StandardItemGroup *group)
{
    bool ok = false;
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
    CT_LASHeader *header = static_cast<CT_LASHeader*>(internalReadHeader(filepath(), error));

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
        CT_LASHeader *header_temp = static_cast<CT_LASHeader*>(internalReadHeader(fileBuffer.filename, error));

        qint32 x, y, z;
        CT_Point pAdded;
        quint16 valueUint16;
        quint8 valueUint8;
        qint8 valueInt8;
        PointCore6_10 valuePointCore6_10;
        CT_Color colRGBA;
        quint16 colR;
        quint16 colG;
        quint16 colB;

        QFile f(fileBuffer.filename);
        f.open(QIODevice::ReadOnly);
        f.seek(header_temp->m_offsetToPointData);
        qint64 head_size = f.pos();

        QDataStream stream(&f);
        stream.setByteOrder(QDataStream::LittleEndian);

        for(const qint64 &i: fileBuffer.indexList)
        {
            // Seek to wanted position
            f.seek(head_size + i*header_temp->m_pointDataRecordLength);

            // READ ALL ATTRIBUTES
            stream >> x >> y >> z;
            intensitySetter.setValueWithStreamWithLocalIndex(k, stream);

            if (header_temp->m_pointDataRecordFormat < 6)
            {
                stream >> valueUint8;
                valuePointCore6_10.entire = valueUint8;
                core6_10Setter.setValueWithLocalIndex(k, valuePointCore6_10);
            }
            else
            {
                stream >> valueUint16;
                valuePointCore6_10.entire = valueUint16;
                core6_10Setter.setValueWithLocalIndex(k, valuePointCore6_10);
            }

            classificationSetter.setValueWithStreamWithLocalIndex(k, stream);

            if(header_temp->m_pointDataRecordFormat < 6)
            {
                stream >> valueInt8;
                scanAngleSetter.setValueWithLocalIndex(k, valueInt8);
                userDataSetter.setValueWithStreamWithLocalIndex(k, stream);
            }
            else
            {
                // "user data" is before "scan angle" in version 6 to 10
                userDataSetter.setValueWithStreamWithLocalIndex(k, stream);
                scanAngleSetter.setValueWithStreamWithLocalIndex(k, stream);
            }

            pointSourceIDSetter.setValueWithStreamWithLocalIndex(k, stream);

            // gps time is always after pointSourceID
            if(gpsTimeSetter)
                gpsTimeSetter->setValueWithStreamWithLocalIndex(k, stream);

            // color is always after gpsTime if exist otherwise after pointSourceID
            if(colorSetter)
            {
                stream >> colR >> colG >> colB;

                redSetter->setValueWithLocalIndex(k, colR);
                greenSetter->setValueWithLocalIndex(k, colG);
                blueSetter->setValueWithLocalIndex(k, colB);

                colRGBA.r() = colR/256;
                colRGBA.g() = colG/256;
                colRGBA.b() = colB/256;
                colRGBA.a() = 255;

                colorSetter->setValueWithLocalIndex(k, colRGBA);
            }

            // NIR is always after colors if exist
            if(nirSetter)
                nirSetter->setValueWithStreamWithLocalIndex(k, stream);

            // wave packet is always after NIR if exist
            if(wpdiSetter)
            {
                wpdiSetter->setValueWithStreamWithLocalIndex(k, stream);
                botwdSetter->setValueWithStreamWithLocalIndex(k, stream);
                wpsibSetter->setValueWithStreamWithLocalIndex(k, stream);
                rpwlSetter->setValueWithStreamWithLocalIndex(k, stream);
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

        f.close();

        delete header_temp;
    }

    // Finalize attribute assignment between PCIR and values
    CT_Scene *scene = new CT_Scene(pcir);
    scene->updateBoundingBox();

    group->addSingularItem(m_hOutScene, scene);

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

    ok = true;

    delete header;

    return ok;
}



void CT_Reader_LASV2::getPointIndicesInside2DShape(QList<CandidateShape2D> &candidateShapes) const
{
    QString error;
    CT_LASHeader *header = static_cast<CT_LASHeader*>(internalReadHeader(filepath(), error));

    if(header == nullptr) {
        PS_LOG->addErrorMessage(LogInterface::reader, tr("Impossible de lire l'en-tête du fichier %1").arg(filepath()));
        return;
    }

    qint64 nPoints = qint64(header->getPointsRecordCount());

    if(nPoints == 0) {
        PS_LOG->addWarningMessage(LogInterface::reader, tr("Aucun points contenu dans le fichier %1").arg(filepath()));
        delete header;
        return;
    }

    QFile f(filepath());

    for (CandidateShape2D& sh : candidateShapes)
    {
        sh._all = true;
    }

    if(f.open(QIODevice::ReadOnly))
    {
        bool mustTransformPoint = header->mustTransformPoints();

        f.seek(header->m_offsetToPointData);

        QDataStream stream(&f);
        stream.setByteOrder(QDataStream::LittleEndian);

        qint64 pos = f.pos();

        qint32 x, y, z;
        double xc = 0;
        double yc = 0;
        double zc = 0;

        for (qint64 i = 0; i < nPoints; ++i)
        {
            // READ ALL ATTRIBUTES
            stream >> x >> y >> z;

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

            pos += header->m_pointDataRecordLength;
            f.seek(pos);
        }

        f.close();
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

QString CT_Reader_LASV2::getFormatCode() const
{
    return "LAS";
}
