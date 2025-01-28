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

#ifndef CT_IMAGE2D_H
#define CT_IMAGE2D_H

#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"

#include "opencv2/core/core.hpp"

template< typename DataT > class CT_StandardImage2DDrawManager;


/*!
 * \class CT_Image2D
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 2D Raster grid</b>
 *
 * It represents raster grid in 2D, with templated values in cells.
 *
 */
template< typename DataT>
class CT_Image2D : public CT_AbstractImage2D
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_Image2D, DataT, CT_AbstractImage2D, Raster)
    using SuperClass = CT_AbstractImage2D;

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
    CT_Image2D();

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
    CT_Image2D(double xmin,
               double ymin,
               int dimx,
               int dimy,
               double resolution,
               double zlevel,
               DataT na,
               DataT initValue);

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
     *          - OpenCv mat
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_Image2D(const CT_Image2D<DataT>& other);

    /*!
     * \brief Destructor
     */
    ~CT_Image2D() override;

    /*!
      * \brief CT_Image2D factory with min and max (X,Y) coordinates
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
    static CT_Image2D<DataT>* createImage2DFromXYCoords(double xmin,
                                                          double ymin,
                                                          double xmax,
                                                          double ymax,
                                                          double resolution,
                                                          double zlevel,
                                                          DataT na,
                                                          DataT initValue);


    /*!
     * \brief Initialize all raster cells values with val
     *
     * Used by Constructors.
     *
     * \param val
     */
    void initGridWithValue(const DataT val);

    /*!
     * \brief Compute min and max values
     */
    void computeMinMax() override;

    void setMinMax(DataT min, DataT max);


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

    /*!
     * \brief Set value at specified index
     * \param index Index
     * \param value Value
     * \return True if the value has actually been set
     */
    bool setValueAtIndex(const size_t index, const DataT value);

    /**
      * \brief Set the value at x y and columne col to value
      * \param x column, first one is 0
      * \param y row, first one is 0
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setValue(const int x, const int y, const DataT value);

    /*!
     * \brief ives the value at specified index
     * \param index Index
     * \return Value
     */
    DataT valueAtIndex(const size_t index) const;

    /**
      * \brief Gives the value at x y and columne col
      * \param x column, first one is 0
      * \param y row, first one is 0
      * \return Value at x y and column clox
      */
    DataT value(const int x, const int y) const;

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
    virtual double ratioValueAtIndex(const size_t index) const override;


    /*!
     * \brief Return a double value for any type (or NAN for NA)
     * \param index index in the grid
     * \return A double value
     */
    virtual double valueAtIndexAsDouble(const size_t index) const override;


    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t index) const override;


    /*!
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const override;


    /*!
     * \brief return na value as a double
     *
     */
    virtual double NAAsDouble() const override;

    virtual double minValueAsDouble() const override;
    virtual double maxValueAsDouble() const override;


    /**
      * \brief Gives neighbours values
      * \param x column, first one is 0
      * \param y row, first one is 0
      * \param distance : Distance for neighbours search in cells
      * \param keepNAs : Should the NA values be kept ?
      * \param centermode : How should be treated the central cell ?
      * \return List of neighbours values
      */
    QList<DataT> neighboursValues(const int x, const int y, const int distance, const bool keepNAs, const CenterMode centermode) const;

    inline DataT* getPointerToData() {return &_data(0,0);}

    /**
      * \brief Gives the value at (x,y) coordinate
      * \param x X coordinate
      * \param y Y coordinate
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


    inline cv::Mat_<DataT>& getMat() {return _data;}

    const static CT_StandardImage2DDrawManager<DataT> IMAGE2D_DRAW_MANAGER;

    inline int xdim() const {return SuperClass::xdim();}
    inline int ydim() const {return SuperClass::ydim();}
    inline double resolution() const {return SuperClass::resolution();}

    CT_ITEM_COPY_IMP(CT_Image2D<DataT>)

protected:
    DataT               _NAdata;    /*!< Valeur codant NA */

    DataT               _dataMax;   /*!< valeur maximale du raster*/
    DataT               _dataMin;   /*!< valeur minimale du raster*/
    cv::Mat_<DataT>     _data;      /*!< Tableau contenant les données pour chaque case de la grille*/

    CT_DEFAULT_IA_BEGIN(CT_Image2D<DataT>)
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataXDimension(), &CT_Image2D<DataT>::xdim, QObject::tr("X dimension"))
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataYDimension(), &CT_Image2D<DataT>::ydim, QObject::tr("Y dimension"))
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataResolution(), &CT_Image2D<DataT>::resolution, QObject::tr("Resolution"))
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataNa(), &CT_Image2D<DataT>::NA, QObject::tr("NA"))
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataValue(), &CT_Image2D<DataT>::dataMin, QObject::tr("Min Value"))
    CT_DEFAULT_IA_V2(CT_Image2D<DataT>, CT_AbstractCategory::staticInitDataValue(), &CT_Image2D<DataT>::dataMax, QObject::tr("Max Value"))
    CT_DEFAULT_IA_END(CT_Image2D<DataT>)
};

// Spécialisations

template<>
inline bool CT_Image2D<bool>::NA() const {return false;}

template<>
CTLIBSTRUCTUREADDON_EXPORT void CT_Image2D<bool>::computeMinMax();

template<>
CTLIBSTRUCTUREADDON_EXPORT double CT_Image2D<bool>::ratioValueAtIndex(const size_t index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT double CT_Image2D<bool>::valueAtIndexAsDouble(const size_t index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT QString CT_Image2D<bool>::valueAtIndexAsString(const size_t index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Image2D<bool>::setMaxValueAtIndex(const size_t index, const bool value);

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Image2D<bool>::setMinValueAtIndex(const size_t index, const bool value);

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Image2D<bool>::addValueAtIndex(const size_t index, const bool value);

template<>
CTLIBSTRUCTUREADDON_EXPORT QList<bool> CT_Image2D<bool>::neighboursValues(const int x, const int y, const int distance, const bool keepNAs, const CenterMode centermode) const;

// TODO : unsigned long not vailable with opencv 4
/*template<>
CTLIBSTRUCTUREADDON_EXPORT QString CT_Image2D<unsigned long>::valueAtIndexAsString(const size_t index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT QString CT_Image2D<unsigned long>::NAAsString() const;
*/

// fin des spécialisations

// Includes the template implementations
#include "ct_itemdrawable/ct_image2d.hpp"

#endif // CT_IMAGE2D_H
