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

#ifndef CT_VIRTUALGRID2D_H
#define CT_VIRTUALGRID2D_H

#include "ct_itemdrawable/abstract/ct_abstractgrid2d.h"
#include "ct_itemdrawable/tools/ct_itemplateddata2darray.h"
#include "ct_math/ct_math.h"

template< typename DataT > class CT_StandardGrid2DDrawManager;

/*!
 * \class CT_VirtualGrid2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 2D Raster grid</b>
 *
 * It represents raster grid in 2D, with templated values in cells.
 *
 */
template< typename DataT>
class CT_VirtualGrid2D : public CT_AbstractGrid2D, public CT_ITemplatedData2DArray<DataT>
{
public:

    /*!
     *  \brief How to manage central cell when using neighboursValues method
     *
     */
    enum CenterMode
    {
        CM_KeepCenter = 1,      /*!< Gives neighbours and central value */
        CM_NAasCenter = 2,      /*!< Gives neighbours and NA for central value */
        CM_DropCenter = 3       /*!< Gives neighbours only */
    };

    /**
      * \brief Empty Contructor vide
      */
    CT_VirtualGrid2D();

    CT_VirtualGrid2D(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result);

    CT_VirtualGrid2D(const QString &modelName, const CT_AbstractResult *result);


    /*!
     * \brief Destructor
     */
    virtual ~CT_VirtualGrid2D();


    /*!
     * \brief Initialize all raster cells values with val
     *
     * Used by Constructors.
     *
     * \param val
     */
    void initGridWithValue(const DataT val);

    virtual QString getType() const;
    static QString staticGetType();

    virtual QString name() const = 0;

    const CT_ITemplatedData2DArray<DataT>* iTemplatedData2DArray() const { return this; }

    /*!
     * \brief Compute min and max values
     */
    void computeMinMax();

    /**
      * \brief Gives the NA value
      * \return Valeur NA value
      */
    inline DataT NA() const {return _NAdata;}

    /*!
     * \brief Gives the max value of the raster
     * \return Max value
     */
    inline DataT dataMax() const { return _dataMax; }

    /*!
     * \brief Gives the min value of the raster
     * \return Min value
     */
    inline DataT dataMin() const { return _dataMin; }

    // CT_ITemplatedData2DArray
    size_t xArraySize() const { return colDim(); }

    // CT_ITemplatedData2DArray
    size_t yArraySize() const { return linDim(); }

    /*!
     * \brief Set value at specified index
     * \param index Index
     * \param value Value
     * \return True if the value has actually been set
     */
    bool setValueAtIndex(const size_t index, const DataT value);

    /**
      * \brief Set the value at row lin and columne col to value
      * \param col column, first one is 0
      * \param lin row, first one is 0
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setValue(const size_t col, const size_t lin, const DataT value);

    /*!
     * \brief ives the value at specified index
     * \param index Index
     * \return Value
     */
    DataT valueAtIndex(const size_t index) const;

    /**
      * \brief Gives the value at row lin and columne col
      * \param col column, first one is 0
      * \param lin row, first one is 0
      * \return Value at row lin and column clox
      */
    DataT value(const size_t col, const size_t lin) const;

    // CT_ITemplatedData2DArray
    DataT dataFromArray(const size_t &x, const size_t &y) const;

    // CT_ITemplatedData2DArray
    DataT dataFromArray(const size_t &index) const;

    /**
      * \brief Set the value for the cell at specified index, if value is superior to actual one
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMaxValueAtIndex(const size_t index, const DataT value);


    /**
      * \brief Set the value for the cell at specified index, if value is inferior to actual one
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMinValueAtIndex(const size_t index, const DataT value);


    /**
      * \brief Add value to the cell at the specified index
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool addValueAtIndex(const size_t index, const DataT value);

    /*!
     * \brief Generic [0;1]  (or -1 for NA) value accessor for use as CT_AbstractGrid3D
     * \param index index in the grid
     * \return A double value between 0 (min value) and 1 (max value), or -1 for NA
     */
    virtual double ratioValueAtIndex(const size_t index) const;


