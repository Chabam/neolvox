#ifndef CT_READER_ASCRGB_H
#define CT_READER_ASCRGB_H

#include "ctlibio/ctlibio_global.h"

#include "ct_reader/abstract/ct_abstractreader.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"
#include "ct_itemdrawable/ct_scene.h"

class CTLIBIO_EXPORT CT_Reader_ASCRGB : public CT_AbstractReader, public CT_ReaderPointsFilteringExtension
{
    Q_OBJECT
    typedef CT_AbstractReader SuperClass;

public:
    CT_Reader_ASCRGB(int subMenuLevel = 0);
    CT_Reader_ASCRGB(const CT_Reader_ASCRGB& other);

    QString displayableName() const override;

    void setRadiusFilter(const double &radius);
    void setRadiusFilter(const double &radius, const double &zmin, const double &zmax);

    READER_ALL_COPY_IMP(CT_Reader_ASCRGB)

private:
    double   m_filterRadius;
    double   _zminFilter;
    double   _zmaxFilter;

    CT_HandleOutSingularItem<CT_Scene>          m_hOutScene;
    CT_HandleOutPointColorWithDenseManager      m_hOutColors;

    bool isInsideRadius(const CT_Point &point);

protected:
    bool preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const override;

    void internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager) override;

    bool internalReadFile(CT_StandardItemGroup* group) override;
};

#endif // CT_READER_ASCRGB_H
