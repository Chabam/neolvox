#ifndef SHOOTINGPATTERN_H
#define SHOOTINGPATTERN_H

#include <Eigen/Geometry>

#include "ray.h"

namespace lvox
{
/**
 * @class ShootingPattern
 * @brief Abstract base class for shooting patterns used in ray-based computations.
 *
 * This class represents an abstract base class for shooting patterns used in ray-based computations.
 * It provides methods to retrieve shooting rays based on point coordinates or ray indices and access
 * information about the shooting pattern, such as the center coordinate and the number of shots.
 */
class __declspec(dllexport) ShootingPattern
{
public:
    /**
     * @brief Get the center coordinate of the shooting pattern.
     * @return The center coordinate as a constant reference to a 3D vector.
     */
    virtual const Eigen::Vector3d& getCenterCoordinate() const = 0;

    /**
     * @brief Get the shooting ray corresponding to a specific point.
     * @param point The 3D vector representing the point.
     * @return The shooting ray as a Ray object.
     */
    virtual lvox::Ray getRayForPoint(const Eigen::Vector3d& point) const = 0;

    /**
     * @brief Get the shooting ray by its index in the pattern.
     * @param index The index of the shooting ray.
     * @return The shooting ray as a Ray object.
     */
    virtual lvox::Ray shotAt(size_t index) const = 0;

    /**
     * @brief Get the total number of shots in the shooting pattern.
     * @return The number of shots as a size_t value.
     */
    virtual size_t getNumberOfShots() const = 0;
};
} // namespace lvox

#endif // SHOOTINGPATTERN_H
