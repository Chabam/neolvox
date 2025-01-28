#include "ct_exporter_las.h"

#include "private/ct_laspiecebypieceprivateexporter.h"

#include "ctliblas/tools/las/ct_laspointformat0.h"
#include "ctliblas/tools/las/ct_laspointformat1.h"
#include "ctliblas/tools/las/ct_laspointformat2.h"
#include "ctliblas/tools/las/ct_laspointformat3.h"
#include "ctliblas/tools/las/ct_laspointformat4.h"
#include "ctliblas/tools/las/ct_laspointformat5.h"
#include "ctliblas/tools/las/ct_laspointformat6.h"
#include "ctliblas/tools/las/ct_laspointformat7.h"
#include "ctliblas/tools/las/ct_laspointformat8.h"
#include "ctliblas/tools/las/ct_laspointformat9.h"
#include "ctliblas/tools/las/ct_laspointformat10.h"

CT_Exporter_LAS::CT_Exporter_LAS(int subMenuLevel) : SuperClass(subMenuLevel),
    mColorAttribute(nullptr),
    mToolsFormat(nullptr)
{
    setCanExportPoints(true);
    addNewExportFormat(FileFormat("las", tr("Fichiers LAS .las")));
    setToolTip(tr("Exporte tous les points au format LAS, ASPRS<br>http://www.asprs.org/Committee-General/LASer-LAS-File-Format-Exchange-Activities.html"));
}

CT_Exporter_LAS::CT_Exporter_LAS(const CT_Exporter_LAS &other) : SuperClass(other)
{
}

QString CT_Exporter_LAS::displayableName() const
{
    return tr("Points, LAS");
}

bool CT_Exporter_LAS::setPointsToExport(const QList<CT_AbstractCloudIndex*> &list)
{
    QList<CT_AbstractCloudIndex*> myList;
    QListIterator<CT_AbstractCloudIndex*> it(list);

    while(it.hasNext())
    {
        CT_AbstractCloudIndex *item = it.next();

        if(dynamic_cast<CT_AbstractPointCloudIndex*>(item) != nullptr)
            myList.append(item);
    }

    if(myList.isEmpty())
    {
        setErrorMessage(errorMessage() + "\r\n" + tr("- Aucun points à exporter"));
        return false;
    }

    return SuperClass::setPointsToExport(myList);
}

void CT_Exporter_LAS::setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                                       const QList<const CT_StdLASPointsAttributesContainer*>& lasAttributesContainers)
{
    setMustUseModels(false);
    clearIterators();
    clearAttributesClouds();
    mItems = items;

    for(const CT_StdLASPointsAttributesContainer* container : lasAttributesContainers)
    {
        findAttributesInContainerAndAddItToCollection(container);
    }
}

bool CT_Exporter_LAS::canExportPieceByPiece() const
{
    return true;
}

CT_AbstractPieceByPieceExporter* CT_Exporter_LAS::createPieceByPieceExporter(const QString& outputFilepath)
{
    return new CT_LASPieceByPiecePrivateExporter(*this, outputFilepath);
}

