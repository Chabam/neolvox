#ifndef RAY_H
#define RAY_H

#include <Eigen/Geometry>

namespace lvox
{

/**
 * @class Ray
 * @brief Represents a ray in 3D space with an origin and direction.
 *
 * This class represents a ray in 3D space, defined by an origin point and a direction vector.
 * It provides methods to calculate intersections with 3D bounding boxes and retrieve the ray's properties.
 */
class __declspec(dllexport) Ray
{
public:
    /**
     * @brief Constructor to create a Ray object.
     * @param origin The origin point of the ray as a 3D vector.
     * @param direction The direction vector of the ray as a 3D vector.
     */
    Ray(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction) : _origin(origin), _direction(direction) {}
    Ray(Ray const & old) : _origin(old.getOrigin()), _direction(old.getDirection()) {}

    /**
     * @brief Get the origin point of the ray.
     * @return The origin point as a constant reference to a 3D vector.
     */
    virtual const Eigen::Vector3d& getOrigin() const;

    /**
     * @brief Get the direction vector of the ray.
     * @return The direction vector as a constant reference to a 3D vector.
     */
    virtual const Eigen::Vector3d& getDirection() const;

    /**
     * @brief Calculate the intersection points of the ray with a 3D bounding box.
     * @param bottom The bottom corner of the bounding box as a reference to a 3D vector.
     * @param top The top corner of the bounding box as a reference to a 3D vector.
     * @param[out] outVoxelIn The intersection point of the ray entering the bounding box as a reference to a 3D vector.
     * @param[out] outVoxelOut The intersection point of the ray exiting the bounding box as a reference to a 3D vector.
     * @return True if the ray intersects the bounding box, false otherwise.
     */
    bool intersect(Eigen::Vector3d& bottom, Eigen::Vector3d& top, Eigen::Vector3d& outVoxelIn,
                   Eigen::Vector3d& outVoxelOut) const;

    /**
     * @brief Get the intersection points of the ray with a 3D bounding box.
     * @param bboxMin The minimum corner of the bounding box as a 3D vector.
     * @param bboxMax The maximum corner of the bounding box as a 3D vector.
     * @param[out] start The starting intersection point of the ray with the bounding box as a reference to a 3D vector.
     * @param[out] end The ending intersection point of the ray with the bounding box as a reference to a 3D vector.
     * @return True if the ray intersects the bounding box, false otherwise.
     */
    bool getIntersectionOfRay(const Eigen::Vector3d& bboxMin, const Eigen::Vector3d& bboxMax, Eigen::Vector3d& start,
                              Eigen::Vector3d& end) const;

private:
    /**
     * @brief Update the intervals of the ray's intersections with a 3D bounding box.
     * @param bottomCorner The coordinate value of the bottom corner of the bounding box.
     * @param upperCorner The coordinate value of the upper corner of the bounding box.
     * @param origin The coordinate value of the origin of the ray.
     * @param direction The coordinate value of the direction vector of the ray.
     * @param[out] t0 The distance from the origin to the first intersection point.
     * @param[out] t1 The distance from the origin to the second intersection point.
     * @return True if the ray intersects the bounding box, false otherwise.
     */
    bool updateIntervals(const double& bottomCorner, const double& upperCorner, const double& origin,
                         const double& direction, double& t0, double& t1) const;

    static const double EPSILON;         ///< A small value used for numerical stability.

    const Eigen::Vector3d _origin;     ///< The origin point of the ray.
    const Eigen::Vector3d _direction;  ///< The direction vector of the ray.
};

} // namespace lvox

#endif // RAY_H
