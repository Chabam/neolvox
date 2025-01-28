/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

 This file is part of PluginShared library 2.0.

 PluginShared is free librardir(1) = you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_MATHPOINT_H
#define CT_MATHPOINT_H

#include "ctlibmath_global.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <QMatrix4x4>

#include <Eigen/Dense>

class CTLIBMATH_EXPORT CT_MathPoint
{
public:

    enum RotationAxis
    {
        OX, OY, OZ
    };

    typedef enum RotationAxis RotationAxis;

    inline static double distance2D(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2)
    {
        return sqrt( (p1(0) - p2(0))*(p1(0) - p2(0)) + (p1(1) - p2(1))*(p1(1) - p2(1)));
    }

    inline static double distance3D(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2)
    {
        return sqrt( (p1(0) - p2(0))*(p1(0) - p2(0)) + (p1(1) - p2(1))*(p1(1) - p2(1)) + (p1(2) - p2(2))*(p1(2) - p2(2)));
    }

    inline static double angle(const Eigen::Vector3d &v1, const Eigen::Vector3d &v2)
    {
        double norm1 = v1.norm();
        double norm2 = v2.norm();

        if ( norm1 == 0 || norm2 == 0 )
        {
            return 0;
        }

        double cosAlpha = v1.dot(v2) / (norm1*norm2);

        if (cosAlpha > 1 && cosAlpha < 1.000001)
        {
            return 0;
        }
        return acos( cosAlpha );
    }

    inline static double angleUnsigned(const Eigen::Vector3d &v1, const Eigen::Vector3d &v2)
    {
        double norm1 = v1.norm();
        double norm2 = v2.norm();

        if ( norm1 == 0 || norm2 == 0 )
        {
            return 0;
        }

        double cosAlpha = fabs(v1.dot(v2)) / (norm1*norm2);

        if (cosAlpha > 1 && cosAlpha < 1.000001)
        {
            return 0;
        }
        return acos( cosAlpha );
    }


    inline static double distancePointLine ( const Eigen::Vector3d &p, const Eigen::Vector3d &lineDirection, const Eigen::Vector3d &pointOnLine )
    {
        // Vector from pointOnLine to p
        Eigen::Vector3d polP;
        polP = pointOnLine - p;

        Eigen::Vector3d crossP = polP.cross(lineDirection);

        return ( crossP.norm() / lineDirection.norm() );
    }

    inline static double distancePointPlane ( const Eigen::Vector3d &p, const Eigen::Vector3d &planeNormal, Eigen::Vector3d &pointOnPlane )
    {
        // Vector from pointOnPlane to p
        Eigen::Vector3d popP;
        popP = pointOnPlane - p;

        return fabs(popP.dot(planeNormal) ) / planeNormal.norm();
    }

    inline static double distancePointPlaneSigned ( const Eigen::Vector3d &p, const Eigen::Vector3d &planeNormal, Eigen::Vector3d &pointOnPlane )
    {
        // Vector from pointOnPlane to p
        Eigen::Vector3d popP;
        popP = pointOnPlane - p;

        return popP.dot(planeNormal) / planeNormal.norm();
    }


    inline static double distanceOnLineForPointProjection ( const Eigen::Vector3d &origin, const Eigen::Vector3d &direction, const Eigen::Vector3d &point, Eigen::Vector3d &projectedPoint)
    {
        double num = direction(0)*(point(0) - origin(0)) + direction(1)*(point(1) - origin(1)) + direction(2)*(point(2) - origin(2));
        double den = direction(0)*direction(0) + direction(1)*direction(1) + direction(2)*direction(2);

        if (den == 0) {return -1;}
        double length = num / den ;

        projectedPoint = origin + direction*length;

        return length * sqrt(den);
    }

