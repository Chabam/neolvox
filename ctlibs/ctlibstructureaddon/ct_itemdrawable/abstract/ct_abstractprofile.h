/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Metiers (ENSAM), Cluny, France.
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

#ifndef CT_ABSTRACTPROFILE_H
#define CT_ABSTRACTPROFILE_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"

#include <Eigen/Core>

/*!
 * \class CT_AbstractProfile
 * \ingroup PluginShared_Items
 * \brief <b>Commun abstract base of all templated CT_AbstractProfile<type> </b>
 *
 * It's usefull to manage generically a profile, without knowing its template type
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractProfile : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractProfile, CT_AbstractItemDrawableWithoutPointCloud, Profile)

public:
    CT_AbstractProfile() = default;

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
    CT_AbstractProfile(const CT_AbstractProfile& other) = default;

    /*!
     * \brief Return a [0;1] value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A double value between 0 (min value) and 1 (max value)
     */
    virtual double ratioValueAtIndex(const size_t &index) const = 0;

    /*!
     * \brief Return a double value for any type (or NAN for NA)
     * \param index index in the grid
     * \return A double value
     */
    virtual double valueAtIndexAsDouble(const size_t &index) const = 0;


    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t &index) const = 0;

    /*!
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const = 0;

    /*!
     * \brief Return index for specified length
     * \param l Length from the profil origin
     * \param index Returned index
     * \return false if length invalid
     */
    inline bool indexForLength(const double &l, size_t &index) const
    {
        if (l < 0 || l > maxLength()) {return false;}
        if (l == maxLength())
        {
            index =  _dim - 1;
        } else {
            index = (size_t) floor(l / _res);
        }
        return true;
    }

    /*!
     * \brief Return index for specified length
     * \param l Length from the profil origin
     * \param index Returned index
     * \return false if length invalid
     */
    inline bool indexForXYZ(const double &x, const double &y, const double &z, size_t &index) const
    {
        double length = _direction.x()*(x - minX()) + _direction.y()*(y - minY()) + _direction.z()*(z - minZ());
        return indexForLength(length, index);
    }

    /*!
     * \brief Generic setter taking a double as input value
     * \param index Index of cell to modify
     * \param value Double value to cast in effective type
     */
    virtual void setValueAtIndexFromDouble(const size_t &index, const double &value) = 0;

    /*!
     * \brief Compute min and max values
     */
    virtual void computeMinMax() = 0;

    /**
      * \brief Gives the resolution
      * \return Resolution (m)
      */
    inline double resolution() const {return _res;}

    /**
     * \brief getMinCoordinates
     * \param min Min coordinates of the grid (bottom left corner)
     */
    inline void getMinCoordinates(Eigen::Vector3d &min) const
    {
        min(0) = minX();
        min(1) = minY();
        min(2) = minZ();
    }

    /**
     * \brief getMaxCoordinates
     * \param max Max coordinates of the grid (upper right corner)
     */
    inline void getMaxCoordinates(Eigen::Vector3d &max) const
    {
        max(0) = maxX();
        max(1) = maxY();
        max(2) = maxZ();
    }


    /*!
     * \brief Profil length on along the axis
     * \return Max Lenght
     */
    inline double maxLength() const {return _maxLength;}


    /*!
     * \brief Total number of cells for the grid
     * \return Number of cells
     */
    inline size_t nCells() const {return _dim;}

    /*!
     * \brief Give length on the profil axis For specified Index
     * \param index
     * \return length (could be outside of the profil range)
     */
    inline double lengthForIndex(const size_t &index) const
    {
        if (index >= _dim) {return maxLength();}
        return (index * _res) + _res / 2.0;
    }

    /*!
     * \brief Gives the X coordinate of the center of cells at index
     * \return X coordinate
     */
    inline double getCellCenterX(const size_t &index) const
    {
        double length = lengthForIndex(index) ;
        return minX() + length * _direction.x();
    }

    /*!
     * \brief Gives the Y coordinate of the center of cells at index
     * \return Y coordinate
     */
    inline double getCellCenterY(const size_t &index) const
    {
        double length = lengthForIndex(index) ;
        return minY() + length * _direction.y();
    }

    /*!
     * \brief Gives the Z coordinate of the center of cells at index
     * \return Z coordinate
     */
    inline double getCellCenterZ(const size_t &index) const
    {
        double length = lengthForIndex(index) ;
        return minZ() + length * _direction.z();
    }

    /*!
     * \brief Gives the (X,Y,Z) coordinates of the center of cell at index
     */
    inline bool getCellCenterXYZ(const size_t &index, Eigen::Vector3d &center) const
    {
        if (index >= _dim) {return false;}

        const double length = lengthForIndex(index) ;

        center = minCoordinates() + length*_direction;

        return true;
    }

    /*!
     * \brief getCellCoordinates Give min and max coordinates of a cell (on the profil axis)
     * \param index Index of the cell
     * \param bottom  Min coordinates
     * \param top Max coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCoordinates(const size_t &index, Eigen::Vector3d &bottom, Eigen::Vector3d &top) const
    {
        if (index >= _dim) {return false;}

        double length = lengthForIndex(index) ;
        double lengthBottom = length - (_res/2.0);
        double lengthTop = length + (_res/2.0);

        bottom = minCoordinates() + lengthBottom*_direction;
        top    = minCoordinates() + lengthTop*_direction;

        return true;
    }

    inline const Eigen::Vector3d &getDirection() const {return _direction;}

    inline size_t getDim() const {return _dim;}


protected:
    size_t          _dim;       /*!< Nombre de cases du profile */
    double          _res;       /*!< Resolution du profile (taille d'une case) */
    double          _maxLength; /*!< Longueur du profil le long de l'axe */

    Eigen::Vector3d _direction; /*!< Direction of the profil axis */

};

#endif // CT_ABSTRACTPROFILE_H
