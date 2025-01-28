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

#ifndef CT_SCANPATH_H
#define CT_SCANPATH_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardscanpathdrawmanager.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_ScanPath : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_ScanPath, CT_AbstractItemDrawableWithoutPointCloud, Scan Path)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:

    struct PathPoint {
    public:
        PathPoint(double gpsTime, Eigen::Vector3d position, Eigen::Vector3d orientation)
        {
            _gpsTime = gpsTime;
            _position = position;
            _orientation = orientation;
        }

        double          _gpsTime;
        Eigen::Vector3d _position; // (x;y;z)
        Eigen::Vector3d _orientation; //(heading, roll, pitch)
    };

    static bool sortPathPointsByGPSTime (PathPoint& a, PathPoint& b) {
        return (a._gpsTime < b._gpsTime);
    }

    CT_ScanPath();
    CT_ScanPath(QString name);
    CT_ScanPath(const CT_ScanPath& other);

    QString getPathName() const;

    void addPathPoint(double gpsTime, double x, double y, double z, double h = 0, double r = 0, double p = 0);
    void addPathPoint(double gpsTime, const Eigen::Vector3d &position);
    void addPathPoint(double gpsTime, const Eigen::Vector3d &position, const Eigen::Vector3d &orientation);

    bool isInScanPath(double gpsTime) const;

    Eigen::Vector3d getPathPointForGPSTime(double gpsTime);

    const QList<CT_ScanPath::PathPoint>& getPath() const {return _pathPoints;}

    CT_ITEM_COPY_IMP(CT_ScanPath)

private:
    QString                         _name;
    bool                            _sorted;
    QList<PathPoint>                _pathPoints;
    double                          _minGPSTime;
    double                          _maxGPSTime;

    const static CT_StandardScanPathDrawManager  SCANPATH_DRAW_MANAGER;
};

#endif // CT_SCANPATH_H
