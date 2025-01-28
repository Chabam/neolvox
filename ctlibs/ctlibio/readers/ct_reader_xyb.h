#ifndef CT_READER_XYB_H
#define CT_READER_XYB_H

#include "ctlibio/ctlibio_global.h"

#include "headers/ct_xybheader.h"

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanner.h"

/**
 * @brief Reader that can load a xyb file (*.xyb) that represent a PointCloud
 */
class CTLIBIO_EXPORT CT_Reader_XYB : public CT_AbstractReader, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_XYB(int subMenuLevel = 0);
    CT_Reader_XYB(const CT_Reader_XYB& other);

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

    /**
     * @brief Redefined to check the validity of the file. Returns false if the file is not compatible.
     */
    bool setFilePath(const QString& filepath) override;

    /**
     * @brief Redefined to create a CT_XYBHeader
     */
    CT_FileHeader* createHeaderPrototype() const override;

    /**
     * @brief Set a radius if you want to filter points
     */
    void setFilterRadius(const double& radius);

    /**
     * @brief Set a radius and a z area if you want to filter points
     */
    void setFilterRadius(const double& radius, const double& zmin, const double& zmax);

    /**
     * @brief Returns the filter radius
     */
    double filterRadius() const;

    /**
     * @brief Returns the filter minimum z
     */
    double filterZMin() const;

    /**
     * @brief Returns the filter maximum z
     */
    double filterZMax() const;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;

    READER_ALL_COPY_IMP(CT_Reader_XYB)

protected:
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    CT_FileHeader* internalReadHeader(const QString &filepath, QString &error) const override;

    bool internalReadFile(CT_StandardItemGroup* group) override;

private:
    struct Params {
        int                 _offset;
        Eigen::Vector3d     m_center;
        int                 _rows;
        int                 _cols;
    };

    Params      m_current;
    double      m_filterRadius;
    double      m_zMinFilter;
    double      m_zMaxFilter;

    CT_HandleOutSingularItem<CT_Scene>                  m_hOutScene;
    CT_HandleOutPointScalarWithDenseManager<quint16>    m_hOutIntensity;
    CT_HandleOutSingularItem<CT_Scanner>                m_hOutScanner;

    /**
     * @brief Returns true if the point is filtered (not accepted). Redefined
     *        to add radius, zmin and zmax filters
     */
    bool isPointFiltered(const CT_Point& point) const override;

    CT_NMPCIR internalReadFileWithFilter(QDataStream& stream,
                                         double& xmin,
                                         double& ymin,
                                         double& zmin,
                                         quint16& imin,
                                         double& xmax,
                                         double& ymax,
                                         double& zmax,
                                         quint16& imax,
                                         const qint64& nPointToRead);

    CT_NMPCIR internalReadFileWithoutFilter(QDataStream& stream,
                                            double& xmin,
                                            double& ymin,
                                            double& zmin,
                                            quint16& imin,
                                            double& xmax,
                                            double& ymax,
                                            double& zmax,
                                            quint16& imax,
                                            const qint64& nPointToRead);
};

#endif // CT_READER_XYB_H
