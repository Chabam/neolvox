/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

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

#ifndef CT_LINEDATA_H
#define CT_LINEDATA_H

#include "ct_shapedata.h"

#include "ct_defines.h"

class CT_AbstractGeometricalItem;

class CTLIBSTRUCTUREADDON_EXPORT CT_LineData : public CT_ShapeData
{
    using SuperClass = CT_ShapeData;

public:
    CT_LineData();
    CT_LineData(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2);
    CT_LineData(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, double error, int n);
    CT_LineData(const CT_LineData& other) = default;

    const Eigen::Vector3d& getP1() const;
    const Eigen::Vector3d& getP2() const;
    double getError() const;

    double x1() const;
    double y1() const;
    double z1() const;

    double x2() const;
    double y2() const;
    double z2() const;

    double length() const;

    inline int n() const {return _n_points;}
    inline double getMSE() const {return _error;}
    inline double getRMSE() const
    {
        if (n() < 2) {return 0;}
        return sqrt(getMSE());
    }

    bool intersectionWithRect3D  (double plan_x, double plan_y, double plan_z, Eigen::Vector3d& vect_plan, double* xi, double* yi, double* zi);

    virtual void getBoundingBox(Eigen::Vector3d& min, Eigen::Vector3d& max) const override;


    CT_SHAPEDATA_CLONE_IMP(CT_LineData)

    /**
      * \brief Retourne les donnes d'une ligne 3D (rgression linaire)  partir du nuage de points passé en paramtre.
      *
      * \return nullptr si le nombre de points est infrieur  2.
      */
    static CT_LineData* staticCreateLineDataFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex);

    static CT_LineData* staticCreateLineDataFromPointCloud(const QList<Eigen::Vector3d>& l_gp, bool computeError = true);

    static CT_LineData* staticCreateLineDataFromItemCenters(const QList<const CT_AbstractGeometricalItem *> &items);

private:
    Eigen::Vector3d     _p1;
    Eigen::Vector3d     _p2;
    double              _error;
    int                 _n_points;
};

#endif // CT_LINEDATA_H
