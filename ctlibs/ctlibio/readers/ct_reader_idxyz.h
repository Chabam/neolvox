#ifndef CT_READER_IDXYZ_H
#define CT_READER_IDXYZ_H

#include "ctlibio/ctlibio_global.h"

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

#include "ct_itemdrawable/ct_scene.h"

class CTLIBIO_EXPORT CT_Reader_IDXYZ : public CT_AbstractReader
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_IDXYZ(int subMenuLevel = 0);
    CT_Reader_IDXYZ(const CT_Reader_IDXYZ& other);

    QString displayableName() const override;

    READER_ALL_COPY_IMP(CT_Reader_IDXYZ)

private:
    CT_HandleOutSingularItem<CT_Scene>          m_hOutScene;
    CT_HandleOutStdGroup                        m_hOutSceneIdGroup;
    CT_HandleOutSingularItem<CT_Scene>          m_hOutSceneId;


protected:
    bool preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const override;
    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;
    bool internalReadFile(CT_StandardItemGroup* group) override;


};

#endif // CT_READER_IDXYZ_H
