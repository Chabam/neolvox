#ifndef LVOX_GRID3DEXPORTER_H
#define LVOX_GRID3DEXPORTER_H

#include "ct_exporter/abstract/ct_abstractexporter.h"

class LVOX_Grid3DExporter : public CT_AbstractExporter
{
    Q_OBJECT
public:
    LVOX_Grid3DExporter();
    ~LVOX_Grid3DExporter();

    virtual QString getExporterCustomName() const;

    CT_StepsMenu::LevelPredefined getExporterSubMenuName() const;

    void init();

    bool setItemDrawableToExport(const QList<CT_AbstractItemDrawable*> &list);

    bool configureExport();

    virtual CT_AbstractExporter* copy() const;

protected:

    bool protectedExportToFile();
};

#endif // LVOX_GRID3DEXPORTER_H