    // Constructs a plane from a collection of points
    // so that the summed squared distance to all points is minimzized
    inline static bool fitPlaneFromPoints(const QList<Eigen::Vector3d>& points, Eigen::Vector3d &direction, Eigen::Vector3d &centroid)
    {
        if (points.size() < 3)
        {
            return false; // At least three points required
        }

        centroid = Eigen::Vector3d(0,0,0);

        for (const Eigen::Vector3d &p : points)
        {
            centroid += p / points.size();
        }

        // Calc full 3x3 covariance matrix, excluding symmetries:
        double xx = 0.0;
        double xy = 0.0;
        double xz = 0.0;
        double yy = 0.0;
        double yz = 0.0;
        double zz = 0.0;

        for (const Eigen::Vector3d &p : points)
        {
            Eigen::Vector3d r = p - centroid;
            xx += r(0) * r(0);
            xy += r(0) * r(1);
            xz += r(0) * r(2);
            yy += r(1) * r(1);
            yz += r(1) * r(2);
            zz += r(2) * r(2);
        }

        double det_x = yy*zz - yz*yz;
        double det_y = xx*zz - xz*xz;
        double det_z = xx*yy - xy*xy;

        double det_max = std::max(std::max(det_x, det_y), det_z);

        if (det_max <= 0.0)
        {
            return false; // The points don't span a plane
        }

        // Pick path with best conditioning:
        if (det_max == det_x)
        {
            direction(0) = det_x;
            direction(1) = xz*yz - xy*zz;
            direction(2) = xy*yz - xz*yy;
        } else if (det_max == det_y)
        {
            direction(0) = xz*yz - xy*zz;
            direction(1) = det_y;
            direction(2) = xy*xz - yz*xx;
        } else
        {
            direction(0) = xy*yz - xz*yy;
            direction(1) = xy*xz - yz*xx;
            direction(2) = det_z;
        }

        direction.normalize();

        return true;
    }


    // *********************************************************** //
    // Rotation
    // *********************************************************** //

    inline static Eigen::Vector3d rotate( const Eigen::Vector3d& p, RotationAxis axis, double alpha )
    {
        double c = cos( alpha );
        double s = sin( alpha );

        return rotate( p, axis, c, s );
    }

    inline static Eigen::Vector3d rotate( const Eigen::Vector3d& p, RotationAxis axis, double cosAlpha, double sinAlpha )
    {
        Eigen::Vector3d rslt;

        switch ( axis )
        {
            case OX :
            {
                rslt(0) = p(0);
                rslt(1) = (p(1) * cosAlpha) + ( p(2) * (-sinAlpha));
                rslt(2) = (p(1) * sinAlpha) + ( p(2) * cosAlpha );
                break;
            }

            case OY :
            {
                rslt(1) = p(1);
                rslt(2) = (p(2) * cosAlpha) + ( p(0) * (-sinAlpha));
                rslt(0) = (p(2) * sinAlpha) + ( p(0) * cosAlpha );
                break;
            }

            case OZ :
            {
                rslt(2) = p(2);
                rslt(0) = (p(0) * cosAlpha) + ( p(1) * (-sinAlpha));
                rslt(1) = (p(0) * sinAlpha) + ( p(1) * cosAlpha );
                break;
            }

            default :
                break;
        }

        return rslt;
    }

    inline static Eigen::Vector3d rotate( const Eigen::Vector3d& p, const Eigen::Vector3d& rotationAxis, double alpha )
    {
        double c = cos( alpha );
        double s = sin( alpha );
        Eigen::Vector3d rotationUnitAxis = rotationAxis / rotationAxis.norm();

        return rotate( p, rotationUnitAxis, c, s );
    }

