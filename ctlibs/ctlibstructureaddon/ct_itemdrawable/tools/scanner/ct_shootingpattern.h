/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef CT_SHOOTINGPATTERN_H
#define CT_SHOOTINGPATTERN_H

#include "ct_shot.h"
#include "ct_point.h"

/**
 * @brief This class is abstract. It offers the possibility to define a specific
 *        pattern of shots for a scanner for example
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_ShootingPattern
{
public:
    virtual ~CT_ShootingPattern() {}

    /**
     * @brief Returns the center coordinate of this shooting pattern
     * @return Cartesian center coordinates
     */
    virtual const Eigen::Vector3d& centerCoordinate() const = 0;
    /**
     * @brief Returns the number of shots
     */
    virtual size_t numberOfShots() const = 0;

    /**
     * @brief Returns the shot at index specified
     */
    virtual CT_Shot shotAt(const size_t& index) = 0;

    /**
     * @brief Returns the shot for a given point
     */
    virtual CT_Shot shotForPoint(const CT_Point& pt) = 0;

    /**
     * @brief Copy this pattern
     */
    virtual CT_ShootingPattern* clone() const = 0;
};

#endif // CT_SHOOTINGPATTERN_H
