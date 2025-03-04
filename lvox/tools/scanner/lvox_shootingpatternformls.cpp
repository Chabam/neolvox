#include "lvox_shootingpatternformls.h"

#include "ct_cloudindex/registered/abstract/ct_abstractcloudindexregisteredt.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"
#include "ct_itemdrawable/tools/scanner/ct_shot.h"
#include "ct_itemdrawable/ct_scanpath.h"

LVOX_ShootingPatternForMLS::LVOX_ShootingPatternForMLS(CT_PCIR pcir, const CT_ScanPath& scanPath, CT_AbstractPointAttributesScalar* attributeGPS)
    : m_pcir(pcir)
    , m_scanPath(scanPath)
    , m_attributeGPS(attributeGPS)
{
}

const Eigen::Vector3d& LVOX_ShootingPatternForMLS::centerCoordinate() const {
    return m_origin;
}

size_t LVOX_ShootingPatternForMLS::numberOfShots() const {
    return m_pcir->abstractCloudIndexT()->size();
}

CT_Shot LVOX_ShootingPatternForMLS::shotAt(const size_t& index) const {
    const size_t idx = m_pcir->abstractCloudIndexT()->constIndexAt(index);
    const CT_Point point = m_pAccessor.pointAt(idx);

    const double gpsTime = m_attributeGPS->scalarAsDoubleAt(idx);

    Eigen::Vector3d scannerPosition = m_scanPath.getPathPointForGPSTime(gpsTime);
    Eigen::Vector3d direction = point - scannerPosition;

    return CT_Shot(scannerPosition, direction);
}

CT_Shot LVOX_ShootingPatternForMLS::shotForPoint(const CT_Point& pt) const {

    return CT_Shot(m_origin, pt - m_origin);
}

CT_ShootingPattern* LVOX_ShootingPatternForMLS::clone() const {
    return new LVOX_ShootingPatternForMLS(*this);
}

