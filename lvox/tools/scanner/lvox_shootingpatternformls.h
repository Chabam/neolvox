#ifndef LVOX_SHOOTINGPATTERNFORMLS_H
#define LVOX_SHOOTINGPATTERNFORMLS_H

#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"
#include "ct_itemdrawable/ct_scanpath.h"
#include "ct_accessor/ct_pointaccessor.h"
#include <Eigen/Core>

/**
 * @brief This class defines a specific shooting pattern for MLS (Mobile Laser Scanning)
 */
class LVOX_ShootingPatternForMLS : public CT_ShootingPattern
{
public:

    /**
     * @brief Default constructor
     */
    LVOX_ShootingPatternForMLS();

    /**
     * @brief Constructor
     * @param pcir Point cloud index registered
     * @param scanPath Trajectory of the mobile lidar
     * @param attributeGPS GPS time attributes of the point cloud
     */
    LVOX_ShootingPatternForMLS(CT_PCIR pcir,const CT_ScanPath& scanPath, CT_AbstractPointAttributesScalar* attributeGPS);

    /**
     * @brief Returns the center coordinate of this shooting pattern
     * @return Cartesian center coordinates
     */
    virtual const Eigen::Vector3d& centerCoordinate() const override;

    /**
     * @brief Returns the number of shots
     * @return Number of shots
     */
    virtual size_t numberOfShots() const override;

    /**
     * @brief Returns the shot at index specified
     * @param index Index of the shot
     * @return CT_Shot at the given index
     */
    virtual CT_Shot shotAt(const size_t& index) override;

    /**
     * @brief Returns the shot for a given point
     * @param pt The point for which the shot is to be determined
     * @return CT_Shot for the given point
     */
    virtual CT_Shot shotForPoint(const CT_Point& pt) override;

    /**
     * @brief Copy this pattern
     * @return A copy of this shooting pattern
     */
    virtual CT_ShootingPattern* clone() const override;

private:
    Eigen::Vector3d m_origin; ///< Origin of the shooting pattern
    CT_PCIR m_pcir; ///< Point cloud
    const CT_ScanPath& m_scanPath; ///< Trajectory of the mobile lidar
    CT_AbstractPointAttributesScalar* m_attributeGPS; ///< GPS time attributes
    CT_PointAccessor m_pAccessor; ///< Accessor to points in the cloud
};

#endif // LVOX_SHOOTINGPATTERNFORMLS_H