    inline static Eigen::Vector3d rotate( const Eigen::Vector3d& p, const Eigen::Vector3d& unitRotationAxis, double cosAlpha, double sinAlpha )
    {
        Eigen::Vector3d rslt;
        rslt(0) =   ( ( (unitRotationAxis(0) * unitRotationAxis(0)) + ( (1 - (unitRotationAxis(0) * unitRotationAxis(0)) ) * cosAlpha) ) * (p(0)) ) +
                    ( ( (unitRotationAxis(0) * unitRotationAxis(1) * ( 1 - cosAlpha ) ) - (unitRotationAxis(2) * sinAlpha) ) * (p(1)) ) +
                    ( ( (unitRotationAxis(0) * unitRotationAxis(2) * ( 1 - cosAlpha ) ) + (unitRotationAxis(1) * sinAlpha) ) * (p(2)) );

        rslt(1) =   ( ( (unitRotationAxis(0) * unitRotationAxis(1) * ( 1 - cosAlpha ) ) + (unitRotationAxis(2) * sinAlpha) ) * (p(0)) ) +
                    ( ( (unitRotationAxis(1) * unitRotationAxis(1)) + ( (1 - (unitRotationAxis(1) * unitRotationAxis(1)) ) * cosAlpha) ) * (p(1)) ) +
                    ( ( (unitRotationAxis(1) * unitRotationAxis(2) * ( 1 - cosAlpha ) ) - (unitRotationAxis(0) * sinAlpha) ) * (p(2)) );

        rslt(2) =   ( ( (unitRotationAxis(0) * unitRotationAxis(2) * ( 1 - cosAlpha ) ) - (unitRotationAxis(1) * sinAlpha) ) * (p(0)) ) +
                    ( ( (unitRotationAxis(1) * unitRotationAxis(2) * ( 1 - cosAlpha ) ) + (unitRotationAxis(0) * sinAlpha) ) * (p(1)) ) +
                    ( ( (unitRotationAxis(2) * unitRotationAxis(2)) + ( (1 - (unitRotationAxis(2) * unitRotationAxis(2)) ) * cosAlpha) ) * (p(2)) );

        return rslt;
    }

    // En spherique on stocke r, theta, phi
    inline static void cartesianToSpherical ( const Eigen::Vector3d& pCartesian, Eigen::Vector3d& outSpherical)
    {
        // On recupere la longueur du vecteur normalisé
        double norme = pCartesian.norm();

        if ( norme == 0 )
        {
            outSpherical(0) = 0;
            outSpherical(1) = 0;
            outSpherical(2) = 0;
        }
        else
        {
            // r est la norme du veteur
            outSpherical(0) = norme;

            // theta depend du signe de y, on ne peut pas diviser par 0 ce qui arrive quand x et y sont nuls
            if ( pCartesian(0) == 0 && pCartesian(1) == 0 )
            {
                outSpherical(1) = 0;
            }

            // Petit problème lorsque y est nul, on arrive a acos(x/x) qui plante pck acos(1) marche pas...
            else if ( pCartesian(1) == 0 )
            {
                outSpherical(1) = 0;
            }

            else if ( pCartesian(1) > 0 )
            {
                outSpherical(1) = acos(pCartesian(0) / sqrt( pCartesian(0)*pCartesian(0) + pCartesian(1)*pCartesian(1) ) );
            }

            else
            {
                outSpherical(1) = (2 * M_PI) - acos(pCartesian(0) / sqrt( pCartesian(0)*pCartesian(0) + pCartesian(1)*pCartesian(1) ));
            }

            // phi depend de la hauteur du point
            outSpherical(2) = acos( pCartesian(2) / norme );
        }
    }

    inline static void sphericalToCartesian ( const Eigen::Vector3d& spher, Eigen::Vector3d& cart )
    {
        // En spherique on stocke r, theta, phi
        cart(0) =  spher(0) * cos( spher(1) ) * sin( spher(2) );
        cart(1) =  spher(0) * sin( spher(1) ) * sin( spher(2) );
        cart(2) =  spher(0) * cos( spher(2) );
    }


    static void transform(const QMatrix4x4 &matrix, Eigen::Vector3d &point)
    {
        double x, y, z, w;
        x = point(0) * matrix(0,0) +
            point(1) * matrix(0,1) +
            point(2) * matrix(0,2) +
            matrix(0,3);
        y = point(0) * matrix(1,0) +
            point(1) * matrix(1,1) +
            point(2) * matrix(1,2) +
            matrix(1,3);
        z = point(0) * matrix(2,0) +
            point(1) * matrix(2,1) +
            point(2) * matrix(2,2) +
            matrix(2,3);
        w = point(0) * matrix(3,0) +
            point(1) * matrix(3,1) +
            point(2) * matrix(3,2) +
            matrix(3,3);

        if (w == 1.0f)
        {
            point(0) = x;
            point(1) = y;
            point(2) = z;
        }
        else
        {
            point(0) = x/w;
            point(1) = y/w;
            point(2) = z/w;
        }
    }

};

#endif // CT_MATHPOINT_H
