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

#ifndef CT_POINT_H
#define CT_POINT_H

#include <iostream>
#include <string>

#include <cmath>
#include "ct_coordinates/abstract/ct_abstractcoordinatesystem.h"

#include <Eigen/Core>

class CT_Point;
class CT_PointData;

class CTLIBCLOUDS_EXPORT CT_PointTools {
public:
    static CT_AbstractCoordinateSystem::realEx staticXExport(const CT_PointData &p, const size_t &globalIndex);
    static CT_AbstractCoordinateSystem::realEx staticYExport(const CT_PointData &p, const size_t &globalIndex);
    static CT_AbstractCoordinateSystem::realEx staticZExport(const CT_PointData &p, const size_t &globalIndex);
    static void staticXYZExport(const CT_PointData &p, const size_t &globalIndex, CT_AbstractCoordinateSystem::realEx &x, CT_AbstractCoordinateSystem::realEx &y, CT_AbstractCoordinateSystem::realEx &z);
    static CT_Point staticRealPoint(const CT_PointData &p, const size_t &globalIndex);
    static void staticRealPoint(const CT_PointData &p, CT_Point &cp, const size_t &globalIndex);
};

/**
 * @brief Point manipulate in step
 */
class CT_Point : public Eigen::Vector3d
{
public:

    /**
     * @brief Use this enum when you want to access data of the point : double x = point[CT_Point::X];
     */
    enum PVAR {
        X = 0,
        Y = 1,
        Z = 2
    };

    inline CT_Point(void):Eigen::Vector3d() {}
    typedef Eigen::Vector3d Base;
    // This constructor allows you to construct CT_Point from Eigen expressions
    template<typename OtherDerived>
    inline CT_Point(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Vector3d(other)
    { }
    // This method allows you to assign Eigen expressions to CT_Point
    template<typename OtherDerived>
    inline CT_Point & operator= (const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Base::operator=(other);
        return *this;
    }

    inline void setX(const double &x) { (*this)(0) = x; }
    inline void setY(const double &y) { (*this)(1) = y; }
    inline void setZ(const double &z) { (*this)(2) = z; }
    inline void setValues(const double &x, const double &y, const double &z) { setX(x); setY(y); setZ(z); }
};

/**
 * @brief Internal point that was shifted by an offset to be ok for opengl : so we don't loose precision
 *        when drawing
 */
class CT_PointData : public Eigen::Vector3f
{
public:
    /**
     * @brief Use this enum when you want to access data of the point : float x = point[CT_PointData::X];
     */
    enum PVAR {
        X = 0,
        Y = 1,
        Z = 2
    };

    inline CT_PointData() : Eigen::Vector3f() {}
    typedef Eigen::Vector3f Base;
    // This constructor allows you to construct CT_Point from Eigen expressions
    template<typename OtherDerived>
    inline CT_PointData(const Eigen::MatrixBase<OtherDerived>& other)
        : Eigen::Vector3f(other)
    { }
    // This method allows you to assign Eigen expressions to CT_Point
    template<typename OtherDerived>
    inline CT_PointData & operator= (const Eigen::MatrixBase <OtherDerived>& other)
    {
        this->Base::operator=(other);
        return *this;
    }

    /**
     * @brief Returns the point converted to it's real coordinates
     * @param globalIndex : the global index of the point in the cloud
     */
    inline CT_Point realPoint(const size_t &globalIndex) const
    {
        return CT_PointTools::staticRealPoint(*this, globalIndex);
    }

    /**
     * @brief Returns the point converted to it's real coordinates
     * @param globalIndex : the global index of the point in the cloud
     */
    inline void realPoint(CT_Point &p, const size_t &globalIndex) const
    {
        return CT_PointTools::staticRealPoint(*this, p, globalIndex);
    }
};

inline CT_Point createCtPoint(const double &x = 0, const double &y = 0, const double &z = 0)
{
    CT_Point result;
    result(0) = x; result(1) = y; result(2) = z;
    return result;
}

inline CT_PointData createCtPointData(const float &x = 0, const float &y = 0, const float &z = 0)
{
    CT_PointData result;
    result(0) = x; result(1) = y; result(2) = z;
    return result;
}

inline void copyCtPointData( CT_PointData &dest, const CT_PointData &src)
{
    dest(0) = src(0);
    dest(1) = src(1);
    dest(2) = src(2);
}

#endif // CT_POINT_H
