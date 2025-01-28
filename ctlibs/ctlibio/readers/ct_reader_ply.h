#ifndef CT_READER_PLY_H
#define CT_READER_PLY_H

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"


#include "ctlibio/ctlibio_global.h"

#include "ct_itemdrawable/ct_scene.h"

#include "ctlibio/readers/tools/ply/plyreadconfiguration.h"
#include "ctlibio/readers/tools/ply/iplyreaderlistener.h"

/**
 * @brief Reader that can load a ply file (*.ply) that represent a Mesh or a PointCloud
 */
class CTLIBIO_EXPORT CT_Reader_PLY : public CT_AbstractReader, public IPlyReaderListener, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_PLY(int subMenuLevel = 0);
    CT_Reader_PLY(const CT_Reader_PLY& other);
    ~CT_Reader_PLY() override;

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

    /**
     * @brief Show a dialog to configure this reader
     */
    bool configure() override;

    /**
     * @brief Set a configuration
     */
    bool setConfiguration(const PlyReadConfiguration& config);

    /**
     * @brief Returns the configuration
     */
    PlyReadConfiguration getConfiguration() const;

    /**
     * @brief Returns an empty file header (just to know the class type)
     */
    CT_FileHeader* createHeaderPrototype() const override;

    void saveSettings(SettingsWriterInterface& writer) const override;
    bool restoreSettings(SettingsReaderInterface& reader) override;


    READER_ALL_COPY_IMP(CT_Reader_PLY)

    /**
     * @brief This method must returns true if the read must be stopped
     */
    bool plyReadMustStop() const override;

    /**
     * @brief This method is called when the progress changed
     */
    void plyProgressChanged(int progress) override;

private:
    PlyReadConfiguration    m_config;

    CT_HandleOutSingularItem<CT_Scene>                          m_outScene;

    QVector<CT_HandleOutPointColorWithDenseManager* >           m_outColorVector;
    QVector<CT_HandleOutPointNormalWithDenseManager* >          m_outNormalVector;
    QVector<CT_HandleOutPointScalarWithDenseManager<float>* >   m_outScalarVector;


protected:

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    CT_FileHeader* internalReadHeader(const QString &filepath, QString &error) const override;
    bool internalReadFile(CT_StandardItemGroup* group) override;

};

#endif // CT_READER_PLY_H
