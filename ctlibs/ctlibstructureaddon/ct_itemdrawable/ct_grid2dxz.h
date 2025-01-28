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

#ifndef CT_GRID2DXZ_H
#define CT_GRID2DXZ_H

#include "ct_math/ct_math.h"
#include "ct_itemdrawable/abstract/ct_virtualgrid2d.h"

template< typename DataT > class CT_StandardGrid2DXZDrawManager;

/*!
 * \class CT_Grid2DXZ
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a vertical 2D grid in XZ plane</b>
 *
 * It represents vertical 2D grid in XZ plane, with templated values in cells.
 *
 */
template< typename DataT>
class CT_Grid2DXZ : public CT_VirtualGrid2D<DataT>
{
public:

    /**
      * \brief Empty Contructor vide
      */
    CT_Grid2DXZ();

   /*!
     * \brief Contructor with integer column and row coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param dimx Number of colums
     * \param dimz Number of rows
     * \param resolution Size of a cell
     * \param ylevel Default Y value for raster plane
     * \param na Value used to code NA
     * \param initValue Initialisation value for raster cells
     */
    CT_Grid2DXZ(const CT_OutAbstractSingularItemModel *model,
                              const CT_AbstractResult *result,
                              double xmin,
                              double zmin,
                              size_t dimx,
                              size_t dimz,
                              double resolution,
                              double ylevel,
                              DataT na,
                              DataT initValue);

    CT_Grid2DXZ(const QString &modelName,
                              const CT_AbstractResult *result,
                              double xmin,
                              double zmin,
                              size_t dimx,
                              size_t dimz,
                              double resolution,
                              double ylevel,
                              DataT na,
                              DataT initValue);



    /*!
     * \brief Factory with min and max (X,Z) coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param xmax Maximum X coordinate (upper right corner)
     * \param zmax Maximum Z coordinate (upper right corner)
     * \param resolution Size of a cell
     * \param ylevel Default Y value for raster plane
     * \param na Value used to code NA
     * \param initValue Initialisation value for raster cells
     * \param coodConstructor Not used, only to ensure constructor different signatures
     */
    static CT_Grid2DXZ<DataT>* createGrid2DXZFromXZCoords(const CT_OutAbstractSingularItemModel *model,
                                                                       const CT_AbstractResult *result,
                                                                       double xmin,
                                                                       double zmin,
                                                                       double xmax,
                                                                       double zmax,
                                                                       double resolution,
                                                                       double ylevel,
                                                                       DataT na,
                                                                       DataT initValue);

    static CT_Grid2DXZ<DataT>* createGrid2DXZFromXZCoords(const QString &modelName,
                                                                       const CT_AbstractResult *result,
                                                                       double xmin,
                                                                       double zmin,
                                                                       double xmax,
                                                                       double zmax,
                                                                       double resolution,
                                                                       double ylevel,
                                                                       DataT na,
                                                                       DataT initValue);


    virtual ~CT_Grid2DXZ();

    /*!
     * \brief Create own alternative drawmanager
     */
    void initDrawManager(QString drawConfigurationName = "", bool mapMode = true, bool scale = false);

    virtual QString getType() const;
    static QString staticGetType();

    virtual QString name() const;
    static QString staticName();

    /*!
     * \brief Copy method
     *
     * \param model Item model for the copy
     * \param result Result containing the copy
     * \param copyModeList Copy mode
     * \return Item copy
     */
    virtual CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel *model, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList);
    virtual CT_AbstractItemDrawable* copy(const QString &modelName, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList);

    /**
      * \brief Gives the value at (x,z) coordinate
      * \param col X coordinate
      * \param lin Z coordinate
      * \return Value at (x,z)
      */
    inline DataT valueAtXZ(const double x, const double z) const {return this->valueAtCoords(x, z);}

    /**
      * \brief Set the value for the cell containing (x,z) coordinate to value
      * \param x X coordinate
      * \param z Z coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setValueAtXZ(const double x, const double z, const DataT value) {return this->setValueAtCoords(x, z, value);}

    /**
      * \brief Set the value for the cell containing (x,z) coordinate to value, if value is superior to actual one
      * \param x X coordinate
      * \param z Z coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setMaxValueAtXZ(const double x, const double z, const DataT value) {return this->setMaxValueAtCoords(x, z, value);}

    /**
      * \brief Set the value for the cell containing (x,z) coordinate to value, if value is inferior to actual one
      * \param x X coordinate
      * \param z Z coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setMinValueAtXZ(const double x, const double z, const DataT value) {return this->setMinValueAtCoords(x, z, value);}

    /**
      * \brief Add value to the cell containing (x,z) coordinate
      * \param x X coordinate
      * \param z Z coordinate
      * \param value Value
      * \return True if the value has actually been updated
      */
    inline bool addValueAtXZ(const double x, const double z, const DataT value) {return this->addValueAtCoords(x, z, value);}

    const static CT_StandardGrid2DXZDrawManager<DataT> ABSGRID2DXZ_DRAW_MANAGER;
};

// Spécialisations

// fin des spécialisations


// Includes the template implementations
#include "ct_itemdrawable/ct_grid2dxz.hpp"

#endif // CT_GRID2DXZ_H
