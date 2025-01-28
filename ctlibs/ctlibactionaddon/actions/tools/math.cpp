#include "math.h"

namespace AMKgl {
    Math::Math()
    {

    }

    void Math::getClosestPointBetweenPointAndLine(const Eigen::Vector3d &p, const Eigen::Vector3d &LP0, const Eigen::Vector3d &dir, Eigen::Vector3d& pOnLine)
    {
        const Eigen::Vector3d w = p - LP0;

        const double c1 = w.dot(dir);
        const double c2 = dir.dot(dir);
        const double b = c1 / c2;

        pOnLine = LP0 + b * dir;
    }

    void Math::getClosestPointBetweenPointAndSegment(const Eigen::Vector3d &p, const Eigen::Vector3d &seg_a, const Eigen::Vector3d &seg_b, Eigen::Vector3d &pOnLine)
    {
        Eigen::Vector3d v = seg_b - seg_a;
        Eigen::Vector3d w = p - seg_a;

        double c1 = w.dot(v);
        if ( c1 <= 0 ) {
            pOnLine = seg_a;
            return;
        }

        double c2 = v.dot(v);
        if ( c2 <= c1 ) {
            pOnLine = seg_b;
            return;
        }

        double b = c1 / c2;
        pOnLine = seg_a + b * v;
    }

    bool Math::isPointOnSegment(const Eigen::Vector3d &p, const Eigen::Vector3d &a, const Eigen::Vector3d &b)
    {
        Eigen::Vector3d AB = a-b;
        Eigen::Vector3d AC = a-p;

        double KAC = AB.dot(AC);
        if (KAC<0) return false;
        if (KAC==0) return false;

        // Compute the square of the segment lenght
        double KAB=AB.dot(AB);
        if (KAC>KAB) return false;
        if (KAC==KAB) return false;

        // The point is on the segment
        return true;
    }

    bool Math::isPointInEllipse(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP)
    {
        Eigen::Vector4d test(p(0), p(1), p(2), 1);
        test = inversedMatrix * test;

        if(inversedP != NULL)
            *inversedP = test.block(0,0,3,1);

        const double val = test.block(0,0,3,1).norm();
        return val <= 1.0;
    }

    bool Math::isPointInCylinder(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP)
    {
        Eigen::Vector4d test(p(0), p(1), p(2), 1);
        test = inversedMatrix * test;

        if(inversedP != NULL)
            *inversedP = test.block(0,0,3,1);

        const double val = sqrt(pow(test(0),2) + pow(test(2), 2));

        return (val <= 1) && (test(1) >= -0.5) && (test(1) <= 0.5);
    }

    bool Math::isPointInSphere(const Eigen::Vector3d &p, const Eigen::Matrix4d &inversedMatrix, Eigen::Vector3d *inversedP)
    {
        Eigen::Vector4d test(p(0), p(1), p(2), 1);
        test = inversedMatrix * test;

        if(inversedP != NULL)
            *inversedP = test.block(0,0,3,1);

        const double val = test.block(0,0,3,1).norm();
        return val <= 1.0;
    }

    QPoint Math::getClosestPolygonPointToPoint(const QPolygon &pol, const QPoint &p, double &distance, int *index)
    {
        QPoint retP;

        distance = std::numeric_limits<double>::max();

        if(index != NULL)
            *index = -1;

        if(!pol.isEmpty()) {
            retP = pol.first();
            double dist = distancePoints(retP, p);

            if(index != NULL)
                *index = 0;

            int i = 0;

            foreach (const QPoint &pos, pol) {
                double nDist = Math::distancePoints(pos, p);

                if(nDist < dist) {
                    retP = pos;
                    dist = nDist;

                    if(index != NULL)
                        *index = i;

                }
                ++i;
            }

            distance = dist;
        }

        return retP;
    }

