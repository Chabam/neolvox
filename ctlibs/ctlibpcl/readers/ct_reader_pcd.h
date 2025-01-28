#ifndef CT_READER_PCD_H
#define CT_READER_PCD_H

#include "ctlibpcl/ctlibpcl_global.h"

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

#include "ct_itemdrawable/ct_scene.h"

class CTLIBPCL_EXPORT CT_Reader_PCD : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_PCD(int subMenuLevel = 0);
    CT_Reader_PCD(const CT_Reader_PCD& other);

    QString displayableName() const override;

    READER_ALL_COPY_IMP(CT_Reader_PCD)

private:
    CT_HandleOutSingularItem<CT_Scene>          m_hOutScene;


protected:
    bool preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const override;
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;


};

#endif // CT_READER_PCD_H
