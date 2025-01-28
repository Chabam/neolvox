#ifndef CT_MATH2DLINES_H
#define CT_MATH2DLINES_H

#include <Eigen/Core>

#include "ctlibmath_global.h"

class CTLIBMATH_EXPORT CT_Math2DLines
{
public:

    inline static double distance2D(const Eigen::Vector2d &p1, const Eigen::Vector2d &p2)
    {
        return sqrt( (p1(0) - p2(0))*(p1(0) - p2(0)) + (p1(1) - p2(1))*(p1(1) - p2(1)));
    }

    static bool intersectSegments(Eigen::Vector2d &a, Eigen::Vector2d &b, Eigen::Vector2d &c, Eigen::Vector2d &d, double &r, double &s, Eigen::Vector2d &intersection);

    static void computePerpendicularSegment(Eigen::Vector2d &a, Eigen::Vector2d &b, Eigen::Vector2d &c, Eigen::Vector2d &d, double length = -1, bool clockwise = false);

};

#endif // CT_MATH2DLINES_H
