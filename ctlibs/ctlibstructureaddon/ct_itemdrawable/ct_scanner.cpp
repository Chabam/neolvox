/****************************************************************************

 Copyright (C) 2012-2012 Université de Sherbrooke, Québec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA

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

#include "ct_scanner.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "ct_itemdrawable/tools/scanner/ct_thetaphishootingpattern.h"

// Initializing the draw manager
const CT_StandardScannerDrawManager CT_Scanner::CT_SCANNER_DRAW_MANAGER;

CT_DEFAULT_IA_INIT(CT_Scanner)

CT_Scanner::CT_Scanner(int scanID, bool clockWise) :
    SuperClass(),
    _scanID(scanID)
{
    setCenterX(0);
    setCenterY(0);
    setCenterZ(0);
    CT_ThetaPhiShootingPattern* p = new CT_ThetaPhiShootingPattern();
    p->setClockWise(clockWise);

    m_shootingPattern.reset(p);

    setBaseDrawManager(&CT_SCANNER_DRAW_MANAGER);
}

CT_Scanner::CT_Scanner(int scanID,
                       CT_ShootingPattern* pattern) :
    SuperClass(),
    _scanID(scanID)
{
    Q_ASSERT(pattern != nullptr);
    if (pattern == nullptr) {qDebug() << "CT_Scanner::CT_Scanner" << ", " << "pattern == nullptr";}

    const Eigen::Vector3d& center = pattern->centerCoordinate();

    setCenterX(center.x());
    setCenterY(center.y());
    setCenterZ(center.z());

    m_shootingPattern.reset(pattern);

    setBaseDrawManager(&CT_SCANNER_DRAW_MANAGER);
}

CT_Scanner::CT_Scanner(int scanID,
                       const Eigen::Vector3d &origin,
                       const Eigen::Vector3d &zVector,
                       double hFov,
                       double vFov,
                       double hRes,
                       double vRes,
                       double initTheta,
                       double initPhi,
                       bool clockWise,
                       bool radians) :
    SuperClass(),
    _scanID(scanID)
{
    _scanID = scanID;

    setCenterX(origin.x());
    setCenterY(origin.y());
    setCenterZ(origin.z());

    m_shootingPattern.reset(new CT_ThetaPhiShootingPattern(origin,
                                                           hFov,
                                                           vFov,
                                                           hRes,
                                                           vRes,
                                                           initTheta,
                                                           initPhi,
                                                           zVector,
                                                           clockWise,
                                                           radians));

    setBaseDrawManager(&CT_SCANNER_DRAW_MANAGER);
}

CT_Scanner::CT_Scanner(const CT_Scanner& other) : SuperClass(other),
    _scanID(other._scanID),
    m_shootingPattern(other.m_shootingPattern.get()->clone())
{
}

CT_Scanner::~CT_Scanner()
{
    m_shootingPattern.release();
}

CT_ShootingPattern* CT_Scanner::shootingPattern() const
{
    return m_shootingPattern.get();
}

void CT_Scanner::beam(int i, CT_Beam& beam) const
{
    CT_ShootingPattern* p = shootingPattern();

    if(p == nullptr)
        return;

    const CT_Shot& shot = p->shotAt(i);

    beam.setOrigin(shot.origin());
    beam.setDirection(shot.direction());
}
