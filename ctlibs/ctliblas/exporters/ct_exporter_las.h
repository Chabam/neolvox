#ifndef CT_EXPORTER_LAS_H
#define CT_EXPORTER_LAS_H

#include "ctliblas/ctliblas_global.h"
#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"
#include "ctliblas/tools/las/abstract/ct_abstractlaspointformat.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

class CTLIBLAS_EXPORT CT_Exporter_LAS : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    CT_Exporter_LAS(int subMenuLevel = 0);
    CT_Exporter_LAS(const CT_Exporter_LAS& other);

    QString displayableName() const final;

    bool setPointsToExport(const QList<CT_AbstractCloudIndex*> &list) override;

    void setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                          const QList<const CT_StdLASPointsAttributesContainer*>& lasAttributesContainers = QList<const CT_StdLASPointsAttributesContainer*>());

    bool canExportPieceByPiece() const final;
    CT_AbstractPieceByPieceExporter* createPieceByPieceExporter(const QString& outputFilepath) override;

    CT_EXPORTER_DECL_COPY(CT_Exporter_LAS)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    ExportReturn internalExportOnePiece(const QList<CT_AbstractPieceByPieceExporter*>& pieceByPieceExporters) override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    friend class CT_LASPieceByPiecePrivateExporter;

    using HandleItemType = CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>;
    using AttributeByTypeCollection = CT_AbstractLASPointFormat::AttributeByTypeCollection;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_hInItem;
    CT_HandleInSingularItem<CT_StdLASPointsAttributesContainer, 0>  m_hInLASAttributesContainer; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInReturnNumber; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInNumberOfReturn; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInClassificationFlag; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInScannerChannel; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInScanDirectionFlag; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInEdgeOfFlightLine; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInIntensity; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInClassification; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInUserData; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInPointSourceID; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInScanAngle; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInGPSTime; // optionnal

    CT_HandleInPointColor<0>                                        m_hInColor; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInRed; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInGreen; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInBlue; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInWavePacketDescriptorIndex; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInByteOffsetToWaveformData; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInWaveformPacketSizeInBytes; // optionnal
    CT_HandleInAbstractPointScalar<0>                               m_hInReturnPointWaveformLocation; // optionnal

    CT_HandleInAbstractPointScalar<0>                               m_hInNIR; // optionnal

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_AbstractItemDrawableWithPointCloud*>             mItems;

    AttributeByTypeCollection                                       mLasAttributeByType;
    const CT_PointsAttributesColor*                                 mColorAttribute;
    QSharedPointer<CT_AbstractLASPointFormat>                       mToolsFormat;

    double                                                          mCurrentPieceByPieceProgress;
    double                                                          mPieceByPieceProgressMultiplicator;

    void findAttributeInContainerAndAddItToCollection(CT_LasDefine::LASPointAttributesType key, const CT_StdLASPointsAttributesContainer* container, AttributeByTypeCollection& allAttributes) const
    {
        CT_AbstractPointAttributesScalar* attribute = static_cast<CT_AbstractPointAttributesScalar*>(container->pointsAttributesAt(key));

        if(attribute != nullptr)
            allAttributes.insert(key, attribute);
    }

    template<typename HandleType>
    void findAttributeWithHandleAndAddItToCollection(CT_LasDefine::LASPointAttributesType key, HandleType& t, AttributeByTypeCollection& allAttributes) const
    {
        for(const CT_AbstractPointAttributesScalar* attribute : t.iterateInputs(m_handleResultExport)) {
            allAttributes.insert(key, attribute);
            // only use the first attribute because all other that use the same model use the same manager of attributes so there is only one cloud !
            break;
        }
    }

    void findAttributesInContainerAndAddItToCollection(const CT_StdLASPointsAttributesContainer* container);

    /**
     * @brief Return the point data format to use (check mLasContainer to know where attributes is in the container and select the appropriate format)
     */
    quint8 getPointDataFormat() const;

    /**
     * @brief Return the length of a point in bytes (use the method "getPointDataFormat" if optFormat == -1, otherwise pass the format to this method)
     */
    quint16 getPointDataLength(const int &optFormat = -1) const;

    /**
     * @brief Create a point data format (tools to export)
     */
    CT_AbstractLASPointFormat* createPointDataFormat(const int &optFormat = -1) const;

private slots:
    void setPieceByPieceProgress(int progress);
};

#endif // CT_EXPORTER_LAS_H
