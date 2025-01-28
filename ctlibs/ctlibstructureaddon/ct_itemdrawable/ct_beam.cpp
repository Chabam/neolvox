/****************************************************************************

 Copyright (C) 2012-2012 Universite de Sherbrooke, Quebec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA
 Adapted by Alexandre Piboule for Computree 2.0

 This file is part of Computree version 2.0.

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Computree.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#include "ct_beam.h"
#include <limits>

const CT_StandardBeamDrawManager CT_Beam::BEAM_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_Beam)

CT_Beam::CT_Beam() : SuperClass(),
    m_shot(CT_Shot(true))
{
    setCenterX(0);
    setCenterY(0);
    setCenterZ(0);

    setBaseDrawManager(&BEAM_DRAW_MANAGER);
}

CT_Beam::CT_Beam(const Eigen::Vector3d& origin,
                 const Eigen::Vector3d& direction) : SuperClass(),
    m_shot(origin, direction.normalized())
{
    Q_ASSERT( !(direction(0) == 0 && direction(1) == 0 && direction(2) == 0) );
    if (direction(0) == 0 && direction(1) == 0 && direction(2) == 0) {qDebug() << "CT_Beam::CT_Beam" << ", " << "direction(0) == 0 && direction(1) == 0 && direction(2) == 0";}

    setCenterCoordinate(origin);

    setBaseDrawManager(&BEAM_DRAW_MANAGER);
}

bool CT_Beam::intersect(const Eigen::Vector3d& bot, const Eigen::Vector3d& top, Eigen::Vector3d &nearP, Eigen::Vector3d &farP) const
{
    double t0 = 0;
    double t1 = std::numeric_limits<double>::max();

    if (!updateIntervals(bot(0), top(0), m_shot.origin()(0), m_shot.direction()(0), t0, t1)) {return false;}
    if (!updateIntervals(bot(1), top(1), m_shot.origin()(1), m_shot.direction()(1), t0, t1)) {return false;}
    if (!updateIntervals(bot(2), top(2), m_shot.origin()(2), m_shot.direction()(2), t0, t1)) {return false;}


    nearP = m_shot.origin() + m_shot.direction()*t0;
    farP  = m_shot.origin() + m_shot.direction()*t1;

    return true;
}

bool CT_Beam::intersect(const Eigen::Vector3d& bot, const Eigen::Vector3d& top) const
{
    double t0 = 0;
    double t1 = std::numeric_limits<double>::max();

    if (!updateIntervals(bot(0), top(0), m_shot.origin()(0), m_shot.direction()(0), t0, t1)) {return false;}
    if (!updateIntervals(bot(1), top(1), m_shot.origin()(1), m_shot.direction()(1), t0, t1)) {return false;}
    if (!updateIntervals(bot(2), top(2), m_shot.origin()(2), m_shot.direction()(2), t0, t1)) {return false;}

    return true;
}

bool CT_Beam::updateIntervals(const double &bot, const double &top, const double &origin, const double &direction, double &t0, double &t1) const
{
    // Update interval for bounding box slab
    double invRayDir = 1.f / direction;
    double tNear = (bot - origin) * invRayDir;
    double tFar  = (top - origin) * invRayDir;

    // Update parametric interval from slab intersection $t$s
    if (tNear > tFar) std::swap(tNear, tFar);

    t0 = tNear > t0 ? tNear : t0;
    t1 = tFar  < t1 ? tFar  : t1;

    if (t0 > t1 && t0 - t1 > EPSILON_INTERSECTION_RAY ) // t0 being always > t1, (t0-t1) is always positive
    {
        return false;
    }
    return true;
}
