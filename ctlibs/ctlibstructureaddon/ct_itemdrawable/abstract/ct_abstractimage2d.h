/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#ifndef CT_ABSTRACTIMAGE2D_H
#define CT_ABSTRACTIMAGE2D_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"

#include <Eigen/Core>

/*!
 * \class CT_AbstractImage2D
 * \ingroup PluginShared_Items
 * \brief <b>Commun abstract base of all templated CT_Grid2D<type> </b>
 *
 * It's usefull to manage generically a grid, without knowing it template type
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractImage2D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractImage2D, CT_AbstractItemDrawableWithoutPointCloud, Raster)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_AbstractImage2D();

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
    CT_AbstractImage2D(const CT_AbstractImage2D& other) = default;

    /*!
     * \brief Return a [0;1] value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A double value between 0 (min value) and 1 (max value)
     */
    virtual double ratioValueAtIndex(const size_t index) const = 0;

    /*!
     * \brief Return a double value for any type (or NAN for NA)
     * \param index index in the grid
     * \return A double value
     */
    virtual double valueAtIndexAsDouble(const size_t index) const = 0;


    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t index) const = 0;

    /*!
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const = 0;

    /*!
     * \brief return na value as a double
     *
     */
    virtual double NAAsDouble() const = 0;

    virtual double minValueAsDouble() const = 0;
    virtual double maxValueAsDouble() const = 0;

    /*!
     * \brief Compute index for given column, row
     * \param x Column
     * \param y Row
     * \param returnedIndex Returned index
     * \return true if index is valid
     */
    inline bool index(const int x, const int y, size_t &returnedIndex) const
    {
        if ((x < 0) || (x >= _dimx) || (y < 0) || (y >= _dimy))
        {
            return false;
        }
        else
        {
            returnedIndex = size_t(y)*size_t(_dimx) + size_t(x);
            return true;
        }
    }

    /*!
     * \brief Return column for specified colCoord coordinate
     * \param xCoord Column coordinate
     * \param xcolumn Column number
     * \return true if index is valid
     */
    inline bool xcol(const double xCoord, int &xcolumn) const
    {
        if (xCoord < minXCoord() || xCoord > maxXCoord()) {return false;}

        xcolumn = int(floor((xCoord - minXCoord()) / _res));

        if (xcolumn < 0) {return false;}
        if (xcolumn > (_dimx - 1)) {return false;}

        return true;
    }

    /*!
     * \brief Return row for specified linCoord coordinate
     * \param yCoord Row coordinate
     * \param yrow number
     * \return true if index is valid
     */
    inline bool lin(const double yCoord, int &yrow) const
    {
        if (yCoord < minYCoord() || yCoord > maxYCoord()) {return false;}

        yrow = _dimy - 1 - int(floor((yCoord - minYCoord()) / _res));

        if (yrow < 0) {return false;}
        if (yrow > (_dimy - 1)) {return false;}

        return true;
    }

    /*!
     * \brief Compute index for given (xCoord, yCoord) coordinate
     * \param xCoord column coordinate
     * \param yCoord Row coordinate
     * \param index Returned index
     * \return true if returnedIndex is valid
     */
    inline bool indexAtCoords(const double xCoord, const double yCoord, size_t &returnedIndex) const
    {
        int colx, liny;
        if (!xcol(xCoord, colx) || !lin(yCoord, liny)) {return false;}

        return index(colx, liny, returnedIndex);
    }

    /*!
     * \brief indexToGrid Convert index in (coly, lin) grid coordinates
     * \param index Index of the cell
     * \param x Resulting column of the cell
     * \param y Resulting row of the cell
     * \return true if the index is in the grid
     */
    inline bool indexToGrid(const size_t index, int &x, int &y) const
    {
        if (index >= nCells()) {return false;}

        y = int(index / size_t(_dimx));
        x = int(index % size_t(_dimx));

        return true;
    }

    /*!
     * \brief Compute min and max values
     */
    virtual void computeMinMax() = 0;

    /*!
     * \brief Gives the default z level of the raster
     * \return Default level
     */
    inline double level() const { return _level; }

    /*!
     * \brief Set the z level of the raster
     * \return Default level
     */
    inline void setlevel(double level) { _level = level; }

    /**
      * \brief Gives the resolution
      * \return Resolution (m)
      */
    inline double resolution() const {return _res;}

    /**
     * \brief getMinCoordinates
     * \param min Min coordinates of the grid (bottom left corner)
     */
    inline void getMinCoordinates(Eigen::Vector2d &min) const
    {
        min(0) = minXCoord();
        min(1) = minYCoord();
    }

    /**
     * \brief getMaxCoordinates
     * \param max Max coordinates of the grid (upper right corner)
     */
    inline void getMaxCoordinates(Eigen::Vector2d &max) const
    {
        max(0) = maxX();
        max(1) = maxY();
    }

    /**
      * \brief Gives the number of columns
      * \return Number of columns
      */
    inline int xdim() const {return _dimx;}

    /**
      * \brief Gives the number of rows
      * \return Number of rows
      */
    inline int ydim() const {return _dimy;}

    /**
      * \brief Gives the minimum column coordinate
      * \return minimum column coordinate
      */
    inline double minXCoord() const {return _minXCoord;}

    /**
      * \brief Gives the minimum row coordinate
      * \return minimum row coordinate
      */
    inline double minYCoord() const {return _minYCoord;}

    /**
      * \brief Gives the maximum column coordinate
      * \return maximum column coordinate
      */
    inline double maxXCoord() const {return _minXCoord + _dimx*_res;}

    /**
      * \brief Gives the maximum row coordinate
      * \return maximum row coordinate
      */
    inline double maxYCoord() const {return _minYCoord + _dimy*_res;}

    /*!
     * \brief Total number of cells for the raster
     * \return Number of cells
     */
    inline size_t nCells() const {return size_t(_dimx)*size_t(_dimy);}

    /*!
     * \brief Gives the column coordinate of the center of cells of column col
     * \param x Column, first one is 0
     * \return Column coordinate
     */
    inline double getCellCenterColCoord(const int x) const {return minXCoord() + x*_res + _res/2;}

    /*!
     * \brief Gives the row coordinate of the center of cells of row lin
     * \param y Row, first one is 0
     * \return Row coordinate
     */
    inline double getCellCenterLinCoord(const int y) const {return maxYCoord() - y*_res - _res/2;}

    /*!
     * \brief getCellCoordinates Give min and max coordinates of a cell
     * \param index Index of the cell
     * \param bottom  Min coordinates
     * \param top Max coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCoordinates(const size_t index, Eigen::Vector2d &bottom, Eigen::Vector2d &top) const
    {
        int col, lin;
        if (!indexToGrid(index, col, lin)) {return false;}
        bottom(0) = minXCoord() + col*_res;
        bottom(1) = maxYCoord() - (lin + 1)*_res;

        top(0) = bottom(0) + _res;
        top(1) = bottom(1) + _res;
        return true;
    }

    inline bool getCellCenterCoordinates(const int x, const int y, Eigen::Vector3d &center) const
    {
        if ((x < 0) || (x >= xdim()) || (y < 0) || (y >= ydim())) {return false;}

        center(0) = minXCoord() + x*_res + _res/2.0;
        center(1) = maxYCoord() - y*_res - _res/2.0;
        center(2) = _level;

        return true;
    }

    inline bool getCellCenterCoordinates(const size_t index, Eigen::Vector3d &center) const
    {
        int col, lin;
        if (!indexToGrid(index, col, lin)) {return false;}

        center(0) = minXCoord() + col*_res + _res/2.0;
        center(1) = maxYCoord() - lin*_res - _res/2.0;
        center(2) = _level;

        return true;
    }



    /*!
     * \brief Get the left corner coordinates of the cell defined by (x, y)
     * \param x Column
     * \param y Row
     * \param bottom Output coordinates
     * \return A Eigen::Vector2d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCorner(const int x, const int y, Eigen::Vector2d &bottom) const
    {
        if ((x < 0) || (x >= xdim()) || (y < 0) || (y >= ydim())) {return false;}
        bottom(0) = minXCoord() + x*_res;
        bottom(1) = maxYCoord() - (y + 1)*_res;

        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell containing by (xCoord, yCoord)
     * \param xCoord column coordinate
     * \param yCoord Row coordinate
     * \param bottom Output coordinates
     * \return A Eigen::Vector2d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCornerAtCoords(const double xCoord, const double yCoord, Eigen::Vector2d &bottom) const
    {

        return getCellBottomLeftCorner(int(floor((xCoord - minXCoord()) / _res)),
                                       int(floor((maxYCoord() - yCoord) / _res)),
                                       bottom);
    }

    inline void getROIIndices(const double xMin, const double yMin, const double xMax, const double yMax, int &xxMin, int &yyMin, int &xxMax, int &yyMax) const
    {
        xcol(xMin, xxMin);
        xcol(xMax, xxMax);

        lin(yMin, yyMax);
        lin(yMax, yyMin);

        Eigen::Vector3d center;
        if (getCellCenterCoordinates(xxMin, yyMin, center))
        {
            if (center(0) < xMin) {xxMin++;}
            if (center(1) < yMin) {yyMax--;}
        }

        if (getCellCenterCoordinates(xxMax, yyMax, center))
        {
            if (center(0) > xMax) {xxMax--;}
            if (center(1) > yMax) {yyMin++;}
        }

        if (xMin < minXCoord() || xxMin < 0) {xxMin = 0;}
        if (xMax > maxXCoord() || xxMax > (_dimx - 1)) {xxMax = _dimx - 1;}

        if (yMin < minYCoord() || yyMax > (_dimy - 1)) {yyMax = _dimy - 1;}
        if (yMax > maxYCoord() || yyMin < 0) {yyMin = 0;}
    }

    inline bool compare(const CT_AbstractImage2D* other) const
    {
        if (other->xdim() != xdim()) {return false;}
        if (other->ydim() != ydim()) {return false;}
        if (other->minXCoord() != minXCoord()) {return false;}
        if (other->minYCoord() != minYCoord()) {return false;}
        if (other->resolution() != resolution()) {return false;}

        return true;
    }


protected:
    int          _dimx;        /*!< Nombre de cases selon x du raster*/
    int          _dimy;        /*!< Nombre de cases selon y du raster*/
    double       _minXCoord;   /*!< Coordonnée colonne minimum*/
    double       _minYCoord;   /*!< Coordonnée ligne minimum*/
    double       _res;           /*!< Resolution de la grille (taille d'une case)*/
    double       _level;         /*!< Niveau Z de placement du raster*/

};

#endif // CT_ABSTRACTIMAGE2D_H
