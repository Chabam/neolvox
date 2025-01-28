#ifndef CT_READER_TERRASCANPRJ_H
#define CT_READER_TERRASCANPRJ_H

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_itemdrawable/ct_box2d.h"
#include "ct_itemdrawable/ct_fileheader.h"

#include "ctlibio/ctlibio_global.h"

class CTLIBIO_EXPORT CT_Reader_TerraScanPrj : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_TerraScanPrj(int subMenuLevel = 0);
    CT_Reader_TerraScanPrj(const CT_Reader_TerraScanPrj& other);

    QString displayableName() const override;

    bool setFilePath(const QString &filepath) override;

    READER_ALL_COPY_IMP(CT_Reader_TerraScanPrj)

protected:
        void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
        bool internalReadFile(CT_StandardItemGroup* group) override;

private:
        CT_HandleOutSingularItem<CT_Box2D>          m_box;
        CT_HandleOutSingularItem<CT_FileHeader>     m_fileName;
};

#endif // CT_READER_TERRASCANPRJ_H
