#ifndef CT_READER_LASV2_H
#define CT_READER_LASV2_H

#include "ct_reader/abstract/ct_abstractreader.h"

#include "ct_reader/extensions/ct_indexablepointsreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

#include "ct_shape2ddata/ct_box2ddata.h"

#include "ctliblas/ctliblas_global.h"
#include "ctliblas/readers/headers/ct_lasheader.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"


/**
 * @brief Read LAS File (http://www.asprs.org/Committee-General/LASer-LAS-File-Format-Exchange-Activities.html)
 */
class CTLIBLAS_EXPORT CT_Reader_LASV2 : public CT_AbstractReader, public CT_IndexablePointsReader, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:

    struct PointCore0_5
    { 
        quint8  entire; // Edge of Flight Line (1 bit) - Scan Direction Flag (1 bit) - Number of Returns (3 bits) - Return Number (3 bits)

        typedef quint8 MASK;
    };

    struct PointCore6_10
    {
        quint16 entire; // Number of Returns (4 bits) - Return Number (4 bits)
                        // Edge of Flight Line (1 bit) - Scan Direction Flag (1 bit) - Scanner Channel (2 bits) - Classification Flags (4 bits)

        typedef quint16 MASK;
    };

    CT_Reader_LASV2(int subMenuLevel = 0);
    CT_Reader_LASV2(const CT_Reader_LASV2 &other);
    virtual ~CT_Reader_LASV2() override;

    QString displayableName() const override;

    bool setFilePath(const QString& filepath) override;

    bool configure() override;
    CT_FileHeader* createHeaderPrototype() const override;

    virtual bool hasBoundingBox() {return true;}

    void getPointIndicesInside2DShape(QList<CandidateShape2D> &candidateShapes) const override;

    QString getFormatCode() const override;

    bool restoreSettings(SettingsReaderInterface &reader) override;

    READER_ALL_COPY_IMP(CT_Reader_LASV2)

protected:

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    CT_FileHeader* internalReadHeader(const QString &filepath, QString &error) const override;

    bool internalReadFile(CT_StandardItemGroup* group) override;

    bool internalReadMultiFile(CT_StandardItemGroup* group) override;

private:
    CT_LASHeader*                                                   m_headerFromConfiguration;

    CT_HandleOutSingularItem<CT_Scene>                              m_hOutScene;
    CT_HandleOutSingularItem<CT_StdLASPointsAttributesContainer>    m_hOutAllAtt;

    CT_DensePointScalarManager<PointCore6_10>                       mCore6_10Manager; // only one manager for all attributes that use PointCore6_10 because only one cloud to optimize memory
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutReturnNumber;
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutNumberOfReturn;
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutClassificationFlag;
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutScannerChannel;
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutScanDirectionFlag;
    CT_HandleOutPointScalarMask<PointCore6_10>                      m_hOutEdgeOfFlightLine;

    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutIntensity;
    CT_HandleOutPointScalarWithDenseManager<quint8>                 m_hOutClassification;
    CT_HandleOutPointScalarWithDenseManager<quint8>                 m_hOutUserData;
    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutPointSourceID;

    CT_HandleOutPointScalarWithDenseManager<qint16>                 m_hOutScanAngle;

    CT_HandleOutPointScalarWithDenseManager<double>                 m_hOutGPSTime;

    CT_HandleOutPointColorWithDenseManager                          m_hOutColor;
    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutRed;
    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutGreen;
    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutBlue;

    CT_HandleOutPointScalarWithDenseManager<quint8>                 m_hOutWavePacketDescriptorIndex;
    CT_HandleOutPointScalarWithDenseManager<quint64>                m_hOutByteOffsetToWaveformData;
    CT_HandleOutPointScalarWithDenseManager<quint32>                m_hOutWaveformPacketSizeInBytes;
    CT_HandleOutPointScalarWithDenseManager<float>                  m_hOutReturnPointWaveformLocation;

    CT_HandleOutPointScalarWithDenseManager<quint16>                m_hOutNIR;

};

CT_TYPEINFO_MACRO(CT_Reader_LASV2::PointCore0_5, CT_Reader_LASV2::PointCore0_5)
CT_TYPEINFO_MACRO(CT_Reader_LASV2::PointCore6_10, CT_Reader_LASV2::PointCore6_10)

#endif // CT_READER_LASV2_H