void CT_Exporter_LAS::internalDeclareInputModels(CT_ExporterInModelStructureManager& manager)
{
    manager.addGroupToRootGroup(m_hInGroup);
    manager.addItemToGroup(m_hInGroup, m_hInItem, tr("Item à exporter"));
    manager.addItemToGroup(m_hInGroup, m_hInLASAttributesContainer, tr("Conteneur d'attributs LAS"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInReturnNumber, tr("Return Number"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInNumberOfReturn, tr("Number of Returns"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInClassificationFlag, tr("Classification Flags"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInScannerChannel, tr("Scanner Channel"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInScanDirectionFlag, tr("Scan Direction Flag"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInEdgeOfFlightLine, tr("Edge of Flight Line"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInIntensity, tr("Intensity"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInClassification, tr("Classification"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInUserData, tr("User Data"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInPointSourceID, tr("Point Source "));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInScanAngle, tr("Scan Angle"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInGPSTime, tr("GPS Time"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInRed, tr("Red"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInGreen, tr("Green"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInBlue, tr("Blue"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInWavePacketDescriptorIndex, tr("Wave Packet Descriptor Index"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInByteOffsetToWaveformData, tr("Byte Offset To Waveform Data"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInWaveformPacketSizeInBytes, tr("Waveform Packet Size In Bytes"));
    manager.addPointAttributeToGroup(m_hInGroup, m_hInReturnPointWaveformLocation, tr("Return Point Waveform Location"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInNIR, tr("NIR"));

    manager.addPointAttributeToGroup(m_hInGroup, m_hInColor, tr("Color"));
}

CT_AbstractExporter::ExportReturn CT_Exporter_LAS::internalExportToFile()
{
    QList<CT_AbstractPieceByPieceExporter*> exporters;
    exporters << createPieceByPieceExporter(filePath());

    CT_AbstractExporter::ExportReturn ret = ErrorWhenExport;
    if((ret = exportOnePieceOfDataToFiles(exporters)) != ErrorWhenExport)
    {
        if(!finalizePieceByPieceExport(exporters))
            return ErrorWhenExport;
    }

    return ret;
}

CT_AbstractExporter::ExportReturn CT_Exporter_LAS::internalExportOnePiece(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters)
{
    if(mustUseModels())
    {
        if(mToolsFormat.isNull())
        {
            bool atLeastOneContainer = false;

            for(const CT_StdLASPointsAttributesContainer* container : m_hInLASAttributesContainer.iterateInputs(m_handleResultExport))
            {
                findAttributesInContainerAndAddItToCollection(container);
                atLeastOneContainer = true;
            }

            if(!atLeastOneContainer)
            {
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Return_Number, m_hInReturnNumber, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Number_of_Returns, m_hInNumberOfReturn, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Classification_Flags, m_hInClassificationFlag, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Scanner_Channel, m_hInScannerChannel, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Scan_Direction_Flag, m_hInScanDirectionFlag, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Edge_of_Flight_Line, m_hInEdgeOfFlightLine, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Intensity, m_hInIntensity, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Classification, m_hInClassification, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::User_Data, m_hInUserData, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Point_Source_ID, m_hInPointSourceID, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Scan_Angle_Rank, m_hInScanAngle, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::GPS_Time, m_hInGPSTime, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Red, m_hInRed, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Green, m_hInGreen, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Blue, m_hInBlue, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Wave_Packet_Descriptor_Index, m_hInWavePacketDescriptorIndex, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Byte_offset_to_waveform_data, m_hInByteOffsetToWaveformData, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Waveform_packet_size_in_bytes, m_hInWaveformPacketSizeInBytes, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::Return_Point_Waveform_Location, m_hInReturnPointWaveformLocation, mLasAttributeByType);
                findAttributeWithHandleAndAddItToCollection(CT_LasDefine::NIR, m_hInNIR, mLasAttributeByType);

                for(const CT_PointsAttributesColor* attribute : m_hInColor.iterateInputs(m_handleResultExport)) {

                    // only use the first attribute because all other that use the same model use the same manager of attributes so there is only one cloud !
                    mColorAttribute = attribute;
                    break;
                }
            }

            mToolsFormat.reset(createPointDataFormat());
        }
    }
    else if(mToolsFormat.isNull())
    {
        mToolsFormat.reset(createPointDataFormat());
    }

    QList<const CT_AbstractPointCloudIndex*> indexes;

    if(mustUseModels())
    {
        if(mIteratorItemBegin == mIteratorItemEnd)
        {
            auto iterator = m_hInItem.iterateInputs(m_handleResultExport);
            mIteratorItemBegin = iterator.begin();
            mIteratorItemEnd = iterator.end();
        }

        int nExported = 0;
        const int totalToExport = maximumItemToExportInFile(std::numeric_limits<int>::max());

        while((mIteratorItemBegin != mIteratorItemEnd)
              && (nExported < totalToExport))
        {
            const CT_AbstractItemDrawableWithPointCloud *item = *mIteratorItemBegin;

            indexes.append(item->pointCloudIndex());

            ++nExported;
            ++mIteratorItemBegin;
        }
    }
    else
    {
        for(const CT_AbstractItemDrawableWithPointCloud* item : mItems)
        {
            indexes.append(item->pointCloudIndex());
        }

        const QList<CT_AbstractCloudIndex*> &pointsSelected = pointsToExport();
        QListIterator<CT_AbstractCloudIndex*> itCI(pointsSelected);

        while(itCI.hasNext())
        {
            indexes.append(dynamic_cast<CT_AbstractPointCloudIndex*>(itCI.next()));
        }
    }

    QList<CT_LASPieceByPiecePrivateExporter*> finalPieceByPieceExporters;

    const quint8 pointDataFormat = getPointDataFormat();
    const quint16 pointDataLength = getPointDataLength(pointDataFormat);

    for(CT_AbstractPieceByPieceExporter* ex : pieceByPieceExporters)
    {
        CT_LASPieceByPiecePrivateExporter* lasEx = static_cast<CT_LASPieceByPiecePrivateExporter*>(ex);

        if(lasEx->initializeHeader(pointDataFormat, pointDataLength))
            finalPieceByPieceExporters.append(lasEx);
    }

    if(!finalPieceByPieceExporters.isEmpty())
    {
        for(const CT_AbstractPointCloudIndex* index : indexes)
        {
            CT_PointIterator itP(index);

            while(itP.hasNext())
            {
                const CT_Point& p = itP.next().currentPoint();
                const size_t globalIndex = itP.cIndex();
                auto returnNumberValues = mLasAttributeByType.value(CT_LasDefine::Return_Number);

                for(CT_LASPieceByPiecePrivateExporter* lasEx : finalPieceByPieceExporters)
                {
                    lasEx->computePointForHeader(returnNumberValues, globalIndex, p);
                }
            }
        }

        mCurrentPieceByPieceProgress = 0;
        mPieceByPieceProgressMultiplicator = 1.0 / double(finalPieceByPieceExporters.size());

        for(CT_LASPieceByPiecePrivateExporter* lasEx : finalPieceByPieceExporters)
        {
            connect(lasEx, &CT_LASPieceByPiecePrivateExporter::progressChanged, this, &CT_Exporter_LAS::setPieceByPieceProgress);
            connect(this, &CT_Exporter_LAS::mustCancel, lasEx, &CT_LASPieceByPiecePrivateExporter::cancel);

            lasEx->finalizeHeaderAndWritePoints();

            disconnect(lasEx, nullptr, this, nullptr);
            disconnect(this, nullptr, lasEx, nullptr);

            mCurrentPieceByPieceProgress += (mPieceByPieceProgressMultiplicator * 100.0);

            if(isStopped())
                break;
        }
    }

    if(mustUseModels())
        return (mIteratorItemBegin == mIteratorItemEnd) ? NoMoreItemToExport : ExportCanContinue;

    return NoMoreItemToExport;
}

void CT_Exporter_LAS::clearIterators()
{
    mIteratorItemBegin = HandleItemType::const_iterator();
    mIteratorItemEnd = mIteratorItemBegin;
}

void CT_Exporter_LAS::clearAttributesClouds()
{
    mLasAttributeByType.clear();
    mColorAttribute = nullptr;
    mToolsFormat.reset(nullptr);
}

void CT_Exporter_LAS::findAttributesInContainerAndAddItToCollection(const CT_StdLASPointsAttributesContainer* container)
{
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Return_Number, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Number_of_Returns, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Classification_Flags, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Scanner_Channel, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Scan_Direction_Flag, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Edge_of_Flight_Line, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Intensity, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Classification, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::User_Data, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Point_Source_ID, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Scan_Angle_Rank, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::GPS_Time, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Red, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Green, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Blue, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Wave_Packet_Descriptor_Index, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Byte_offset_to_waveform_data, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Waveform_packet_size_in_bytes, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::Return_Point_Waveform_Location, container, mLasAttributeByType);
    findAttributeInContainerAndAddItToCollection(CT_LasDefine::NIR, container, mLasAttributeByType);
}

quint8 CT_Exporter_LAS::getPointDataFormat() const
{
    if(mLasAttributeByType.isEmpty() && (mColorAttribute == nullptr))
        return 1;

    const QList<CT_LasDefine::LASPointAttributesType> list = mLasAttributeByType.uniqueKeys();

    bool wavePacket = true;
    bool nir = true;
    bool colors = true;

    if(!list.contains(CT_LasDefine::Wave_Packet_Descriptor_Index)
            && !list.contains(CT_LasDefine::Byte_offset_to_waveform_data)
            && !list.contains(CT_LasDefine::Waveform_packet_size_in_bytes)
            && !list.contains(CT_LasDefine::Return_Point_Waveform_Location))
        wavePacket = false;

    if(!list.contains(CT_LasDefine::NIR))
        nir = false;

    if(!list.contains(CT_LasDefine::Red)
            && !list.contains(CT_LasDefine::Green)
            && !list.contains(CT_LasDefine::Blue)
            && (mColorAttribute == nullptr))
        colors = false;

    if(wavePacket)
    {
        if(nir || colors)
            return 10;

        return 9;
    }

    if(nir)
        return 8;

    if(colors)
        return 7;

    return 1;
}

quint16 CT_Exporter_LAS::getPointDataLength(const int &optFormat) const
{
    int format = optFormat;

    if((format < 0) || (format > 10))
        format = getPointDataFormat();

    if(format == 0)
        return quint16(CT_LASPointFormat0().sizeInBytes());
    else if(format == 1)
        return quint16(CT_LASPointFormat1().sizeInBytes());
    else if(format == 2)
        return quint16(CT_LASPointFormat2().sizeInBytes());
    else if(format == 3)
        return quint16(CT_LASPointFormat3().sizeInBytes());
    else if(format == 4)
        return quint16(CT_LASPointFormat4().sizeInBytes());
    else if(format == 5)
        return quint16(CT_LASPointFormat5().sizeInBytes());
    else if(format == 6)
        return quint16(CT_LASPointFormat6().sizeInBytes());
    else if(format == 7)
        return quint16(CT_LASPointFormat7().sizeInBytes());
    else if(format == 8)
        return quint16(CT_LASPointFormat8().sizeInBytes());
    else if(format == 9)
        return quint16(CT_LASPointFormat9().sizeInBytes());
    else if(format == 10)
        return quint16(CT_LASPointFormat10().sizeInBytes());

    return 0;
}

CT_AbstractLASPointFormat* CT_Exporter_LAS::createPointDataFormat(const int &optFormat) const
{
    int format = optFormat;

    if((format < 0) || (format > 10))
        format = getPointDataFormat();

    if(format == 0)
        return new CT_LASPointFormat0();
    else if(format == 1)
        return new CT_LASPointFormat1();
    else if(format == 2)
        return new CT_LASPointFormat2();
    else if(format == 3)
        return new CT_LASPointFormat3();
    else if(format == 4)
        return new CT_LASPointFormat4();
    else if(format == 5)
        return new CT_LASPointFormat5();
    else if(format == 6)
        return new CT_LASPointFormat6();
    else if(format == 7)
        return new CT_LASPointFormat7();
    else if(format == 8)
        return new CT_LASPointFormat8();
    else if(format == 9)
        return new CT_LASPointFormat9();
    else if(format == 10)
        return new CT_LASPointFormat10();

    return nullptr;
}

void CT_Exporter_LAS::setPieceByPieceProgress(int progress)
{
    const double finalProgress = double(progress) * mPieceByPieceProgressMultiplicator;
    setExportProgress(int(finalProgress + mCurrentPieceByPieceProgress));
}
