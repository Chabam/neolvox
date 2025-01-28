#ifndef CT_GRID4D_SPARSE_H
#define CT_GRID4D_SPARSE_H

#include "ct_itemdrawable/ct_grid4d.h"
#include <typeinfo>

#include "opencv2/core/mat.hpp"

/*!
 * \class CT_Grid4D_Sparse
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing a 4D grid</b>
 *
 * It represents grid in 4D, with templated values in cells.
 */
template< typename DataT>
class   CT_Grid4D_Sparse  : public CT_Grid4D<DataT>
{
    CT_TYPE_TEMPLATED_IMPL_MACRO(CT_Grid4D_Sparse, DataT, CT_Grid4D<DataT>, 4D sparse grid)
    using SuperClass = CT_Grid4D<DataT>;

public:
    //**********************************************//
    //           Constructors/Destructors           //
    //**********************************************//
    /**
      * \brief Default constructor
      *  Each attribute will be set to 0, nullptr or will be cleared
      */
    CT_Grid4D_Sparse();
    CT_Grid4D_Sparse(const CT_Grid4D_Sparse& other);

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
    CT_Grid4D_Sparse(double wmin,
              double xmin,
              double ymin,
              double zmin,
              size_t dimw,
              size_t dimx,
              size_t dimy,
              size_t dimz,
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
    CT_Grid4D_Sparse(double wmin,
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
    static CT_Grid4D_Sparse<DataT>* createGrid4DFromWXYZCoords(double wmin,
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
    ~CT_Grid4D_Sparse() override;


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
        int levw, levx, levy, levz;
        if( !CT_AbstractGrid4D::indexToGrid( index, levw, levx, levy, levz ) )
        {
            return NA();
        }

        int idx[4] = { static_cast<int>(levw), static_cast<int>(levx), static_cast<int>(levy), static_cast<int>(levz) };

        return _data.operator ()( idx );
    }

    /*!
     * \brief Set value at specified index
     * \param index Index
     * \param value Value
     * \return True if the value has actually been set
     */
    inline bool setValueAtIndex(const size_t index, const DataT value) override
    {
        int levw, levx, levy, levz;
        if( !CT_AbstractGrid4D::indexToGrid( index, levw, levx, levy, levz ) )
        {
            return false;
        }

        int idx[4] = { static_cast<int>(levw), static_cast<int>(levx), static_cast<int>(levy), static_cast<int>(levz) };

        _data.ref( idx ) = value;

        return true;
    }


    //**********************************************//
    //          CompuTree Core and GUI tools        //
    //**********************************************//

    virtual void computeMinMax() override;

    inline cv::SparseMatConstIterator_<DataT> beginIterator() const { return _data.begin(); }
    inline cv::SparseMatConstIterator_<DataT> endIterator()  const { return _data.end(); }
    inline int countNonZeroCells() { return _data.nzcount(); }

    CT_ITEM_COPY_IMP(CT_Grid4D_Sparse<DataT>)

protected:

    // -------------------------------------------------------
    // Attributes
    // -------------------------------------------------------

    using CT_Grid4D<DataT>::nCells;
    using CT_Grid4D<DataT>::NA;
    using CT_Grid4D<DataT>::id;
    using CT_Grid4D<DataT>::alternativeDrawManager;
    using CT_Grid4D<DataT>::_top;
    using CT_Grid4D<DataT>::_bot;
    using CT_Grid4D<DataT>::_dimw;
    using CT_Grid4D<DataT>::_dimx;
    using CT_Grid4D<DataT>::_dimy;
    using CT_Grid4D<DataT>::_dimz;
    using CT_Grid4D<DataT>::_resw;
    using CT_Grid4D<DataT>::_resx;
    using CT_Grid4D<DataT>::_resy;
    using CT_Grid4D<DataT>::_resz;
    using CT_Grid4D<DataT>::_NAdata;
    using CT_Grid4D<DataT>::_dataMin;
    using CT_Grid4D<DataT>::_dataMax;
    cv::SparseMat_<DataT> _data;       /*!< Tableau contenant les donnees pour chaque case de la grille*/
};

// Includes the template implementations
#include "ct_itemdrawable/ct_grid4d_sparse.hpp"

#endif // CT_GRID4D_SPARSE_H
