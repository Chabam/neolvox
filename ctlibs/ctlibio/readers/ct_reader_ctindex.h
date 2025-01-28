#ifndef CT_READER_CTINDEX_H
#define CT_READER_CTINDEX_H

#include "ctlibio/ctlibio_global.h"
#include "ct_reader/abstract/ct_abstractreader.h"

#include "ct_itemdrawable/ct_box2d.h"
#include "ct_itemdrawable/ct_circle2d.h"
#include "ct_itemdrawable/ct_polygon2d.h"

/**
 * @brief Reader that can load a cti file (*.cti) that represent a PointCloud index list
 */
class CTLIBIO_EXPORT CT_Reader_CTIndex : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_CTIndex(int subMenuLevel = 0);
    CT_Reader_CTIndex(const CT_Reader_CTIndex& other);
    ~CT_Reader_CTIndex();

    /**
     * @brief Returns a displayable name of the reader
     */
    QString displayableName() const override;

    /**
     * @brief Redefined to check the validity of the file. Returns false if the file is not compatible.
     */
    bool setFilePath(const QString& filepath) override;

    READER_ALL_COPY_IMP(CT_Reader_CTIndex)

    virtual CT_FileHeader *createHeaderPrototype() const override;

    bool restoreSettings(SettingsReaderInterface &reader) override;

protected:
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    CT_FileHeader* internalReadHeader(const QString &filepath, QString &error) const override;

    bool internalReadFile(CT_StandardItemGroup* group) override;

private:
    QString            m_fileType;
    QList<FileBuffer>  m_fileBufferList;

    CT_AbstractReader* m_reader;

    QString            m_areaType;
    QString            m_areaSmallType;

    CT_HandleOutSingularItem<CT_Box2D>       m_outBox2D;
    CT_HandleOutSingularItem<CT_Circle2D>    m_outCircle2D;
    CT_HandleOutSingularItem<CT_Polygon2D>   m_outPolygon2D;

    CT_HandleOutSingularItem<CT_Box2D>       m_outBox2DSmall;
    CT_HandleOutSingularItem<CT_Circle2D>    m_outCircle2DSmall;
    CT_HandleOutSingularItem<CT_Polygon2D>   m_outPolygon2DSmall;

};

#endif // CT_READER_CTINDEX_H
