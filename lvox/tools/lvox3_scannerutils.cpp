#include <QDebug>

//Tools
#include "lvox3_scannerutils.h"

//Drawables
#include "ct_itemdrawable/tools/scanner/ct_thetaphishootingpattern.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpatternfrompointcloud.h"
#include "ct_itemdrawable/tools/scanner/ct_parallelshootingpatternfrompointcloud.h"

const QVector<ScannerDefinitionStruct>
//Scanner types declaration
LVOX3_ScannerUtils::m_scannerDefinitions = {
    { ScannerSphericPointCloud, LVOX_SP_SPHERE_POINTCLOUD, QObject::tr("Real shot - sherical"), QObject::tr("The position of the scanner represents the origin of the shots. The direction of a shot is oriented towards a point of the cloud. This for each point of the cloud.") },
    { ScannerPlanePointCloud, LVOX_SP_PLANE_POINTCLOUD,  QObject::tr("Real shots - planar"), QObject::tr("The position of the scanner and the direction represent respectively a point of the plane and its normal vector. The origin of a shot is the point of the cloud projected on the plane, the direction is the point of the cloud. This for each point of the cloud.") },
    { ScannerSphericTheoreticScanner,  LVOX_SP_SPHERE_THEORETIC_SCANNER, QObject::tr("Theoretical shots from scanner - spherical"), QObject::tr("The shots are generated from the scanner data contained in each loaded file. In addition to offering a forced scanner for generated shots from the user parameters entered.") },
    { ScannerSphericTheoreticCustom,  LVOX_SP_SPHERE_THEORETIC_CUSTOM, QObject::tr("Theoretical shots - custom spherical"), QObject::tr("The shots are generated from the user parameters entered below.") }
};

//Registering the types of scanner
LVOX3_ScannerUtils::LVOX3_ScannerUtils()
{
    f.registerType<CT_ShootingPatternFromPointCloud>(LVOX_SP_SPHERE_POINTCLOUD);
    f.registerType<CT_ParallelShootingPatternFromPointCloud>(LVOX_SP_PLANE_POINTCLOUD);
    f.registerType<CT_ThetaPhiShootingPattern>(LVOX_SP_SPHERE_THEORETIC_SCANNER);
    f.registerType<CT_ThetaPhiShootingPattern>(LVOX_SP_SPHERE_THEORETIC_CUSTOM);
}

const QVector<ScannerDefinitionStruct> &LVOX3_ScannerUtils::getScannerDefinitions()
{
    return m_scannerDefinitions;
}

const ScannerDefinitionStruct &LVOX3_ScannerUtils::getScannerDefinition(ScannerTypeEnum id)
{
    return m_scannerDefinitions[id];
}

const ScannerDefinitionStruct &LVOX3_ScannerUtils::getScannerDefinition(const QString &label)
{
    for (const ScannerDefinitionStruct &def: m_scannerDefinitions) {
        if (def.label == label) {
            return def;
        }
    }

    Q_ASSERT(false);

    // FIXME: is this the proper way to handle the case where the scanner label is not found?
    return m_scannerDefinitions.at(0);
}

//This method unlocks certain options in the scanner options if the pattern is theoretical and keeps them locked if not
bool LVOX3_ScannerUtils::isCustomScannerConfiguration(ScannerTypeEnum id)
{
    return(id != ScannerSphericPointCloud && id != ScannerPlanePointCloud);
}
