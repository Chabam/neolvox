#ifndef LVOX3_SCANNERUTILS_H
#define LVOX3_SCANNERUTILS_H

#include <QVector>
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"
#include "tools/lvox3_factory.hpp"

/*
 * Scanner definitions
 * TODO: move to pluginshared
 */

#define LVOX_SP_SPHERE_THEORETIC_SCANNER "SphericTheroreticScanner"
#define LVOX_SP_SPHERE_THEORETIC_CUSTOM "SphericTheroreticCustom"
#define LVOX_SP_SPHERE_POINTCLOUD "SphericPointCloud"
#define LVOX_SP_PLANE_POINTCLOUD "PlanePointCloud"

enum ScannerTypeEnum {
    ScannerSphericPointCloud = 0,
    ScannerPlanePointCloud,
    ScannerSphericTheoreticScanner,
    ScannerSphericTheoreticCustom

};

/*
 * TODO: implement following geometries for completeness:
 *   - CYLINDRIC_THERORETIC
 *   - CYLINDRIC_POINTCLOUD
 *   - PLANE_THEORETIC
 */

struct ScannerDefinitionStruct {
    ScannerTypeEnum id;
    QString label;
    QString name;
    QString description;
};

class LVOX3_ScannerUtils
{
public:
    LVOX3_ScannerUtils();
    static const QVector<ScannerDefinitionStruct> &getScannerDefinitions();
    static const ScannerDefinitionStruct &getScannerDefinition(ScannerTypeEnum id);
    static const ScannerDefinitionStruct &getScannerDefinition(const QString& label);
    static bool isCustomScannerConfiguration(ScannerTypeEnum id);

private:
    Factory<CT_ShootingPattern> f;
    static const QVector<ScannerDefinitionStruct> m_scannerDefinitions;
};

#endif // LVOX3_SCANNERUTILS_H
