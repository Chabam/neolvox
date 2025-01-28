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

#ifndef CT_PROFILE_H
#define CT_PROFILE_H

#include "ct_itemdrawable/abstract/ct_abstractprofile.h"
#include <QMutex>

template< typename DataT > class CT_StandardProfileDrawManager;

/*!
 * \class CT_Profile
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 3D profile</b>
 *
 * It represents profile along one axis in 3D, with templated values in cells.
 *
 */
template< typename DataT>
class CT_Profile : public CT_AbstractProfile
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_Profile, DataT, CT_AbstractProfile, Profile)

    using SuperClass = CT_AbstractProfile;

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

    CT_Profile();

    /*!
     * \brief Contructor with integer column and row coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Origin X coordinate of profil axis
     * \param ymin Origin Y coordinate of profil axis
     * \param zmin Origin Z coordinate of profil axis
     * \param xdir Direction X coordinate of profil axis
     * \param ydir Direction Y coordinate of profil axis
     * \param zdir Direction Z coordinate of profil axis
     * \param dim Number of cells
     * \param resolution Size of a cell
     * \param na Value used to code NA
     * \param initValue Initialisation value for grid cells
     */
    CT_Profile(double xmin,
               double ymin,
               double zmin,
               double xdir,
               double ydir,
               double zdir,
               size_t dim,
               double resolution,
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
     *          - NAdata
     *          - dataMax
     *          - dataMin
     *          - data
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_Profile(const CT_Profile& other) = default;

    /*!
     * \brief Factory with origin and end of segment
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Origin X coordinate of profil axis
     * \param ymin Origin Y coordinate of profil axis
     * \param zmin Origin Z coordinate of profil axis
     * \param xmax End X coordinate of profil axis
     * \param ymax End Y coordinate of profil axis
     * \param zmax End Z coordinate of profil axis
     * \param dim Number of cells
     * \param resolution Size of a cell
     * \param na Value used to code NA
     * \param initValue Initialisation value for grid cells
     */
    static CT_Profile<DataT>* createProfileFromSegment(double xmin,
                                                       double ymin,
                                                       double zmin,
                                                       double xmax,
                                                       double ymax,
                                                       double zmax,
                                                       double resolution,
                                                       DataT na,
                                                       DataT initValue);

    /*!
     * \brief Destructor
     */
    ~CT_Profile() override;

    /*!
     * \brief Initialize all grid cells values with val
     *
     * Used by Constructors.
     *
     * \param val
     */
    void initGridWithValue(const DataT &val);

    /*!
     * \brief Generic setter taking a double as input value
     * \param index Index of cell to modify
     * \param value Double value to cast in effective type
     */
    virtual void setValueAtIndexFromDouble(const size_t &index, const double &value) override;

    /*!
     * \brief Compute min and max values
     */
    void computeMinMax() override;

    /**
      * \brief Gives the NA value
      * \return Valeur NA value
      */
    inline DataT NA() const {return _NAdata;}

    /*!
     * \brief Gives the max value of the grid
     * \return Max value
     */
    inline DataT dataMax() const { return _dataMax; }

    /*!
     * \brief Gives the min value of the grid
     * \return Min value
     */
    inline DataT dataMin() const { return _dataMin; }


    /*!
     * \brief Set value at specified index
     * \param index Index
     * \param value Value
     * \return True if the value has actually been set
     */
    bool setValueAtIndex(const size_t &index, const DataT &value);


    /**
      * \brief Set the value for the cell containing (x,y,z) coordinate to value
      * \param l Length coordinate
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setValueAtLength(const double &l, const DataT &value);

    /**
      * \brief Set the value for the cell corresponding to (X,Y,Z), projected on profil axis
      * \param x X coordinate in 3D
      * \param y Y coordinate in 3D
      * \param z Z coordinate in 3D
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setValueForXYZ(const double &x, const double &y, const double &z, const DataT &value);

    /*!
     * \brief ives the value at specified index
     * \param index Index
     * \return Value
     */
    DataT valueAtIndex(const size_t &index) const;

    /*!
     * \brief Generic [0;1]  (or -1 for NA) value accessor for use as CT_AbstractGrid3D
     * \param index index in the grid
     * \return A double value between 0 (min value) and 1 (max value), or -1 for NA
     */
    virtual double ratioValueAtIndex(const size_t &index) const override;


    /*!
     * \brief Return a double value for any type (or NAN for NA)
     * \param index index in the grid
     * \return A double value
     */
    virtual double valueAtIndexAsDouble(const size_t &index) const override;


    /*!
     * \brief Return a QString value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A QString représenting value
     */
    virtual QString valueAtIndexAsString(const size_t &index) const override;

    /*!
     * \brief return na value as a string
     *
     */
    virtual QString NAAsString() const override;

    /**
      * \brief Gives the value at length
      * \param l length
      * \return Value
      */
    DataT valueAtLength(const double &l) const;

    /**
      * \brief Gives the value at length
      * \param l length
      * \return Value
      */
    DataT valueForXYZ(const double &x, const double &y, const double &z) const;


    /**
      * \brief Set the value for the cell at specified index, if value is superior to actual one
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMaxValueAtIndex(const size_t &index, const DataT &value);

    /**
      * \brief Set the value for the cell at length l, if value is superior to actual one
      * \param l length
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMaxValueAtLength(const double &l, const DataT &value);

    /**
      * \brief Set the value for the cell corresponding to (X,Y,Z), projected on profil axis, if value is superior to actual one
      * \param x X coordinate in 3D
      * \param y Y coordinate in 3D
      * \param z Z coordinate in 3D
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMaxValueForXYZ(const double &x, const double &y, const double &z, const DataT &value);

    /**
      * \brief Set the value for the cell at specified index, if value is inferior to actual one
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMinValueAtIndex(const size_t &index, const DataT &value);

    /**
      * \brief Set the value for the cell  at length l, if value is inferior to actual one
      * \param l Length
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMinValueAtLength(const double &l, const DataT &value);

    /**
      * \brief Set the value for the cell corresponding to (X,Y,Z), projected on profil axis, if value is inferior to actual one
      * \param x X coordinate in 3D
      * \param y Y coordinate in 3D
      * \param z Z coordinate in 3D
      * \param value Value
      * \return True if the value has actually been set
      */
    bool setMinValueForXYZ(const double &x, const double &y, const double &z, const DataT &value);

    /**
      * \brief Add value to the cell at the specified index
      * \param index Index
      * \param value Value
      * \return True if the value has actually been set
      */
    bool addValueAtIndex(const size_t &index, const DataT &value);

    /**
      * \brief Add value to the cell at length l
      * \param l Length
      * \param value Value
      * \return True if the value has actually been updated
      */
    bool addValueAtLength(const double &l, const DataT &value);

    /**
      * \brief Add value to the cell corresponding to (X,Y,Z), projected on profil axis
      * \param x X coordinate in 3D
      * \param y Y coordinate in 3D
      * \param z Z coordinate in 3D
      * \param value Value
      * \return True if the value has actually been updated
      */
    bool addValueForXYZ(const double &x, const double &y, const double &z, const DataT &value);

    /**
      * \brief Gives neighbours values
      * \param l index, first one is 0
      * \param distance : Distance for neighbours search in cells
      * \param keepNAs : Should the NA values be kept ?
      * \param centermode : How should be treated the central cell ?
      * \return List of neighbours values
      */
    QList<DataT> neighboursValues(const size_t &index, const size_t &distance, const bool keepNAs = false, const CenterMode centermode = CT_Profile::CM_DropCenter) const;


    /**
      * \brief Standardize the profil
      *
      * After, the sum of the profil values is equal to 1
      *
      */
    void standardize();

    /**
      * \brief Compute OTSU threshold for the profil
      * \param outProfileLow  : copy of the profil, with 0 values under the threshold
      * \param outProfileHigh : copy of the profil, with 0 values over  the threshold
      * \return Threshold value (as a length on the profil)
      */
    double getOtsuThreshold(CT_Profile<DataT> *outProfileLow, CT_Profile<DataT> *outProfileHigh);

    CT_ITEM_COPY_IMP(CT_Profile<DataT>)

protected:
    DataT       _NAdata;            /*!< Valeur codant NA */

    DataT       _dataMax;           /*!< valeur maximale du grid*/
    DataT       _dataMin;           /*!< valeur minimale du grid*/
    std::vector<DataT> _data;       /*!< Tableau contenant les donnees pour chaque case de la grille*/

    const static CT_StandardProfileDrawManager<DataT> ABSPROFILE_DRAW_MANAGER;

private:
    size_t pNCells() const {return this->nCells();}
    double pMinX() const {return this->minX();}
    double pMinY() const {return this->minY();}
    double pMinZ() const {return this->minZ();}
    double pResolution() const {return this->resolution();}

    CT_DEFAULT_IA_BEGIN(CT_Profile<DataT>)
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataSize(), &CT_Profile<DataT>::pNCells, QObject::tr("Dimension"))
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataX(), &CT_Profile<DataT>::pMinX, QObject::tr("X origin"))
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataY(), &CT_Profile<DataT>::pMinY, QObject::tr("Y origin"))
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataZ(), &CT_Profile<DataT>::pMinZ, QObject::tr("Z origin"))
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataResolution(), &CT_Profile<DataT>::pResolution, QObject::tr("Resolution"))
    CT_DEFAULT_IA_V2(CT_Profile<DataT>, CT_AbstractCategory::staticInitDataNa(), &CT_Profile<DataT>::NA, QObject::tr("NA"))
    CT_DEFAULT_IA_END(CT_Profile<DataT>)
};

// Spécialisations

template<>
inline bool CT_Profile<bool>::NA() const {return false;}

template<>
CTLIBSTRUCTUREADDON_EXPORT void CT_Profile<bool>::computeMinMax();

template<>
CTLIBSTRUCTUREADDON_EXPORT double CT_Profile<bool>::ratioValueAtIndex(const size_t &index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT double CT_Profile<bool>::valueAtIndexAsDouble(const size_t &index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT QString CT_Profile<bool>::valueAtIndexAsString(const size_t &index) const;

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Profile<bool>::setMaxValueAtIndex(const size_t &index, const bool &value);

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Profile<bool>::setMinValueAtIndex(const size_t &index, const bool &value);

template<>
CTLIBSTRUCTUREADDON_EXPORT bool CT_Profile<bool>::addValueAtIndex(const size_t &index, const bool &value);

template<>
CTLIBSTRUCTUREADDON_EXPORT QList<bool> CT_Profile<bool>::neighboursValues(const size_t &index, const size_t &distance, const bool keepNAs, const CenterMode centermode) const;


// fin des spécialisations


// Includes the template implementations
#include "ct_itemdrawable/ct_profile.hpp"

#endif // CT_PROFILE_H
