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

#ifndef CT_BEAM_H
#define CT_BEAM_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardbeamdrawmanager.h"
#include "ct_itemdrawable/tools/scanner/ct_shot.h"

/*! \def    EPSILON_INTERSECTION_RAY
            Redefinition of the zero for the ray-box intersection algorithm */
#define EPSILON_INTERSECTION_RAY 0.000001    // 10^-6

/** \class  CT_Beam
    \brief  This class represents a 3D beam described in a parametric manner. It inherits from the CT_AbstractItemDrawableWithoutPointCloud class so it can be drawn.
            Each ray has a normalized direction.
    \ingroup PluginShared_Items
*/
class CTLIBSTRUCTUREADDON_EXPORT CT_Beam : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Beam, CT_AbstractItemDrawableWithoutPointCloud, Beam)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    /*!
    *  \brief Default constructor
    *
    *  Default constructor of the class
    *  Each attribute will be set to 0 or nullptr
    *  Each vector will be empty
    *  \warning All will be set to 0 or nullptr exept the direction which is (1,0,0) by default (a ray can not have a direction equals to (0,0,0) ).
    *
    */
    CT_Beam();

    /*!
    *  \brief Constructor
    *
    *  Constructor of the class
    *
    *  \param origin : origin of the ray
    *  \param direction : direction of the ray
    */
    CT_Beam(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction);

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
     *          - Origin and direction (shot)
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_Beam(const CT_Beam& other) = default;

//********************************************//
//                Operators                   //
//********************************************//

    /*!
    *  \brief Access operator
    *
    *  \param t : time spent by the ray
    *
    *  \return Returns the point of the ray at a given time
    */
    inline Eigen::Vector3d operator() (double t) const
    {
        return m_shot.origin() + m_shot.direction()*t;
    }

//********************************************//
//                  Getters                   //
//********************************************//
    /*!
    *  \brief Getter of the class
    *
    *  \return Returns the origin of the ray
    */
    inline Eigen::Vector3d origin () const { return m_shot.origin(); }

    /*!
    *  \brief Getter of the class
    *
    *  \return Returns the direction of the ray
    */
    inline Eigen::Vector3d direction () const { return m_shot.direction(); }

    inline CT_Shot* shot() const { return &const_cast<CT_Shot&>(m_shot); }

//********************************************//
//                  Setters                   //
//********************************************//
    /*!
    *  \brief Setter of the class
    */
    inline void setOrigin ( const Eigen::Vector3d& origin )
    {
        m_shot.setOrigin(origin);
    }

    /*!
    *  \brief Setter of the class
    */
    inline void setDirection ( const Eigen::Vector3d& direction )
    {
        Q_ASSERT( !(direction(0) == 0 && direction(1) == 0 && direction(2) == 0) );
        if (direction(0) == 0 && direction(1) == 0 && direction(2) == 0) {qDebug() << "CT_Beam::setDirection" << ", " << "direction(0) == 0 && direction(1) == 0 && direction(2) == 0";}
        m_shot.setDirection(direction);
    }

//********************************************//
//        Coordinate system conversion        //
//********************************************//

//********************************************//
//                   Tools                    //
//********************************************//
    /*!
    *  \brief Calculates the intersection between a grid and a ray
    *
    *  This method uses the algorithm from Williams et al.
    *  ****************************************************************
    *  Williams, A., Barrus, S., & Morley, R. (2005).
    *  An efficient and robust ray-box intersection algorithm.
    *  ACM SIGGRAPH 2005
    *  1-4.
    *  *****************************************************************
    *
    *  \param bot : bottom left corner of the box to intersect
    *  \param top : top right corner of the box to intersect
    *  \param near : output, nearest 3D point of the intersection
    *  \param far : output, farest 3D point intersection
    *
    *  \return Returns false if no intersection was found, true else
    */
    bool intersect ( const Eigen::Vector3d& bot, const Eigen::Vector3d& top, Eigen::Vector3d& nearP, Eigen::Vector3d& farP ) const;

    /*!
    *  \brief Test the intersection between a grid and a ray
    *
    *  This method uses the algorithm from Williams et al.
    *  ****************************************************************
    *  Williams, A., Barrus, S., & Morley, R. (2005).
    *  An efficient and robust ray-box intersection algorithm.
    *  ACM SIGGRAPH 2005
    *  1-4.
    *  *****************************************************************
    *
    *  \param bot : bottom left corner of the box to intersect
    *  \param top : top right corner of the box to intersect
    *
    *  \return Returns false if no intersection was found, true else
    */
    bool intersect (const Eigen::Vector3d &bot, const Eigen::Vector3d &top ) const;

    /*!
     * \brief Utilitary function for intersect
     */
    bool updateIntervals(const double &bottomCorner,const double &upperCorner,const double &origin,const double &direction,double &t0,double &t1) const;

//***********************************************************************************//
//      Virtual/redefined methods from CT_AbstractItemDrawableWithoutPointCloud      //
//***********************************************************************************//

    CT_ITEM_COPY_IMP(CT_Beam)

private :
    CT_Shot m_shot;

    const static CT_StandardBeamDrawManager BEAM_DRAW_MANAGER;      /*!< Static attribute of the class :
                                                                   *  Draw manager of a ray.
                                                                   */
};

#endif // CT_BEAM_H
