/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_cylinderdata.h"

#include "ct_pointcloudindex/abstract/ct_abstractpointcloudindex.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#include <math.h>
#endif

const double CT_CylinderData::M_PI_X2 = M_PI*2.0;

CT_CylinderData::CT_CylinderData() : SuperClass(),
    _radius(0),
    _h(0),
    _lineError(0),
    _circleError(0)
{
}

CT_CylinderData::CT_CylinderData(const Eigen::Vector3d& center,
                                 const Eigen::Vector3d& direction,
                                 double radius,
                                 double h) : SuperClass(center, direction),
    _radius(radius),
    _h(h),
    _lineError(0),
    _circleError(0)
{
}

CT_CylinderData::CT_CylinderData(const Eigen::Vector3d& center,
                                 const Eigen::Vector3d& direction,
                                 double radius,
                                 double h,
                                 double lineError,
                                 double circleError) : SuperClass(center, direction),
    _radius(radius),
    _h(h),
    _lineError(lineError),
    _circleError(circleError)
{
}

double CT_CylinderData::getRadius() const
{
    return _radius;
}

double CT_CylinderData::getHeight() const
{
    return _h;
}

double CT_CylinderData::getLineError() const
{
    return _lineError;
}

double CT_CylinderData::getCircleError() const
{
    return _circleError;
}

void CT_CylinderData::setLineError(double error)
{
    _lineError  = error;
}

void CT_CylinderData::setCircleError(double error)
{
    _circleError  = error;
}

void CT_CylinderData::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    double dist = std::max(_radius, _h/2.0);

    min(0) = _center(0) - dist;
    min(1) = _center(1) - dist;
    min(2) = _center(2) - dist;

    max(0) = _center(0) + dist;
    max(1) = _center(1) + dist;
    max(2) = _center(2) + dist;
}

CT_CylinderData* CT_CylinderData::staticCreate3DCylinderDataFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                           const Eigen::Vector3d& pointCloudBarycenter)
{
    if(pointCloudIndex.size() < 3)
        return nullptr;

    // fitting des points avec une ligne 3D
    CT_LineData* lineData = CT_LineData::staticCreateLineDataFromPointCloud(pointCloudIndex);

    CT_CylinderData* data = CT_CylinderData::staticCreate3DCylinderDataFromPointCloudAndDirection(pointCloudIndex, pointCloudBarycenter, *lineData);
    delete lineData;

    return data;
}

CT_CylinderData* CT_CylinderData::staticCreate3DCylinderDataFromPointCloudAndDirection(const CT_AbstractPointCloudIndex& pointCloudIndex,
                                                                                       const Eigen::Vector3d& pointCloudBarycenter,
                                                                                       const CT_LineData& direction,
                                                                                       CT_CircleData* outCircleData)
{
    if(pointCloudIndex.size() < 3)
        return nullptr;

    // on va maintenant deplacer et tourner tous les points
    // pour qu'ils soient centres sur l'origine et parallele a l'axe Z

    double x1, y1, z1, x2, y2, z2;

    x1 = direction.x1();
    y1 = direction.y1();
    z1 = direction.z1();
    x2 = direction.x2();
    y2 = direction.y2();
    z2 = direction.z2();

    // P2 toujours en dessous de P1 pour que le calcul se passe bien
    if(z2 > z1)
    {
        x1 = direction.x2();
        y1 = direction.y2();
        z1 = direction.z2();
        x2 = direction.x1();
        y2 = direction.y1();
        z2 = direction.z1();
    }

    // translation des points
    double tx = -pointCloudBarycenter.x();
    double ty = -pointCloudBarycenter.y();
    double tz = -pointCloudBarycenter.z();

    Eigen::Vector3d vect_line(x1-x2, y1-y2, z1-z2);
    vect_line.normalize();

    Eigen::Vector3d vect_line2(x1-x2, y1-y2, 0);
    vect_line2.normalize();

    Eigen::Vector3d vx(1,0,0);
    Eigen::Vector3d vz(0,0,1);

    // rotation des points a effectuer autour de l'axe z
    double rz = acos(vect_line2.dot(vx));

    if(vect_line.y() > 0)
    {
        rz = CT_CylinderData::M_PI_X2 - rz;
    }

    Eigen::Vector3d vect_line3(vect_line(0)*cos(rz) - vect_line(1)*sin(rz), 0, vect_line(2));
    vect_line3.normalize();

    // rotation des points a effectuer autour de l'axe y
    double ry = acos(vect_line3.dot(vz));

    /*1, 0, 1, 0*/

    // on deplace et tourne les points puis on fit un cercle
    CircleDataPreProcessingAction preProcessingAction(Eigen::Vector3d(tx, ty, tz), cos(rz), sin(rz), cos(ry), sin(ry));

    CT_CircleData* circleData = CT_CircleData::staticCreateZAxisAlignedCircleDataFromPointCloudWithPreProcessing(pointCloudIndex,
                                                                                                                 &preProcessingAction);
    if(circleData == nullptr)
        return nullptr;


    /*double cosRotationZ = 1;
    double sinRotationZ = 0;*/

    double cosRotationZ = cos(-rz);
    double sinRotationZ = sin(-rz);

    // calcul les coordonnées du centre du cercle en fonction de la rotation et translation appliquées aux points
    Eigen::Vector3d cen = circleData->getCenter();

    // rotation inverse
    double xp = cen(0)*cosRotationZ - cen(1)*sinRotationZ;
    double yp = cen(0)*sinRotationZ + cen(1)*cosRotationZ;

    // translation inverse
    cen(0) = xp - preProcessingAction.getTranslationX();
    cen(1) = yp - preProcessingAction.getTranslationY();
    cen(2) = -preProcessingAction.getTranslationZ();

    // nouveau centre
    circleData->setCenter(cen);

    if(outCircleData != nullptr)
        *outCircleData = *circleData;

    CT_CylinderData* cylinderData = new CT_CylinderData(circleData->getCenter(),
                                                        vect_line,
                                                        circleData->getRadius(),
                                                        preProcessingAction.getZMax()-preProcessingAction.getZMin(),
                                                        direction.getRMSE(),
                                                        circleData->getError());

    delete circleData;

    return cylinderData;
}

