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

#ifndef CT_SCANNER_H
#define CT_SCANNER_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"  // Inherits from CT_abstractItemDrawableWithoutPointCloud
#include "ct_point.h"                                                           // Used to get rays from a scanner
#include "ct_itemdrawable/ct_beam.h"                                            // A scan creates some rays
#include "ct_itemdrawable/tools/drawmanager/ct_standardscannerdrawmanager.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

#include <memory>
#include <math.h>

/*! \def    SCANNER_EPSILON
            Used to avoid some instabilities : in some cases, we want values less than this constant (10e-5) become 0
            */
#define SCANNER_EPSILON 0.00001

/** \class  CT_Scanner
    \brief  This class represents a terrestrial LiDAR scanner.
            Angles are measured in radians and the theta/phi attributes are calculated on a spherical coordinates basis.
    \ingroup PluginShared_Items
    \todo   Un scanner est-il un item drawable ? (Affichage de sa position) ?
*/
class CTLIBSTRUCTUREADDON_EXPORT CT_Scanner : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Scanner, CT_AbstractItemDrawableWithoutPointCloud, Scan position)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
//********************************************//
//         Constructors/Destructors           //
//********************************************//
    /*!
    *  \brief Default constructor
    *
    *  Default constructor of the class
    *  Each attribute will be set to 0 or nullptr
    *  Each vector will be empty
    *  \warning The _zVector attribute is set to (0,0,1) by default
    *
    */
    CT_Scanner(int scanID = 0, bool clockWise = true);

    /**
     * @brief Constructor with custom shooting pattern
     * The CT_Scanner takes the ownership of the shooting pointer.
     */
    CT_Scanner(int scanID, CT_ShootingPattern* pattern);

    /*!
    *  \brief Constructor for scanner with theta phi shooting pattern
    *
    *  Constructor of the class
    *
    *  \param scanID : ID of the scan
    *  \param position : position of the scan in world coordinate system
    *  \param zVector : vertica of the scan : (0,0,1) by default
    *  \param hFov : horizontal field of view
    *  \param vFov : vertical field of view
    *  \param hRes : horizontal resolution (angle between two consecutive horizontal rays)
    *  \param vRes : vertical resolution
    *  \param initTheta : (horizontal) angle between the first ray and the Ox axis
    *  \param initPhi : (vertical) angle between the first vertical ray and the Oz axis
    *  \param radians : type of angle (radians or degrees), degrees by default
    */
    CT_Scanner(int scanID, const Eigen::Vector3d &position, const Eigen::Vector3d &zVector, double hFov, double vFov, double hRes, double vRes, double initTheta, double initPhi, bool clockWise, bool radians = false );


    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Min and Max coordinates (bounding box)
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_Scanner(const CT_Scanner& other);

    ~CT_Scanner();

//********************************************//
//                  Getters                   //
//********************************************//
    /*!
    *  \brief Getter of the class
    *
    *  \return Returns the ID of the scanner
    */
    inline int scanID() const { return _scanID; }

    /*!
    *  \brief Getter of the class
    *
    *  \return Returns the position of the scanner
    */
    inline Eigen::Vector3d position() const { return centerCoordinate(); }

    inline double positionX() const {return centerX();}
    inline double positionY() const {return centerY();}
    inline double positionZ() const {return centerZ();}

    /**
     * @brief Returns the shooting pattern
     */
    CT_ShootingPattern* shootingPattern() const;

//********************************************//
//                  Setters                   //
//********************************************//
    /*!
    *  \brief Setter of the class
    */
    inline void setID ( int scanID ) { _scanID = scanID; }

//***********************************************************************************//
//      Virtual/redefined methods from CT_AbstractItemDrawableWithoutPointCloud      //
//***********************************************************************************//

    CT_ITEM_COPY_IMP(CT_Scanner)

//********************************************//
//                   Tools                    //
//********************************************//
public :

    /*!
    *  \brief Provides access the (ith, jth) beam
    *
    *  \param i : horizontal index of the beam
    *  \param j : vertical index of the beam
    *  \param beam : Returns the (ith, jth) beam
    */
    void beam(int i, CT_Beam &beam) const;

private :
    int                             _scanID;        /*!< ID of the scan*/

    std::unique_ptr<CT_ShootingPattern>     m_shootingPattern;

    CT_DEFAULT_IA_BEGIN(CT_Scanner)
    CT_DEFAULT_IA_V2(CT_Scanner, CT_AbstractCategory::staticInitDataId(), &CT_Scanner::scanID, QObject::tr("ScanID"))
    CT_DEFAULT_IA_V2(CT_Scanner, CT_AbstractCategory::staticInitDataX(), &CT_Scanner::positionX, QObject::tr("PositionX"))
    CT_DEFAULT_IA_V2(CT_Scanner, CT_AbstractCategory::staticInitDataY(), &CT_Scanner::positionY, QObject::tr("PositionY"))
    CT_DEFAULT_IA_V2(CT_Scanner, CT_AbstractCategory::staticInitDataZ(), &CT_Scanner::positionZ, QObject::tr("PositionZ"))
    CT_DEFAULT_IA_END(CT_Scanner)

    const static CT_StandardScannerDrawManager CT_SCANNER_DRAW_MANAGER;
};

#endif // CT_SCANNER_H