    /*!
     * \brief Return a double value for any type (or NAN for NA)
     * \param index index in the grid
     * \return A double value
     */
    virtual double valueAtIndexAsDouble(const size_t index) const;


    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t index) const;


    /*!
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const;

    /**
      * \brief Gives neighbours values
      * \param col column, first one is 0
      * \param lin row, first one is 0
      * \param distance : Distance for neighbours search in cells
      * \param keepNAs : Should the NA values be kept ?
      * \param centermode : How should be treated the central cell ?
      * \return List of neighbours values
      */
    QList<DataT> neighboursValues(const size_t col, const size_t lin, const size_t distance, const bool keepNAs, const CenterMode centermode) const;

    inline DataT* getPointerToData() {return &_data[0];}

protected:


    /**
      * \brief Gives the value at (x,y) coordinate
      * \param col X coordinate
      * \param lin Y coordinate
      * \return Value at (x,y)
      */
    DataT valueAtCoords(const double x, const double y) const;

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setValueAtCoords(const double x, const double y, const DataT value);

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value, if value is superior to actual one
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMaxValueAtCoords(const double x, const double y, const DataT value);

    /**
      * \brief Set the value for the cell containing (x,y) coordinate to value, if value is inferior to actual one
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMinValueAtCoords(const double x, const double y, const DataT value);

    /**
      * \brief Add value to the cell containing (x,y) coordinate
      * \param x X coordinate
      * \param y Y coordinate
      * \param value Value
      * \return True if the value has actually been updated
      */
    bool addValueAtCoords(const double x, const double y, const DataT value);

    DataT               _NAdata;    /*!< Valeur codant NA */

    DataT               _dataMax;   /*!< valeur maximale du raster*/
    DataT               _dataMin;   /*!< valeur minimale du raster*/
    std::vector<DataT>  _data;    /*!< Tableau contenant les données pour chaque case de la grille*/

    CT_DEFAULT_IA_BEGIN(CT_VirtualGrid2D<DataT>)
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataXDimension(), &CT_VirtualGrid2D<DataT>::colDim, QObject::tr("X dimension"), "xd")
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataYDimension(), &CT_VirtualGrid2D<DataT>::linDim, QObject::tr("Y dimension"), "yd")
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataX(), &CT_VirtualGrid2D<DataT>::minX, QObject::tr("X min"), "xm")
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataY(), &CT_VirtualGrid2D<DataT>::minY, QObject::tr("Y min"), "ym")
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataResolution(), &CT_VirtualGrid2D<DataT>::resolution, QObject::tr("Resolution"), "res")
    CT_DEFAULT_IA_V3(CT_VirtualGrid2D<DataT>, CT_AbstractCategory::staticInitDataNa(), &CT_VirtualGrid2D<DataT>::NA, QObject::tr("NA"), "na")
    CT_DEFAULT_IA_END(CT_VirtualGrid2D<DataT>)
};

// Spécialisations

template<>
inline bool CT_VirtualGrid2D<bool>::NA() const {return false;}

template<>
PLUGINSHAREDSHARED_EXPORT void CT_VirtualGrid2D<bool>::computeMinMax();

template<>
PLUGINSHAREDSHARED_EXPORT double CT_VirtualGrid2D<bool>::ratioValueAtIndex(const size_t index) const;

template<>
PLUGINSHAREDSHARED_EXPORT double CT_VirtualGrid2D<bool>::valueAtIndexAsDouble(const size_t index) const;

template<>
PLUGINSHAREDSHARED_EXPORT QString CT_VirtualGrid2D<bool>::valueAtIndexAsString(const size_t index) const;

template<>
PLUGINSHAREDSHARED_EXPORT bool CT_VirtualGrid2D<bool>::setMaxValueAtIndex(const size_t index, const bool value);

template<>
PLUGINSHAREDSHARED_EXPORT bool CT_VirtualGrid2D<bool>::setMinValueAtIndex(const size_t index, const bool value);

template<>
PLUGINSHAREDSHARED_EXPORT bool CT_VirtualGrid2D<bool>::addValueAtIndex(const size_t index, const bool value);

template<>
PLUGINSHAREDSHARED_EXPORT QList<bool> CT_VirtualGrid2D<bool>::neighboursValues(const size_t colx, const size_t liny, const size_t distance, const bool keepNAs, const CenterMode centermode) const;


// fin des spécialisations

// Includes the template implementations
#include "ct_itemdrawable/abstract/ct_virtualgrid2d.hpp"


#endif // CT_VIRTUALGRID2D_H
