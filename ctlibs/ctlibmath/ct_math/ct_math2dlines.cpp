#include "ct_math2dlines.h"
#include "ct_math/ct_mathpoint.h"

bool CT_Math2DLines::intersectSegments(Eigen::Vector2d &a, Eigen::Vector2d &b, Eigen::Vector2d &c, Eigen::Vector2d &d, double &r, double &s, Eigen::Vector2d &intersection)
{
    double denom = (b(0)-a(0))*(d(1)-c(1)) - (b(1)-a(1))*(d(0)-c(0));
    if (denom==0) {return false;} // droites parallèles ou coincidentes

    r = ((a(1)-c(1))*(d(0)-c(0)) - (a(0)-c(0))*(d(1)-c(1))) / denom; // position sur le segment AB (inclus si entre 0 et 1)
    s = ((a(1)-c(1))*(b(0)-a(0)) - (a(0)-c(0))*(b(1)-a(1))) / denom; // position sur le segment CD (inclus si entre 0 et 1)

    intersection(0) = a(0) + r*(b(0)-a(0));
    intersection(1) = a(1) + r*(b(1)-a(1));
    return true;
}


void CT_Math2DLines::computePerpendicularSegment(Eigen::Vector2d &a, Eigen::Vector2d &b, Eigen::Vector2d &c, Eigen::Vector2d &d, double length, bool clockwise)
{
    double k = 1;

    if (length > 0)
        k = length/CT_Math2DLines::distance2D(a, b);

    if (clockwise) {
        d(0) = c(0) + k*(a(1) - b(1));
        d(1) = c(1) + k*(b(0) - a(0));
    } else {
        d(0) = c(0) + k*(b(1) - a(1));
        d(1) = c(1) + k*(a(0) - b(0));
    }
}
