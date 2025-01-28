#ifndef CT_READER_PTX_H
#define CT_READER_PTX_H

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_transformationmatrix.h"

#include "ctlibio/ctlibio_global.h"

/**
 * @brief Reader that can load a ptx file (*.ptx) that represent a PointCloud
 */
class CTLIBIO_EXPORT CT_Reader_PTX : public CT_AbstractReader, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_PTX(int subMenuLevel = 0);
    CT_Reader_PTX(const CT_Reader_PTX& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;


    /**
     * @brief Set the filepath to the reader. The file will be opened to check is validity.
     */
    bool setFilePath(const QString &filepath) override;

    /**
     * @brief Configure the reader
     */
    bool configure() override;

    void setApplyTransformationMatrix(bool enable);
    bool applyTransformationMatrix() const;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    READER_ALL_COPY_IMP(CT_Reader_PTX)

private:
    bool    m_applyTransformation;

    CT_HandleOutSingularItem<CT_Scene>                                      m_outScene;
    CT_HandleOutPointScalarWithDenseManager<double>                         m_outIntensity;
    CT_HandleOutPointColorWithDenseManager                                  m_outRGB;
    CT_HandleOutSingularItem<CT_TransformationMatrix>                       m_outMatrix;
    CT_HandleOutSingularItem<CT_Scanner>                                    m_outScanner;

    bool readHeaderValues(QTextStream &stream, int &nColumn, int &nRow, Eigen::Matrix4d &matrix, bool &hasColors) const;

protected:

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;

};

#endif // CT_READER_PTX_H
