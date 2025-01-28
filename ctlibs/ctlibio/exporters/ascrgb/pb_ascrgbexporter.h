#ifndef PB_ASCRGBEXPORTER_H
#define PB_ASCRGBEXPORTER_H

#include "ctlibio/ctlibio_global.h"

#include "ct_exporter/abstract/ct_abstractexporter.h"
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithpointcloud.h"
#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"
#include "ct_itemdrawable/ct_pointsattributesnormal.h"

class CTLIBIO_EXPORT PB_ASCRGBExporter : public CT_AbstractExporter
{
    Q_OBJECT
    using SuperClass = CT_AbstractExporter;

public:
    PB_ASCRGBExporter(int subMenuLevel = 0);
    PB_ASCRGBExporter(const PB_ASCRGBExporter& other);

    QString displayableName() const final;

    void setItemsToExport(const QList<const CT_AbstractItemDrawableWithPointCloud*>& items,
                          const CT_AbstractPointsAttributes* pointsAttributeOfTypeColorOrScalar,
                          const CT_PointsAttributesNormal* normalsAttributes = nullptr);

    CT_EXPORTER_DECL_COPY(PB_ASCRGBExporter)

protected:
    void internalDeclareInputModels(CT_ExporterInModelStructureManager& manager) override;

    ExportReturn internalExportToFile() override;

    void clearIterators() override;

    void clearAttributesClouds() override;

private:
    using HandleItemType = CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>;

    CT_HandleInStdGroup<>                                           m_hInGroup;
    HandleItemType                                                  m_hInItem;
    CT_HandleInAbstractPointAttribute<>                             m_hInColorsAttribute;
    CT_HandleInPointNormal<0>                                       m_hInNormalsAttribute; // optionnal

    HandleItemType::const_iterator                                  mIteratorItemBegin;
    HandleItemType::const_iterator                                  mIteratorItemEnd;

    QList<const CT_AbstractItemDrawableWithPointCloud*>             mItems;
    const CT_AbstractPointsAttributes*                              mPointsAttributeOfTypeColorOrScalar;
    const CT_PointsAttributesNormal*                                mPointsNormal;

    void exportItem(const CT_AbstractItemDrawableWithPointCloud* item,
                    QTextStream& txtStream,
                    std::function<void (const size_t&, quint16&, quint16&, quint16&)> colorFunc,
                    const int& nExported,
                    const int& totalToExport);
};

#endif // PB_ASCRGBEXPORTER_H
