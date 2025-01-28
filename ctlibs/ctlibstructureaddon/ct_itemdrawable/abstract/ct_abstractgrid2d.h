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

#ifndef CT_ABSTRACTGRID2D_H
#define CT_ABSTRACTGRID2D_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_math/ct_math.h"

#include <Eigen/Core>

#define EPSILON_GRID2D 0.000001    // 10^-6

/*!
 * \class CT_AbstractGrid2D
 * \ingroup PluginShared_Items
 * \brief <b>Commun abstract base of all templated CT_Grid2D<type> </b>
 *
 * It's usefull to manage generically a grid, without knowing it template type
 *
 */
class PLUGINSHAREDSHARED_EXPORT CT_AbstractGrid2D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractGrid2D, CT_AbstractItemDrawableWithoutPointCloud, 2D grid)

public:

    /**
      * \brief Empty Contructor vide
      */
    CT_AbstractGrid2D();
    CT_AbstractGrid2D(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result);
    CT_AbstractGrid2D(const QString &modelName, const CT_AbstractResult *result);

    virtual ~CT_AbstractGrid2D();

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
     * \brief Compute index for given column, row
     * \param col Column
     * \param lin Row
     * \param returnedIndex Returned index
     * \return true if index is valid
     */
    inline bool index(const size_t col, const size_t lin, size_t &returnedIndex) const
    {
        if ((col >= _dimCol) || (lin >= _dimLin))
        {
            return false;
        }
        else
        {
            returnedIndex = lin*_dimCol + col;
            return true;
        }
    }

    /*!
     * \brief Return column for specified colCoord coordinate
     * \param colCoord Column coordinate
     * \param column Column number
     * \return true if index is valid
     */
    inline bool col(const double colCoord, size_t &column) const
    {
        if (fabs(colCoord - minColCoord()) < EPSILON_GRID2D) {column = 0;         return true;}
        if (fabs(colCoord - maxColCoord()) < EPSILON_GRID2D) {column = _dimCol - 1; return true;}

        if (colCoord < minColCoord() || colCoord > maxColCoord()) {return false;}
        if (colCoord == maxColCoord())
        {
            column = _dimCol - 1;
        }
        column = (size_t) floor((colCoord - minColCoord()) / _res);
        return true;
    }

    /*!
     * \brief Return row for specified linCoord coordinate
     * \param linCoord Row coordinate
     * \param Row number
     * \return true if index is valid
     */
    inline bool lin(const double linCoord, size_t &row) const
    {
        if (fabs(linCoord - minLinCoord()) < EPSILON_GRID2D) {row = 0;         return true;}
        if (fabs(linCoord - maxLinCoord()) < EPSILON_GRID2D) {row = _dimLin - 1; return true;}

        if (linCoord < minLinCoord() || linCoord > maxLinCoord()) {return false;}
        if (linCoord == maxLinCoord())
        {
            row = _dimLin - 1;
        }
        row = (size_t) floor((linCoord - minLinCoord()) / _res);
        return true;
    }

    /*!
     * \brief Compute index for given (colCoord, linCoord) coordinate
     * \param colCoord column coordinate
     * \param linCoord Row coordinate
     * \param index Returned index
     * \return true if returnedIndex is valid
     */
    inline bool indexAtCoords(const double colCoord, const double linCoord, size_t &returnedIndex) const
    {
        size_t colx, liny;
        if (!col(colCoord, colx) || !lin(linCoord, liny)) {return false;}

        return index(colx, liny, returnedIndex);
    }

    /*!
     * \brief indexToGrid Convert index in (coly, lin) grid coordinates
     * \param index Index of the cell
     * \param col Resulting column of the cell
     * \param lin Resulting row of the cell
     * \return true if the index is in the grid
     */
    inline bool indexToGrid(const size_t index, size_t &col, size_t &lin) const
    {
        if (index >= nCells()) {return false;}

        lin = index / _dimCol;
        col = index % _dimCol;

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
        min(0) = minColCoord();
        min(1) = minLinCoord();
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
    inline size_t colDim() const {return _dimCol;}

    /**
      * \brief Gives the number of rows
      * \return Number of rows
      */
    inline size_t linDim() const {return _dimLin;}

    /**
      * \brief Gives the minimum column coordinate
      * \return minimum column coordinate
      */
    inline double minColCoord() const {return _minColCoord;}

    /**
      * \brief Gives the minimum row coordinate
      * \return minimum row coordinate
      */
    inline double minLinCoord() const {return _minLinCoord;}

    /**
      * \brief Gives the maximum column coordinate
      * \return maximum column coordinate
      */
    inline double maxColCoord() const {return _minColCoord + _dimCol*_res;}

    /**
      * \brief Gives the maximum row coordinate
      * \return maximum row coordinate
      */
    inline double maxLinCoord() const {return _minLinCoord + _dimLin*_res;}

    /*!
     * \brief Total number of cells for the raster
     * \return Number of cells
     */
    inline size_t nCells() const {return _dimCol*_dimLin;}

    /*!
     * \brief Gives the column coordinate of the center of cells of column col
     * \param col Column, first one is 0
     * \return Column coordinate
     */
    inline double getCellCenterColCoord(const size_t col) const {return minColCoord() + col*_res + _res/2;}

    /*!
     * \brief Gives the row coordinate of the center of cells of row lin
     * \param lin Row, first one is 0
     * \return Row coordinate
     */
    inline double getCellCenterLinCoord(const size_t lin) const {return minLinCoord() + lin*_res + _res/2;}

    /*!
     * \brief getCellCoordinates Give min and max coordinates of a cell
     * \param index Index of the cell
     * \param bottom  Min coordinates
     * \param top Max coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCoordinates(const size_t index, Eigen::Vector2d &bottom, Eigen::Vector2d &top) const
    {
        size_t col, lin;
        if (!indexToGrid(index, col, lin)) {return false;}
        bottom(0) = minColCoord() + col*_res;
        bottom(1) = minLinCoord() + lin*_res;

        top(0) = bottom(0) + _res;
        top(1) = bottom(1) + _res;
        return true;
    }

    inline bool getCellCenterCoordinates(const size_t index, Eigen::Vector3d &center) const
    {
        size_t col, lin;
        if (!indexToGrid(index, col, lin)) {return false;}

        center(0) = minColCoord() + col*_res + _res/2.0;
        center(1) = minLinCoord() + lin*_res + _res/2.0;
        center(2) = _level;

        return true;
    }


    /*!
     * \brief Get the left corner coordinates of the cell defined by (col, lin)
     * \param col Column
     * \param lin Row
     * \param bottom Output coordinates
     * \return A Eigen::Vector2d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCorner(const size_t col, const size_t lin, Eigen::Vector2d &bottom) const
    {
        if ((col >= _dimCol) || (lin >= _dimLin)) {return false;}
        bottom(0) = minColCoord() + col*_res;
        bottom(1) = minLinCoord() + lin*_res;

        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell containing by (colCoord, linCoord)
     * \param colCoord column coordinate
     * \param linCoord Row coordinate
     * \param bottom Output coordinates
     * \return A Eigen::Vector2d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCornerAtCoords(const double colCoord, const double linCoord, Eigen::Vector2d &bottom) const
    {

        return getCellBottomLeftCorner((size_t) floor((colCoord - minColCoord()) / _res),
                                       (size_t) floor((linCoord - minLinCoord()) / _res),
                                       bottom);
    }

protected:
    size_t      _dimCol;        /*!< Nombre de cases selon x du raster*/
    size_t      _dimLin;        /*!< Nombre de cases selon y du raster*/
    double       _minColCoord;   /*!< Coordonnée colonne minimum*/
    double       _minLinCoord;   /*!< Coordonnée ligne minimum*/
    double       _res;           /*!< Resolution de la grille (taille d'une case)*/
    double       _level;         /*!< Niveau Z de placement du raster*/

};

#endif // CT_ABSTRACTGRID2D_H