    void Math::getClosestPointBetweenPointAndTriangle(const Eigen::Vector3d& p1,
                                                      const Eigen::Vector3d& tr1,
                                                      const Eigen::Vector3d& tr2,
                                                      const Eigen::Vector3d& tr3,
                                                      Eigen::Vector3d& closestPoint)
    {
        Eigen::Vector3d edge0 = tr2 - tr1;
        Eigen::Vector3d edge1 = tr3 - tr1;
        Eigen::Vector3d v0 = tr1 - p1;

        double a = edge0.dot( edge0 );
        double b = edge0.dot( edge1 );
        double c = edge1.dot( edge1 );
        double d = edge0.dot( v0 );
        double e = edge1.dot( v0 );

        double det = a*c - b*b;
        double s = b*e - c*d;
        double t = b*d - a*e;

        if ( s + t < det )
        {
            if ( s < 0.0 )
            {
                if ( t < 0.0 )
                {
                    if ( d < 0.0 )
                    {
                        s = qBound( -d/a, 0.0, 1.0 );
                        t = 0.0;
                    }
                    else
                    {
                        s = 0.0;
                        t = qBound( -e/c, 0.0, 1.0 );
                    }
                }
                else
                {
                    s = 0.0;
                    t = qBound( -e/c, 0.0, 1.0 );
                }
            }
            else if ( t < 0.0 )
            {
                s = qBound( -d/a, 0.0, 1.0 );
                t = 0.0;
            }
            else
            {
                double invDet = 1.0 / det;
                s *= invDet;
                t *= invDet;
            }
        }
        else
        {
            if ( s < 0.0 )
            {
                double tmp0 = b+d;
                double tmp1 = c+e;
                if ( tmp1 > tmp0 )
                {
                    double numer = tmp1 - tmp0;
                    double denom = a-2*b+c;
                    s = qBound( numer/denom, 0.0, 1.0 );
                    t = 1-s;
                }
                else
                {
                    t = qBound( -e/c, 0.0, 1.0 );
                    s = 0.0;
                }
            }
            else if ( t < 0.0 )
            {
                if ( a+d > b+e )
                {
                    double numer = c+e-b-d;
                    double denom = a-2*b+c;
                    s = qBound( numer/denom, 0.0, 1.0 );
                    t = 1-s;
                }
                else
                {
                    s = qBound( -e/c, 0.0, 1.0 );
                    t = 0.0;
                }
            }
            else
            {
                double numer = c+e-b-d;
                double denom = a-2*b+c;
                s = qBound( numer/denom, 0.0, 1.0 );
                t = 1.0 - s;
            }
        }

        closestPoint = tr1 + s * edge0 + t * edge1;
    }

