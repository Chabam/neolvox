#ifndef PB_MULTIXYBEXPORTER_H
#define PB_MULTIXYBEXPORTER_H

#include "ct_exporter/abstract/ct_abstractexporterpointattributesselection.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_view/itemdrawable/ct_itemdrawablehierarchycollectionwidget.h"
#include "ct_tools/attributes/ct_attributestocloudworkert.h"
#include "ct_itemdrawable/abstract/ct_abstractpointsattributes.h"

class PB_MultiXYBExporter : public CT_AbstractExporterPointAttributesSelection
{
    Q_OBJECT
    typedef CT_AbstractExporter SuperClass;

public:
    PB_MultiXYBExporter();
    PB_MultiXYBExporter(const PB_MultiXYBExporter& other) = default;

    void init() override;

    QString getExporterCustomName() const override;

    CT_StepsMenu::LevelPredefined getExporterSubMenuName() const override;

    bool setItemDrawableToExport(const QList<CT_AbstractItemDrawable*> &list) override;

    CT_EXPORTER_DECL_COPY(PB_MultiXYBExporter)

protected:
    bool protectedExportToFile() override;

private:
    void exportPoints(QDataStream &stream,
                      const CT_AbstractPointCloudIndex *constPCIndex,
                      const CT_AbstractColorCloud *cc);
};

#endif // PB_MULTIXYBEXPORTER_H
