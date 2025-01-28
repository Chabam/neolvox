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

#ifndef CT_GRID2DXY_H
#define CT_GRID2DXY_H

#include "ct_math/ct_math.h"
#include "ct_itemdrawable/abstract/ct_virtualgrid2d.h"

template< typename DataT > class CT_StandardGrid2DXYDrawManager;

/*!
 * \class CT_Grid2DXY
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 2D Raster grid in XY plane</b>
 *
 * It represents raster grid in XY plane, with templated values in cells.
 *
 */
template< typename DataT>
class CT_Grid2DXY : public CT_VirtualGrid2D<DataT>
{
public:

    /**
      * \brief Empty Contructor vide
      */
    CT_Grid2DXY();

    /*!
     * \brief Contructor with integer column and row coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param dimx Number of colums
     * \param dimy Number of rows
     * \param resolution Size of a cell
     * \param zlevel Default Z value for raster plane
     * \param na Value used to code NA
     * \param initValue Initialisation value for raster cells
     */
    CT_Grid2DXY(const CT_OutAbstractSingularItemModel *model,
                const CT_AbstractResult *result,
                double xmin,
                double ymin,
                size_t dimx,
                size_t dimy,
                double resolution,
                double zlevel,
                DataT na,
                DataT initValue);

    CT_Grid2DXY(const QString &modelName,
                const CT_AbstractResult *result,
                double xmin,
                double ymin,
                size_t dimx,
                size_t dimy,
                double resolution,
                double zlevel,
                DataT na,
                DataT initValue);

    /*!
      * \brief CT_Grid2DXY factory with min and max (X,Y) coordinates
      *
      * \param model Item model for creation
      * \param result Result containing the item
      * \param xmin Minimum X coordinate (bottom left corner)
      * \param ymin Minimum Y coordinate (bottom left corner)
      * \param xmax Maximum X coordinate (upper right corner)
      * \param ymax Maximum Y coordinate (upper right corner)
      * \param resolution Size of a cell
      * \param zlevel Default Z value for raster plane
      * \param na Value used to code NA
      * \param initValue Initialisation value for raster cells
      * \param coodConstructor Not used, only to ensure constructor different signatures
      */
    static CT_Grid2DXY<DataT>* createGrid2DXYFromXYCoords(const CT_OutAbstractSingularItemModel *model,
                                                          const CT_AbstractResult *result,
                                                          double xmin,
                                                          double ymin,
                                                          double xmax,
                                                          double ymax,
                                                          double resolution,
                                                          double zlevel,
                                                          DataT na,
                                                          DataT initValue);

    static CT_Grid2DXY<DataT>* createGrid2DXYFromXYCoords(const QString &modelName,
                                                          const CT_AbstractResult *result,
                                                          double xmin,
                                                          double ymin,
                                                          double xmax,
                                                          double ymax,
                                                          double resolution,
                                                          double zlevel,
                                                          DataT na,
                                                          DataT initValue);

    virtual ~CT_Grid2DXY();
    
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
      * \brief Gives the value at (x,y) coordinate
      * \param col X coordinate
      * \param lin Y coordinate
      * \return Value at (x,y)
      */
    inline DataT valueAtXY(const double x, const double y) const {return this->valueAtCoords(x, y);}

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setValueAtXY(const double x, const double y, const DataT value) {return this->setValueAtCoords(x, y, value);}

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value, if value is superior to actual one
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setMaxValueAtXY(const double x, const double y, const DataT value) {return this->setMaxValueAtCoords(x, y, value);}

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value, if value is inferior to actual one
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    inline bool setMinValueAtXY(const double x, const double y, const DataT value) {return this->setMinValueAtCoords(x, y, value);}

    /**
      * \brief Add value to the cell containing (x,y) coordinate
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been updated
      */
    inline bool addValueAtXY(const double x, const double y, const DataT value) {return this->addValueAtCoords(x, y, value);}

    const static CT_StandardGrid2DXYDrawManager<DataT> ABSGRID2DXY_DRAW_MANAGER;
};

// Spécialisations

// fin des spécialisations


// Includes the template implementations
#include "ct_itemdrawable/ct_grid2dxy.hpp"

#endif // CT_GRID2DXY_H