    /*double Orthonormalize(int numInputs, Eigen::Vector3d* v)
    {
        if (v && 1 <= numInputs && numInputs <= 3)
        {
            v[0].normalize();
            double minLength = v[0].norm();
            for (int i = 1; i < numInputs; ++i)
            {
                for (int j = 0; j < i; ++j)
                {
                    double dot = v[i].dot(v[j]);
                    v[i] -= v[j] * dot;
                }
                v[i].normalize();
                double length = v[i].norm();
                if (length < minLength)
                {
                    minLength = length;
                }
            }
            return minLength;
        }

        return (double)0;
    }

    double ComputeOrthogonalComplement(int numInputs, Eigen::Vector3d* v)
    {
        if (numInputs == 1)
        {
            if (fabs(v[0][0]) > fabs(v[0][1]))
            {
                v[1] = { -v[0][2], (double)0, +v[0][0] };
            }
            else
            {
                v[1] = { (double)0, +v[0][2], -v[0][1] };
            }
            numInputs = 2;
        }

        if (numInputs == 2)
        {
            v[2] = v[0].cross(v[1]);
            return Orthonormalize(3, v);
        }

        return (double)0;
    }

    void getClosestPointBetweenSegmentAndLine(const Eigen::Vector3d &lineOrigin, const Eigen::Vector3d &lineDirection,
                                              const Eigen::Vector3d& segCenter, const Eigen::Vector3d& segDirection, const double& segExtent,
                                              double& lsResultSqrDistance,
                                              double lsResultParameter[2],
                                              Eigen::Vector3d lsResultClosestPoint[2])
    {
        Eigen::Vector3d diff = lineOrigin - segCenter;
        double a01 = -lineDirection.dot(segDirection);
        double b0 = diff.dot(lineDirection);
        double s0, s1;

        if (std::abs(a01) < (double)1)
        {
            // The line and segment are not parallel.
            double det = (double)1 - a01 * a01;
            double extDet = segExtent * det;
            double b1 = -diff.dot(segDirection);
            s1 = a01 * b0 - b1;

            if (s1 >= -extDet)
            {
                if (s1 <= extDet)
                {
                    // Two interior points are closest, one on the line and one
                    // on the segment.
                    s0 = (a01 * b1 - b0) / det;
                    s1 /= det;
                }
                else
                {
                    // The endpoint e1 of the segment and an interior point of
                    // the line are closest.
                    s1 = segExtent;
                    s0 = -(a01 * s1 + b0);
                }
            }
            else
            {
                // The endpoint e0 of the segment and an interior point of the
                // line are closest.
                s1 = -segExtent;
                s0 = -(a01 * s1 + b0);
            }
        }
        else
        {
            // The line and segment are parallel.  Choose the closest pair so that
            // one point is at segment origin.
            s1 = (double)0;
            s0 = -b0;
        }

        lsResultParameter[0] = s0;
        lsResultParameter[1] = s1;
        lsResultClosestPoint[0] = lineOrigin + s0 * lineDirection;
        lsResultClosestPoint[1] = segCenter + s1 * segDirection;
        diff = lsResultClosestPoint[0] - lsResultClosestPoint[1];
        lsResultSqrDistance = diff.dot(diff);
        //result.distance = sqrt(lsResultSqrDistance);
    }

    bool Math::getClosestPointBetweenSegmentAndTriangle(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2,
                                                        Eigen::Vector3d tr[],
                                                        Eigen::Vector3d closestPoint[],
                                                        double &lineParameter)
    {
        //Result result;

        // Test if line intersects triangle.  If so, the squared distance is zero.
        Eigen::Vector3d edge0 = tr[1] - tr[0];
        Eigen::Vector3d edge1 = tr[2] - tr[0];
        Eigen::Vector3d normal = edge0.cross(edge1).normalized();

        Eigen::Vector3d lineOrigin = p1;
        Eigen::Vector3d lineDirection = p2-p1;
        double NdD = normal.dot(lineDirection);
        if (std::abs(NdD) > (double)0)
        {
            // The line and triangle are not parallel, so the line intersects
            // the plane of the triangle.
            Eigen::Vector3d diff = lineOrigin - tr[0];
            Eigen::Vector3d basis[3];  // {D, U, V}
            basis[0] = lineDirection;
            ComputeOrthogonalComplement(1, basis);
            double UdE0 = basis[1].dot(edge0);
            double UdE1 = basis[1].dot(edge1);
            double UdDiff = basis[1].dot(diff);
            double VdE0 = basis[2].dot(edge0);
            double VdE1 = basis[2].dot(edge1);
            double VdDiff = basis[2].dot(diff);
            double invDet = ((double)1) / (UdE0*VdE1 - UdE1*VdE0);

            // Barycentric coordinates for the point of intersection.
            double b1 = (VdE1*UdDiff - UdE1*VdDiff)*invDet;
            double b2 = (UdE0*VdDiff - VdE0*UdDiff)*invDet;
            double b0 = (double)1 - b1 - b2;

            if (b0 >= (double)0 && b1 >= (double)0 && b2 >= (double)0)
            {
                // Line parameter for the point of intersection.
                double DdE0 = lineDirection.dot(edge0);
                double DdE1 = lineDirection.dot(edge1);
                double DdDiff = lineDirection.dot(diff);

                lineParameter = b1*DdE0 + b2*DdE1 - DdDiff;
    //            result.lineParameter = b1*DdE0 + b2*DdE1 - DdDiff;

    //            // Barycentric coordinates for the point of intersection.
    //            result.triangleParameter[0] = b0;
    //            result.triangleParameter[1] = b1;
    //            result.triangleParameter[2] = b2;

    //            // The intersection point is inside or on the triangle.
    //            result.closestPoint[0] = lineOrigin + result.lineParameter*lineDirection;
    //            result.closestPoint[1] = tr[0] + b1*edge0 + b2*edge1;

    //            result.distance = (double)0;
    //            result.sqrDistance = (double)0;

    //            return result;

                closestPoint[0] = lineOrigin + lineParameter*lineDirection;
                closestPoint[1] = tr[0] + b1*edge0 + b2*edge1;

                return true;
            }
        }

        // Either (1) the line is not parallel to the triangle and the point of
        // intersection of the line and the plane of the triangle is outside the
        // triangle or (2) the line and triangle are parallel.  Regardless, the
        // closest point on the triangle is on an edge of the triangle.  Compare
        // the line to all three edges of the triangle.
        double distance = std::numeric_limits<double>::max();
        double sqrDistance = std::numeric_limits<double>::max();

        for (int i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
        {
            Eigen::Vector3d segCenter = ((double)0.5)*(tr[i0] + tr[i1]);
            Eigen::Vector3d segDirection = tr[i1] - tr[i0];
            segDirection.normalize();
            double segExtent = ((double)0.5)*segDirection.norm();

            double lsResultSqrDistance;
            double lsResultParameter[2];
            Eigen::Vector3d lsResultClosestPoint[2];
            getClosestPointBetweenSegmentAndLine(lineOrigin, lineDirection, segCenter, segDirection, segExtent, lsResultSqrDistance, lsResultParameter, lsResultClosestPoint);

            if (lsResultSqrDistance < sqrDistance)
            {
                //sqrDistance = lsResult.sqrDistance;
                //distance = lsResult.distance;

                lineParameter = lsResultParameter[0];

    //            result.triangleParameter[i0] = ((double)0.5)*((double)1 -
    //                lsResult.parameter[0] / segExtent);
    //            result.triangleParameter[i1] = (double)1 -
    //                result.triangleParameter[i0];
    //            result.triangleParameter[3 - i0 - i1] = (double)0;

                closestPoint[0] = lsResultClosestPoint[0];
                closestPoint[1] = lsResultClosestPoint[1];
            }
        }

        return true;
    }*/



    bool Math::isPointInPolygon(const QPolygon &pol, const QPoint &p)
    {
        return (pol.size() > 1) && pol.containsPoint(p, Qt::OddEvenFill);
    }
}
