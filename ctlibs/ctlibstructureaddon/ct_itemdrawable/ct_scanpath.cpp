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

#include "ct_scanpath.h"

#include <limits>

const CT_StandardScanPathDrawManager CT_ScanPath::SCANPATH_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_ScanPath)

CT_ScanPath::CT_ScanPath()
{
    _sorted = false;
    _minGPSTime = std::numeric_limits<double>::max();
    _maxGPSTime = -std::numeric_limits<double>::max();
}

CT_ScanPath::CT_ScanPath(QString name) : SuperClass(),
    _sorted(false),
    _minGPSTime(std::numeric_limits<double>::max()),
    _maxGPSTime(-std::numeric_limits<double>::max())
{
    _name = name;
    setDisplayableName(_name);
    setBoundingBox(std::numeric_limits<double>::max(),
                   std::numeric_limits<double>::max(),
                   std::numeric_limits<double>::max(),
                   -std::numeric_limits<double>::max(),
                   -std::numeric_limits<double>::max(),
                   -std::numeric_limits<double>::max());

    setBaseDrawManager(&SCANPATH_DRAW_MANAGER);
}

CT_ScanPath::CT_ScanPath(const CT_ScanPath &other)
{
    _name = other._name;
    setDisplayableName(_name);

    _sorted = other._sorted;
    _pathPoints = other._pathPoints;
    _minGPSTime = other._minGPSTime;
    _maxGPSTime = other._maxGPSTime;

    Eigen::Vector3d min, max;
    other.boundingBox(min, max);

    setBoundingBox(min(0), min(1), min(2), max(0), max(1), max(2));

    setBaseDrawManager(&SCANPATH_DRAW_MANAGER);
}

QString CT_ScanPath::getPathName() const
{
    return _name;
}

void CT_ScanPath::addPathPoint(double gpsTime, double x, double y, double z, double h, double r, double p)
{
    _pathPoints.append(PathPoint(gpsTime, Eigen::Vector3d(x,y,z), Eigen::Vector3d(h,r,p)));
    _sorted = false;

    Eigen::Vector3d min, max;
    boundingBox(min, max);

    if (x < min(0)) {min(0) = x;}
    if (y < min(1)) {min(1) = y;}
    if (z < min(2)) {min(2) = z;}
    if (gpsTime < _minGPSTime) {_minGPSTime = gpsTime;}

    if (x > max(0)) {max(0) = x;}
    if (y > max(1)) {max(1) = y;}
    if (z > max(2)) {max(2) = z;}
    if (gpsTime > _maxGPSTime) {_maxGPSTime = gpsTime;}

    setBoundingBox(min, max);
}

void CT_ScanPath::addPathPoint(double gpsTime, const Eigen::Vector3d &position)
{
    addPathPoint(gpsTime, position(0), position(1), position(2), 0, 0, 0);
}

void CT_ScanPath::addPathPoint(double gpsTime, const Eigen::Vector3d &position, const Eigen::Vector3d &orientation)
{
    addPathPoint(gpsTime, position(0), position(1), position(2), orientation(0), orientation(1), orientation(2));
}

bool CT_ScanPath::isInScanPath(double gpsTime) const
{
    if (gpsTime < _minGPSTime) {return false;}
    if (gpsTime > _maxGPSTime) {return false;}
    return true;
}

Eigen::Vector3d CT_ScanPath::getPathPointForGPSTime(double gpsTime)
{
    if (!_sorted)
    {
        _sorted = true;
        std::sort(_pathPoints.begin(), _pathPoints.end(), sortPathPointsByGPSTime);
    }

    double lastGPSTime = -1;
    Eigen::Vector3d lastPoint(0,0,0);
    int lastIndex = -1;

    double currentGPSTime = -1;
    Eigen::Vector3d currentPoint(0,0,0);

    // Début d'optimisation
    bool found = false;
    for (int i = 0 ; i < _pathPoints.size() && !found; i += 10000)
    {
        const PathPoint& pathpoint = _pathPoints.at(i);

        if (pathpoint._gpsTime < gpsTime) {
            currentGPSTime = pathpoint._gpsTime;
            currentPoint   = pathpoint._position;
            lastIndex = i;
        } else {
            found = true;
        }
    }

    found = false;
    for (int i = lastIndex + 1 ; i < _pathPoints.size() && !found; i += 1000)
    {
        const PathPoint& pathpoint = _pathPoints.at(i);

        if (pathpoint._gpsTime < gpsTime) {
            currentGPSTime = pathpoint._gpsTime;
            currentPoint   = pathpoint._position;
            lastIndex = i;
        } else {
            found = true;
        }
    }

    found = false;
    for (int i = lastIndex + 1 ; i < _pathPoints.size() && !found; i += 100)
    {
        const PathPoint& pathpoint = _pathPoints.at(i);

        if (pathpoint._gpsTime < gpsTime) {
            currentGPSTime = pathpoint._gpsTime;
            currentPoint   = pathpoint._position;
            lastIndex = i;
        } else {
            found = true;
        }
    }
    // Fin d'optimisation


    found = false;
    for (int i = lastIndex + 1 ; i < _pathPoints.size() && !found; i++)
    {
        const PathPoint& pathpoint = _pathPoints.at(i);

        lastGPSTime = currentGPSTime;
        lastPoint = currentPoint;

        currentGPSTime = pathpoint._gpsTime;
        currentPoint   = pathpoint._position;

        if (currentGPSTime > gpsTime) {found = true;}
    }

    if (lastGPSTime == -1 || !found)
    {
        return currentPoint;
    } else {
        Eigen::Vector3d direction = currentPoint - lastPoint;
        double ratio = (gpsTime - lastGPSTime) / (currentGPSTime - lastGPSTime);

        Eigen::Vector3d vec = lastPoint + direction*ratio;
        return vec;
    }
}
