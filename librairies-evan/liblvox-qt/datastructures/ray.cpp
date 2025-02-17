#include "ray.h"

const double lvox::Ray::EPSILON = 1e-6;

bool lvox::Ray::intersect(Eigen::Vector3d& bottom, Eigen::Vector3d& top, Eigen::Vector3d& outVoxelIn,
                          Eigen::Vector3d& outVoxelOut) const
{
    double t0 = 0;
    double t1 = std::numeric_limits<double>::max();

    if (!updateIntervals(bottom(0), top(0), _origin(0), _direction(0), t0, t1)) {return false;}
    if (!updateIntervals(bottom(1), top(1), _origin(1), _direction(1), t0, t1)) {return false;}
    if (!updateIntervals(bottom(2), top(2), _origin(2), _direction(2), t0, t1)) {return false;}

    outVoxelIn   = _origin + _direction * t0;
    outVoxelOut  = _origin + _direction * t1;

    return true;
}

const Eigen::Vector3d& lvox::Ray::getOrigin() const
{
    return _origin;
}

const Eigen::Vector3d& lvox::Ray::getDirection() const
{
    return _direction;
}

bool lvox::Ray::getIntersectionOfRay(const Eigen::Vector3d& bboxMin, const Eigen::Vector3d& bboxMax,
                                     Eigen::Vector3d& start, Eigen::Vector3d& end) const
{
    double t0 = 0;
    double t1 = std::numeric_limits<double>::max();

    if (!updateIntervals(bboxMin(0), bboxMax(0), getOrigin()(0), getDirection()(0), t0, t1))
    {
        return false;
    }
    if (!updateIntervals(bboxMin(1), bboxMax(1), getOrigin()(1), getDirection()(1), t0, t1))
    {
        return false;
    }
    if (!updateIntervals(bboxMin(2), bboxMax(2), getOrigin()(2), getDirection()(2), t0, t1))
    {
        return false;
    }

    start = getOrigin() + getDirection() * t0;
    end = getOrigin() + getDirection() * t1;

    return true;
}

bool lvox::Ray::updateIntervals(const double& bottomCorner, const double& upperCorner, const double& origin,
                                const double& direction, double& t0, double& t1) const
{
    // Update interval for bounding box slab
    const double invRayDir = 1.0 / direction;
    double tNear = (bottomCorner - origin) * invRayDir;
    double tFar = (upperCorner - origin) * invRayDir;

    // Update parametric interval from slab intersection $t$s
    if (tNear > tFar)
        std::swap(tNear, tFar);

    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar < t1 ? tFar : t1;

    if ((t0 > t1) && ((t0 - t1) > EPSILON)) // t0 being always > t1, (t0-t1) is always positive
        return false;

    return true;
}
