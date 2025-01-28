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

#ifndef LVOX3_ABSTRACTGRID3D_H
#define LVOX3_ABSTRACTGRID3D_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_math/ct_mathpoint.h"

#include <Eigen/Core>
#include <QDebug>

#define EPSILON_GRID3D 0.000001    // 10^-6


/*!
 * \class LVOX3_AbstractGrid3D
 * \brief <b>Shared abstract base of all templated LVOX3_AbstractGrid3D<type> </b>
 *
 * Re-implementation of the original CT_AbstractGrid3D to include voxel resx,resy,resz
 *
 */
class LVOX3_AbstractGrid3D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(LVOX3_AbstractGrid3D, CT_AbstractItemDrawableWithoutPointCloud, 3D grid)

public:

    /**
      * \brief Empty Contructor vide
      */
    LVOX3_AbstractGrid3D();

    LVOX3_AbstractGrid3D(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result);
    LVOX3_AbstractGrid3D(const QString &modelName, const CT_AbstractResult *result);

    virtual ~LVOX3_AbstractGrid3D() override;

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
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const = 0;

    /*!
     * \brief return true if NA in cell at given index
     *
     */
    virtual bool isNA(size_t index) const = 0;

    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t index) const = 0;

    /*!
     * \brief Compute index for given column, row and z level
     * \param colx Column
     * \param liny Row
     * \param levz Z level
     * \param index Returned index
     * \return false if index is invalid
     */
    inline bool index(const size_t colx, const size_t liny, const size_t levz, size_t &returnedIndex) const
    {
        if ((colx >= _dimx) || (liny >= _dimy) || (levz >= _dimz))
        {
            return false;
        }
        else
        {
            returnedIndex = levz*_dimx*_dimy + liny*_dimx + colx;
            return true;
        }
    }

    /*!
     * \brief Return column for specified x coordinate
     * \param x X coordinate
     * \param colx Returned column
     * \return false if x invalid
     */
    inline bool colX(const double x, size_t &colx) const
    {
        if (fabs(x - minX()) < EPSILON_GRID3D) {colx = 0;         return true;}
        if (fabs(x - maxX()) < EPSILON_GRID3D) {colx = _dimx - 1; return true;}

        if (x < minX() || x > maxX()) {return false;}
        if (x == maxX())
        {
            colx =  _dimx - 1;
        } else {
            colx = (size_t) floor((x - minX()) / _resx);
        }
        return true;
    }

    /*!
     * \brief Return row for specified y coordinate
     * \param y Y coordinate
     * \param liny Returned row
     * \return false if y invalid
     */
    inline bool linY(const double y, size_t &liny) const
    {
        if (fabs(y - minY()) < EPSILON_GRID3D) {liny = 0;         return true;}
        if (fabs(y - maxY()) < EPSILON_GRID3D) {liny = _dimy - 1; return true;}

        if (y < minY() || y > maxY()) {return false;}
        if (y == maxY())
        {
            liny =  _dimy - 1;
        } else {
            liny = (size_t) floor((y - minY()) / _resy);
        }
        return true;
    }

    /*!
     * \brief Return z level for specified z coordinate
     * \param z Z coordinate
     * \param levz Returned z level
     * \return false if z invalid
     */
    inline bool levelZ(const double z, size_t &levz) const
    {
        if (fabs(z - minZ()) < EPSILON_GRID3D) {levz = 0;         return true;}
        if (fabs(z - maxZ()) < EPSILON_GRID3D) {levz = _dimz - 1; return true;}

        if (z < minZ() || z > maxZ()) {return false;}
        if (z == maxZ())
        {
            levz =  _dimz - 1;
        } else {
            levz = (size_t) floor((z - minZ()) / _resz);
        }
        return true;
    }

    /*!
     * \brief indexToGrid Convert index in (coly, liny, levz) grid coordinates
     * \param index Index of the cell
     * \param colx Resulting column of the cell
     * \param liny Resulting row of the cell
     * \param levz Resulting z level of the cell
     * \return true if the index is in the grid
     */
    inline bool indexToGrid(const size_t index, size_t &colx, size_t &liny, size_t &levz) const
    {
        if (index>=nCells()) {return false;}

        size_t mod;

        levz = index / (_dimx*_dimy);
        mod = index % (_dimx*_dimy);

        liny = mod / _dimx;
        colx = mod % _dimx;

        return true;
    }

    /*!
     * \brief Compute index for given (x, y, z) coordinate
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \param index Returned index
     * \return false if index is invalid
     */
    inline bool indexAtXYZ(const double x, const double y, const double z, size_t &returnedIndex) const
    {
        size_t colx, liny, levz;

        if (!colX(x, colx) || !linY(y, liny) || !levelZ(z, levz)) {return false;}

        return index(colx, liny, levz, returnedIndex);
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
      * \brief Gives the resolution of x axis
      * \return Resolution (m)
      */
    inline double xresolution() const {return _resx;}

    /**
      * \brief Gives the resolution of y axis
      * \return Resolution (m)
      */
    inline double yresolution() const {return _resy;}

    /**
      * \brief Gives the resolution of z axis
      * \return Resolution (m)
      */
    inline double zresolution() const {return _resz;}

    /**
     * \brief getMinCoordinates
     * \param min Min coordinates of the grid (bottom left corner)
     */
    inline void getMinCoordinates(Eigen::Vector3d &min) const
    {
        min = minCoordinates();
    }

    /**
     * \brief getMaxCoordinates
     * \param max Max coordinates of the grid (upper right corner)
     */
    inline void getMaxCoordinates(Eigen::Vector3d &max) const
    {
        max = maxCoordinates();
    }

    /**
      * \brief Gives the number of columns
      * \return Number of columns
      */
    inline size_t xdim() const {return _dimx;}

    /**
      * \brief Gives the number of rows
      * \return Number of rows
      */
    inline size_t ydim() const {return _dimy;}

    /**
      * \brief Gives the number of z levels
      * \return Number of z levels
      */
    inline size_t zdim() const {return _dimz;}

    /*!
     * \brief Total number of cells for the grid
     * \return Number of cells
     */
    inline size_t nCells() const {return _dimx*_dimy*_dimz;}

    /*!
     * \brief Gives the X coordinate of the center of cells of column colx
     * \param colx Column, first one is 0
     * \return X coordinate
     */
    inline double getCellCenterX(const size_t colx) const
    {
        return minX() + ((double)colx)*_resx + _resx/2;
    }

    /*!
     * \brief Gives the Y coordinate of the center of cells of row liny
     * \param liny Row, first one is 0
     * \return Y coordinate
     */
    inline double getCellCenterY(const size_t liny) const
    {
        return minY() + ((double)liny)*_resy + _resy/2;
    }

    /*!
     * \brief Gives the Z coordinate of the center of cells of z level levz
     * \param levz z level, first one is 0
     * \return Z coordinate
     */
    inline double getCellCenterZ(const size_t levz) const
    {
        return minZ() + ((double)levz)*_resz + _resz/2;
    }

    /*!
     * \brief getCellCoordinates Give min and max coordinates of a cell
     * \param index Index of the cell
     * \param bottom  Min coordinates
     * \param top Max coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCoordinates(const size_t index, Eigen::Vector3d &bottom, Eigen::Vector3d &top) const
    {
        size_t colx, liny, levz;
        if (!indexToGrid(index, colx, liny, levz)) {return false;}
        bottom(0) = minX() + colx*_resx;
        bottom(1) = minY() + liny*_resy;
        bottom(2) = minZ() + levz*_resz;

        top(0) = bottom(0) + _resx;
        top(1) = bottom(1) + _resy;
        top(2) = bottom(2) + _resz;
        return true;
    }

    /*!
     * \brief getCellCoordinates Give center coordinates of a cell
     * \param index Index of the cell
     * \param center  Center coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCenterCoordinates(const size_t index, Eigen::Vector3d &center) const
    {
        size_t colx, liny, levz;
        if (!indexToGrid(index, colx, liny, levz)) {return false;}
        double demiResx = _resx / 2.0;
        double demiResy = _resy / 2.0;
        double demiResz = _resz / 2.0;
        center(0) = minX() + colx*_resx + demiResx;
        center(1) = minY() + liny*_resy + demiResy;
        center(2) = minZ() + levz*_resz + demiResz;
        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell defined by (colx, liny, levz)
     * \param colx Column
     * \param liny Row
     * \param levz Z level
     * \param bottom Output coordinates
     * \return A Eigen::Vector3d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCorner(const size_t colx, const size_t liny, const size_t levz, Eigen::Vector3d &bottom) const
    {
        if ((colx >= _dimx) || (liny >= _dimy) || (levz >= _dimz)) {return false;}
        bottom(0) = minX() + colx*_resx;
        bottom(1) = minY() + liny*_resy;
        bottom(2) = minZ() + levz*_resz;

        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell containing by (x, y, z)
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \param bottom Output coordinates
     * \return A Eigen::Vector3d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCornerAtXYZ(const double x, const double y, const double z, Eigen::Vector3d &bottom) const
    {

        return getCellBottomLeftCorner((size_t) floor((x - minX()) / _resx),
                                       (size_t) floor((y - minY()) / _resy),
                                       (size_t) floor((z - minZ()) / _resz),
                                       bottom);
    }

protected:
    size_t         _dimx;      /*!< Nombre de cases selon x du grid*/
    size_t         _dimy;      /*!< Nombre de cases selon y du grid*/
    size_t         _dimz;      /*!< Nombre de cases selon z du grid*/
    double          _resx;     /*!< Resolution de la grille en x(taille d'une case)*/
    double          _resy;     /*!< Resolution de la grille en y(taille d'une case)*/
    double          _resz;     /*!< Resolution de la grille en z(taille d'une case)*/

};

#endif // LVOX3_ABSTRACTGRID3D_H
