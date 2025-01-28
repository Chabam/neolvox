#ifndef CT_GRID4D_DENSE_H
#define CT_GRID4D_DENSE_H

#include "ct_itemdrawable/ct_grid4d.h"
#include <typeinfo>

#include "opencv2/core/core.hpp"

/*!
 * \class CT_Grid4D_Dense
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 4D grid</b>
 *
 * It represents grid in 4D, with templated values in cells.
 */
template< typename DataT>
class   CT_Grid4D_Dense  : public CT_Grid4D<DataT>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_Grid4D_Dense, DataT, CT_Grid4D<DataT>, 4D dense grid)
    using SuperClass = CT_Grid4D<DataT>;

public:
    //**********************************************//
    //           Constructors/Destructors           //
    //**********************************************//
    /**
      * \brief Default constructor
      *  Each attribute will be set to 0, nullptr or will be cleared
      */
    CT_Grid4D_Dense();
    CT_Grid4D_Dense(const CT_Grid4D_Dense& other);

    /*!
     * \brief Initialisation constructor
     *
     * Grid is created thanks to bottom left point of bounding box (4D) and number of cells along each dimension.
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param wmin Minimum W coordinate (bottom left corner)
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param dimw Number of w levels
     * \param dimx Number of colums
     * \param dimy Number of rows
     * \param dimz Number of zlevels
     * \param resw Length of a cell on w
     * \param resx Length of a cell on x
     * \param resy Length of a cell on y
     * \param resz Length of a cell on z
     * \param na Value used to code NA
     * \param initValue Initialisation value for grid cells
     */
    CT_Grid4D_Dense(double wmin,
              double xmin,
              double ymin,
              double zmin,
              int dimw,
              int dimx,
              int dimy,
              int dimz,
              double resw,
              double resx,
              double resy,
              double resz,
              DataT na,
              DataT initValue);

    /*!
     * \brief Initialisation constructor
     *
     * Grid is created thanks to the bounding box (4D) of the grid
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param wmin Minimum W coordinate (bottom left corner)
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param wmax Maximum W coordinate (top right corner)
     * \param xmax Maximum X coordinate (top right corner)
     * \param ymax Maximum Y coordinate (top right corner)
     * \param zmax Maximum Z coordinate (top right corner)
     * \param resw Length of a cell on w
     * \param resx Length of a cell on x
     * \param resy Length of a cell on y
     * \param resz Length of a cell on z
     * \param na Value used to code NA
     * \param initValue Initialisation value for grid cells
     * \param coordConstructor Not used, only to ensure constructor different signatures
     */
    CT_Grid4D_Dense(double wmin,
              double xmin,
              double ymin,
              double zmin,
              double wmax,
              double xmax,
              double ymax,
              double zmax,
              double resw,
              double resx,
              double resy,
              double resz,
              DataT na,
              DataT initValue);

    /*!
     * \brief Factory
     *
     * Grid is created thanks to the bounding box (4D) of the grid
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param wmin Minimum W coordinate (bottom left corner)
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param zmin Minimum Z coordinate (bottom left corner)
     * \param wmax Maximum W coordinate (top right corner)
     * \param xmax Maximum X coordinate (top right corner)
     * \param ymax Maximum Y coordinate (top right corner)
     * \param zmax Maximum Z coordinate (top right corner)
     * \param resw Length of a cell on w
     * \param resx Length of a cell on x
     * \param resy Length of a cell on y
     * \param resz Length of a cell on z
     * \param na Value used to code NA
     * \param initValue Initialisation value for grid cells
     * \param coordConstructor Not used, only to ensure constructor different signatures
     */
    static CT_Grid4D_Dense<DataT>* createGrid4DFromWXYZCoords(double wmin,
                                                        double xmin,
                                                        double ymin,
                                                        double zmin,
                                                        double wmax,
                                                        double xmax,
                                                        double ymax,
                                                        double zmax,
                                                        double resw,
                                                        double resx,
                                                        double resy,
                                                        double resz,
                                                        DataT na,
                                                        DataT initValue);

    /*!
     * \brief Destructor
     */
    virtual ~CT_Grid4D_Dense();

    //**********************************************//
    //              Constructor Tools               //
    //**********************************************//
    /*!
     * \brief Initialize all grid cells values with val
     *
     * Used by Constructors.
     *
     * \param val
     */
    void initGridWithValue(const DataT val);


    //**********************************************//
    //                  Getters Tools               //
    //**********************************************//
    /*!
     * \brief ives the value at specified index
     * \param index Index
     * \return Value
     */
    inline DataT valueAtIndex(const size_t index) const override
    {
        if ( index >= this->nCells() )
        {
            return this->NA();
        }

        return _data(static_cast<int>(index));
    }

    /*!
     * \brief Set value at specified index
     * \param index Index
     * \param value Value
     * \return True if the value has actually been set
     */
    inline bool setValueAtIndex(const size_t index, const DataT value) override
    {
        if ( index >= this->nCells() )
        {
            return false;
        }

        _data(static_cast<int>(index)) = value;

        return true;
    }


    //**********************************************//
    //          CompuTree Core and GUI tools        //
    //**********************************************//
    CT_ITEM_COPY_IMP(CT_Grid4D_Dense<DataT>)

protected:

    // -------------------------------------------------------
    // Attributes
    // -------------------------------------------------------

    cv::Mat_<DataT> _data;       /*!< Tableau contenant les donnees pour chaque case de la grille*/
};

// Includes the template implementations
#include "ct_itemdrawable/ct_grid4d_dense.hpp"

#endif // CT_GRID4D_DENSE_H
