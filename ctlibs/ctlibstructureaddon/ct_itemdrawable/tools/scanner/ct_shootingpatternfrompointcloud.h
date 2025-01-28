/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef CT_SHOOTINGPATTERNFROMPOINTCLOUD_H
#define CT_SHOOTINGPATTERNFROMPOINTCLOUD_H

#include "ct_defines.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"
#include "ct_accessor/ct_pointaccessor.h"

/**
 * @brief A pattern of shots can be shown like an origin of shot and a cloud of
 *        point of impact, [point of impact] - [origin] = [direction] !
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_ShootingPatternFromPointCloud : public CT_ShootingPattern
{
public:
    /**
     * @brief default constructor
     */
    CT_ShootingPatternFromPointCloud();

    /**
     * @brief Construct a pattern with an origin and a list of index of points
     * @param origin : origin of all shots
     * @param pcir : list of index of points that represent an impact or to deduce the direction
     */
    CT_ShootingPatternFromPointCloud(const Eigen::Vector3d& origin,
                                     CT_PCIR pcir);

    /**
     * @brief Construct it from existing object (copy it)
     */
    CT_ShootingPatternFromPointCloud(const CT_ShootingPatternFromPointCloud& other);

    const Eigen::Vector3d& centerCoordinate() const { return m_origin; }
    inline const Eigen::Vector3d& origin() const { return m_origin; }
    virtual size_t numberOfShots() const;
    virtual CT_Shot shotAt(const size_t& index);
    virtual CT_Shot shotForPoint(const CT_Point& pt);
    virtual CT_ShootingPattern* clone() const;

private:
    Eigen::Vector3d     m_origin;
    CT_PCIR             m_pcir;
    CT_PointAccessor    m_pAccessor;
};

#endif // CT_SHOOTINGPATTERNFROMPOINTCLOUD_H
