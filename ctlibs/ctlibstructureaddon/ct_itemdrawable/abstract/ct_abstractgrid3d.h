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

#ifndef CT_ABSTRACTGRID3D_H
#define CT_ABSTRACTGRID3D_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ctlibstructureaddon_global.h"

/*!
 * \class CT_AbstractGrid3D
 * \ingroup PluginShared_Items
 * \brief <b>Commun abstract base of all templated CT_AbstractGrid3D<type> </b>
 *
 * It's usefull to manage generically a grid, without knowing it template type
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractGrid3D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractGrid3D, CT_AbstractItemDrawableWithoutPointCloud, 3D grid)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:

    /**
      * \brief Empty Contructor vide
      */
    CT_AbstractGrid3D();
    CT_AbstractGrid3D(int dimx,
                      int dimy,
                      int dimz,
                      double resolution);

    CT_AbstractGrid3D(double xmin,
                      double ymin,
                      double zmin,
                      int dimx,
                      int dimy,
                      int dimz,
                      double resolution);

    CT_AbstractGrid3D(double xmin,
                      double ymin,
                      double zmin,
                      double xmax,
                      double ymax,
                      double zmax,
                      double resolution);
    CT_AbstractGrid3D(const CT_AbstractGrid3D& other) = default;

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
    inline bool index(const int colx, const int liny, const int levz, size_t &returnedIndex) const
    {
        if ((colx < 0) || (liny < 0) || (levz <0) || (colx >= _dimx) || (liny >= _dimy) || (levz >= _dimz))
        {
            return false;
        }
        else
        {
            returnedIndex = size_t(levz)*size_t(_dimx)*size_t(_dimy) + size_t(liny)*size_t(_dimx) + size_t(colx);
            return true;
        }
    }

    /*!
     * \brief Return column for specified x coordinate
     * \param x X coordinate
     * \param colx Returned column
     * \return false if x invalid
     */
    inline bool colX(const double x, int &colx) const
    {
        if (x < minX() || x > maxX()) {return false;}

        colx = int(floor((x - minX()) / _res));

        if (colx < 0) {return false;}
        if (colx > (_dimx - 1)) {return false;}

        return true;
    }

    /*!
     * \brief Return row for specified y coordinate
     * \param y Y coordinate
     * \param liny Returned row
     * \return false if y invalid
     */
    inline bool linY(const double y, int &liny) const
    {
        if (y < minY() || y > maxY()) {return false;}

        liny = int(floor((y - minY()) / _res));

        if (liny < 0) {return false;}
        if (liny > (_dimy - 1)) {return false;}

        return true;
    }

    /*!
     * \brief Return z level for specified z coordinate
     * \param z Z coordinate
     * \param levz Returned z level
     * \return false if z invalid
     */
    inline bool levelZ(const double z, int &levz) const
    {
        if (z < minZ() || z > maxZ()) {return false;}

        levz = int(floor((z - minZ()) / _res));

        if (levz < 0) {return false;}
        if (levz > (_dimz - 1)) {return false;}

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
    inline bool indexToGrid(const size_t index, int &colx, int &liny, int &levz) const
    {
        if (index>=nCells()) {return false;}

        size_t mod;

        levz = int(index / (size_t(_dimx)*size_t(_dimy)));
        mod = index % (size_t(_dimx)*size_t(_dimy));

        liny = int(mod / size_t(_dimx));
        colx = int(mod % size_t(_dimx));

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
        int colx, liny, levz;

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
    inline int xdim() const {return _dimx;}

    /**
      * \brief Gives the number of rows
      * \return Number of rows
      */
    inline int ydim() const {return _dimy;}

    /**
      * \brief Gives the number of z levels
      * \return Number of z levels
      */
    inline int zdim() const {return _dimz;}

    /*!
     * \brief Total number of cells for the grid
     * \return Number of cells
     */
    inline size_t nCells() const {return size_t(_dimx)*size_t(_dimy)*size_t(_dimz);}

    /*!
     * \brief Gives the X coordinate of the center of cells of column colx
     * \param colx Column, first one is 0
     * \return X coordinate
     */
    inline double getCellCenterX(const int colx) const
    {
        return minX() + (double(colx))*_res + _res/2.0;
    }

    /*!
     * \brief Gives the Y coordinate of the center of cells of row liny
     * \param liny Row, first one is 0
     * \return Y coordinate
     */
    inline double getCellCenterY(const int liny) const
    {
        return minY() + (double(liny))*_res + _res/2.0;
    }

    /*!
     * \brief Gives the Z coordinate of the center of cells of z level levz
     * \param levz z level, first one is 0
     * \return Z coordinate
     */
    inline double getCellCenterZ(const int levz) const
    {
        return minZ() + (double(levz))*_res + _res/2.0;
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
        int colx, liny, levz;
        if (!indexToGrid(index, colx, liny, levz)) {return false;}
        bottom(0) = minX() + colx*_res;
        bottom(1) = minY() + liny*_res;
        bottom(2) = minZ() + levz*_res;

        top(0) = bottom(0) + _res;
        top(1) = bottom(1) + _res;
        top(2) = bottom(2) + _res;
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
        int colx, liny, levz;
        if (!indexToGrid(index, colx, liny, levz)) {return false;}
        double demiRes = _res / 2.0;
        center(0) = minX() + colx*_res + demiRes;
        center(1) = minY() + liny*_res + demiRes;
        center(2) = minZ() + levz*_res + demiRes;
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
    inline bool getCellBottomLeftCorner(const int colx, const int liny, const int levz, Eigen::Vector3d &bottom) const
    {
        if ((colx >= _dimx) || (liny >= _dimy) || (levz >= _dimz)) {return false;}
        bottom(0) = minX() + colx*_res;
        bottom(1) = minY() + liny*_res;
        bottom(2) = minZ() + levz*_res;

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

        return getCellBottomLeftCorner(int(floor((x - minX()) / _res)),
                                       int(floor((y - minY()) / _res)),
                                       int(floor((z - minZ()) / _res)),
                                       bottom);
    }

    /*!
     * \brief Factory with min and max (X,Y,Z) coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param xmax Maximum X coordinate (upper right corner)
     * \param ymax Maximum Y coordinate (upper right corner)
     * \param zmax Maximum Z coordinate (upper right corner)
     * \param resolution Size of a cell
     * \param coordConstructor Not used, only to ensure constructor different signatures
     */
    template<class GridType, typename... OtherArgs>
    static GridType* CreateGrid3DTFromXYZCoords(double xmin,
                                                double ymin,
                                                double zmin,
                                                double xmax,
                                                double ymax,
                                                double zmax,
                                                double resolution,
                                                bool extends,
                                                OtherArgs&&... args)
    {
        int dimx = int(ceil((xmax - xmin)/resolution));
        int dimy = int(ceil((ymax - ymin)/resolution));
        int dimz = int(ceil((zmax - zmin)/resolution));

        if (extends)
        {
            // to ensure a point exactly on a maximum limit of the grid will be included in the grid
            while (xmax >= (xmin + dimx * resolution))
            {
                dimx++;
            }

            while (ymax >= (ymin + dimy * resolution))
            {
                dimy++;
            }

            while (zmax >= (zmin + dimz * resolution))
            {
                dimz++;
            }
        }

        return new GridType(xmin, ymin, zmin, dimx, dimy, dimz, resolution, std::forward<OtherArgs>(args)...);
    }

protected:
    int         _dimx;      /*!< Nombre de cases selon x du grid*/
    int         _dimy;      /*!< Nombre de cases selon y du grid*/
    int         _dimz;      /*!< Nombre de cases selon z du grid*/
    double      _res;       /*!< Resolution de la grille (taille d'une case*/
};

#endif // CT_ABSTRACTGRID3D_H
