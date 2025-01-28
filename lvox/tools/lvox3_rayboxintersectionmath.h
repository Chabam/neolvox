/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_RAYBOXINTERSECTIONMATH_H
#define LVOX3_RAYBOXINTERSECTIONMATH_H

#include "Eigen/Core"

/**
 * @brief Use this class to check/get the intersection of a ray with a box
 */
class LVOX3_RayBoxIntersectionMath
{
public:
    /**
     * @brief Compute the intersection of the ray with the box
     * @param origin : origin of the ray
     * @param direction : direction of the ray
     * @param start (OUT) : first impact point of the ray with the box
     * @param end (OUT) : second impact point of the ray with the box
     * @return true if it was an impact, false otherwise
     */
    static bool getIntersectionOfRay(const Eigen::Vector3d& bboxMin,
                                     const Eigen::Vector3d& bboxMax,
                                     const Eigen::Vector3d& origin,
                                     const Eigen::Vector3d& direction,
                                     Eigen::Vector3d& start,
                                     Eigen::Vector3d& end);

private:
    /**
     * @brief Utility method for intersection
     */
    static bool updateIntervals(const double &bottomCorner,const double &upperCorner,const double &origin,const double &direction,double &t0,double &t1);
};

#endif // LVOX3_RAYBOXINTERSECTIONMATH_H
