#ifndef CT_READER_POINTS_ASCII_H
#define CT_READER_POINTS_ASCII_H

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"
#include "ct_itemdrawable/ct_scene.h"

#include "ctlibio/ctlibio_global.h"

/**
 * @brief Reader that can load a ascii file (*.asc *.xyz *.csv etc...) that contains a point cloud and/or normals and/or colors and/or intensity
 */
class CTLIBIO_EXPORT CT_Reader_Points_ASCII : public CT_AbstractReader, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_Points_ASCII(int subMenuLevel = 0);
    CT_Reader_Points_ASCII(const CT_Reader_Points_ASCII& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

    /**
     * @brief Show a dialog to configure this reader
     */
    bool configure() override;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    void setXColumnIndex(int index);
    void setYColumnIndex(int index);
    void setZColumnIndex(int index);

    void setIntensityColumnIndex(int index);

    void setRedColumnIndex(int index);
    void setGreenColumnIndex(int index);
    void setBlueColumnIndex(int index);

    void setNxColumnIndex(int index);
    void setNyColumnIndex(int index);
    void setNzColumnIndex(int index);
    void setNCurvatureIndex(int index);

    void setFirstConfiguration(bool first);

    void setLinesToSkip(int skip);
    void setHasHeader(bool hasHeader);
    void setValueSeparator(QString sep);
    void setLocaleName(QString locale);

    int xColumnIndex() const;
    int yColumnIndex() const;
    int zColumnIndex() const;

    int intensityColumnIndex() const;

    int redColumnIndex() const;
    int greenColumnIndex() const;
    int blueColumnIndex() const;

    int nXColumnIndex() const;
    int nYColumnIndex() const;
    int nZColumnIndex() const;
    int nCurvatureIndex() const;

    bool canLoadPoints() const;
    bool canLoadIntensity() const;
    bool canLoadColors() const;
    bool canLoadNormals() const;

    READER_ALL_COPY_IMP(CT_Reader_Points_ASCII)

private:
    bool    m_firstConfiguration;
    int     m_columnXIndex;
    int     m_columnYIndex;
    int     m_columnZIndex;
    int     m_columnIntensityIndex;
    int     m_columnRedIndex;
    int     m_columnGreenIndex;
    int     m_columnBlueIndex;
    int     m_columnNxIndex;
    int     m_columnNyIndex;
    int     m_columnNzIndex;
    int     m_columnNCurvatureIndex;
    int     m_nLinesToSkip;
    bool    m_hasHeader;
    QString m_separator;
    QString m_localeName;


    CT_HandleOutSingularItem<CT_Scene>                          m_outPointCloud;
    CT_HandleOutPointScalarWithDenseManager<float>              m_outIntensity;
    CT_HandleOutPointColorWithDenseManager                      m_outColors;
    CT_HandleOutPointNormalWithDenseManager                     m_outNormals;


protected:
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;


    CT_AbstractUndefinedSizePointCloud* createPointCloud() const;
    CT_DensePointScalarManager<float>::UPCSSetterPtr createIntensityArray(CT_AbstractUndefinedSizePointCloud* uspc);
    CT_DensePointColorManager::UPCSSetterPtr createColorsArray(CT_AbstractUndefinedSizePointCloud* uspc);
    CT_DensePointNormalManager::UPCSSetterPtr createNormalsArray(CT_AbstractUndefinedSizePointCloud* uspc);

    int maxColumnIndex() const;

    void initBoundinBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const;
    void skipLines(QTextStream &stream, qint64 &currentSizeRead) const;

    void updateBoundingBox(const CT_Point &point, Eigen::Vector3d &bboxMin, Eigen::Vector3d &bboxMax) const;

    bool readPoint(const QStringList &wordsOfLine, const QLocale &locale, CT_Point &point) const;
    void addPoint(const CT_Point &point, CT_AbstractUndefinedSizePointCloud *array, Eigen::Vector3d &minBB, Eigen::Vector3d &maxBB) const;
    bool readAndAddIntensity(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointScalarManager<float>::UPCSSetterPtr& setter, float &minI, float &maxI) const;
    bool readAndAddColor(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointColorManager::UPCSSetterPtr& setter) const;
    bool readAndAddNormal(const QStringList &wordsOfLine, const QLocale &locale, CT_DensePointNormalManager::UPCSSetterPtr& setter) const;
};

#endif // CT_READER_POINTS_ASCII_H
