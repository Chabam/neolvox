/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre PIBOULE (ONF)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
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

#ifndef CT_SPHERICALLINE3D_H
#define CT_SPHERICALLINE3D_H

#include "ct_shapedata/ct_linedata.h"

#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif
#include <math.h>

#include <Eigen/Core>

class CTLIBSTRUCTUREADDON_EXPORT CT_SphericalLine3D
{
public:

    CT_SphericalLine3D(Eigen::Vector3d* p1, Eigen::Vector3d* p2, bool orderByZ)
    {
        if (orderByZ && ((*p1)(2) > (*p2)(2)))
        {
            _p1 = p2;
            _p2 = p1;
        } else {
            _p1 = p1;
            _p2 = p2;
        }

        CT_SphericalLine3D::convertToSphericalCoordinates(_p1, _p2, _phi, _theta, _length);
    }


    inline CT_LineData* getLineData()
    {
        return new CT_LineData(*_p1, *_p2);
    }

    // champs
    Eigen::Vector3d*    _p1;        /*!< Point le plus bas */
    Eigen::Vector3d*    _p2;        /*!< Point le plus haut*/
    double               _phi;       /*!< Angle zénithal (du haut vers le bas)*/
    double               _theta;     /*!< Angle azimuthal (en sens anti-horaire)*/
    double               _length;    /*!< Longueur de segment*/



    // Méthodes statiques

    inline static QList<CT_SphericalLine3D*> getLinesFromPoints(const QList<Eigen::Vector3d*> &points, double minZenithalAngle = 0, double maxZenithalAngle = M_PI_2, bool orderByZ = true)
    {
        QList<CT_SphericalLine3D*> liste;
        int size = points.size();

        for (int i = 0 ; i < size ; i++)
        {
            Eigen::Vector3d* pi = points.at(i);
            for (int j = i+1 ; j < size ; j++)
            {
                Eigen::Vector3d* pj = points.at(j);
                CT_SphericalLine3D* line = new CT_SphericalLine3D(pi, pj, orderByZ);
                if ((line->_phi >= minZenithalAngle) && (line->_phi <= maxZenithalAngle))
                {
                    liste.append(line);
                } else {
                    delete line;
                }
            }
        }

        return liste;
    }

    inline static void convertToSphericalCoordinates(const Eigen::Vector3d* p1, const Eigen::Vector3d* p2, double &phi, double &theta, double &length)
    {
        double x = ((*p2)(0) - (*p1)(0));
        double y = ((*p2)(1) - (*p1)(1));
        double z = ((*p2)(2) - (*p1)(2));

        convertToSphericalCoordinates(x, y, z, phi, theta, length);
    }

    inline static void convertToSphericalCoordinates(double x, double y, double z, double &phi, double &theta, double &length)
    {
        length = sqrt(x*x + y*y + z*z);
        phi = acos(z/length);
        theta = acos(x/sqrt(x*x + y*y));
        if (y < 0)
        {
           theta = 2*M_PI - theta;
        }
    }

    inline static void convertToCartesianCoordinates(double phi, double theta, double length, double &x, double &y, double &z)
    {
        x = length * cos(theta) * sin(phi);
        y = length * sin(theta) * sin(phi);
        z = length * cos(phi);
    }
};

#endif // CT_SPHERICALLINE3D_H
