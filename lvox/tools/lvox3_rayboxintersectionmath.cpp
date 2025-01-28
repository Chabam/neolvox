#include "lvox3_rayboxintersectionmath.h"

#define EPSILON_INTERSECTION_RAY 0.000001    // 10^-6

bool LVOX3_RayBoxIntersectionMath::getIntersectionOfRay(const Eigen::Vector3d& bboxMin,
                                                        const Eigen::Vector3d& bboxMax,
                                                        const Eigen::Vector3d& origin,
                                                        const Eigen::Vector3d& direction,
                                                        Eigen::Vector3d& start,
                                                        Eigen::Vector3d& end)
{
    double t0 = 0;
    double t1 = std::numeric_limits<double>::max();

    if (!updateIntervals(bboxMin(0), bboxMax(0), origin(0), direction(0), t0, t1)) {return false;}
    if (!updateIntervals(bboxMin(1), bboxMax(1), origin(1), direction(1), t0, t1)) {return false;}
    if (!updateIntervals(bboxMin(2), bboxMax(2), origin(2), direction(2), t0, t1)) {return false;}

    start = origin + direction*t0;
    end  = origin + direction*t1;

    return true;
}

bool LVOX3_RayBoxIntersectionMath::updateIntervals(const double &bottomCorner, const double &upperCorner, const double &origin, const double &direction, double &t0, double &t1)
{
    // Update interval for bounding box slab
    const double invRayDir = 1.0 / direction;
    double tNear = (bottomCorner - origin) * invRayDir;
    double tFar  = (upperCorner - origin) * invRayDir;

    // Update parametric interval from slab intersection $t$s
    if (tNear > tFar) std::swap(tNear, tFar);

    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar  < t1 ? tFar  : t1;

    if ((t0 > t1)
            && ((t0 - t1) > EPSILON_INTERSECTION_RAY)) // t0 being always > t1, (t0-t1) is always positive
        return false;

    return true;
}
