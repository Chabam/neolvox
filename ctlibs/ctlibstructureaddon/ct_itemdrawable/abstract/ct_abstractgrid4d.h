#ifndef CT_ABSTRACTGRID4D_H
#define CT_ABSTRACTGRID4D_H

// The grid is an itemDrawableWithoutPointCloud
#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"

// Using vector4d to store bounding box
#include <Eigen/Core>

// Using floor()
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

/*!
 * \class CT_AbstractGrid4D
 * \ingroup PluginShared_Items
 * \brief <b>Commun abstract base of all templated CT_AbstractGrid4D<type> </b>
 *
 * It's usefull to manage generically a grid, without knowing it template type
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractGrid4D : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractGrid4D, CT_AbstractItemDrawableWithoutPointCloud, 4D grid)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
//**********************************************//
//           Constructors/Destructors           //
//**********************************************//
    /*!
     * \brief CT_AbstractGrid4D
     *  Default constructor
     *  Each attribute will be set to 0, nullptr or will be cleared
     */
    CT_AbstractGrid4D();

    CT_AbstractGrid4D(double wmin,
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
                      double resz);

    CT_AbstractGrid4D(double wmin,
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
                      double resz);

    CT_AbstractGrid4D(const CT_AbstractGrid4D& other) = default;


//**********************************************//
//                    Getters                   //
//**********************************************//

    /**
      * \brief Gives the number of w level
      * \return Number of w level
      */
    inline int wdim() const {return _dimw;}

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

    /**
      * \brief Gives the length of a voxel along the w dimension w level
      * \return Lenght of a voxel along w dimension
      */
    inline double wres() const {return _resw;}

    /**
      * \brief Gives the length of a voxel along the x dimension x level
      * \return Lenght of a voxel along x dimension
      */
    inline double xres() const {return _resx;}

    /**
      * \brief Gives the length of a voxel along the y dimension y level
      * \return Lenght of a voxel along y dimension
      */
    inline double yres() const {return _resy;}

    /**
      * \brief Gives the length of a voxel along the z dimension z level
      * \return Lenght of a voxel along z dimension
      */
    inline double zres() const {return _resz;}

    /**
      * \brief Getter
      * \return the lower w coordinate of the grid
      */
    inline double minW() const {return _bot(0);}

    /**
      * \brief Getter
      * \return the upper w coordinate of the grid
      */
    inline double maxW() const {return _top(0);}

//**********************************************//
//              Getters Tools                   //
//**********************************************//
    /*!
     * \brief Total number of cells for the grid
     * \return Number of cells
     */
    inline size_t nCells() const {return size_t(_dimw)*size_t(_dimx)*size_t(_dimy)*size_t(_dimz);}

    /*!
     * \brief Return a [0;1] value for any type (or -1 for NA)
     * \param index index in the grid
     * \return A double value between 0 (min value) and 1 (max value)
     */
    virtual double ratioValueAtIndex(const size_t index) const = 0;

    /**
     * \brief getMinCoordinates
     * \param min Min coordinates of the grid (bottom left corner)
     */
    inline void getMinCoordinates(Eigen::Vector4d &min) const
    {
        min = _bot;
    }

    /**
     * \brief getMaxCoordinates
     * \param max Max coordinates of the grid (upper right corner)
     */
    inline void getMaxCoordinates(Eigen::Vector4d &max) const
    {
        max = _top;
    }

    /**
     * \brief getResolutions
     * \param res Resolutions of the grid (size of voxel along each axis)
     */
    inline void getResolutions(Eigen::Vector4d &res) const
    {
        res(0) = _resw;
        res(1) = _resx;
        res(2) = _resy;
        res(3) = _resz;
    }

    /**
     * \brief getDimensions
     * \param res Dimensions of the grid (size of voxel along each axis)
     */
    inline void getDimensions(Eigen::Vector4d &dim) const
    {
        dim(0) = _dimw;
        dim(1) = _dimx;
        dim(2) = _dimy;
        dim(3) = _dimz;
    }

    /*!
     * \brief Gives the W coordinate of the center of cells of level levw
     * \param levw Column, first one is 0
     * \return W coordinate
     */
    inline double getCellCenterW(const int levw) const
    {
        return _bot(0) + (double(levw))*_resw + _resw/2;
    }

    /*!
     * \brief Gives the X coordinate of the center of cells of level levx
     * \param levx Column, first one is 0
     * \return X coordinate
     */
    inline double getCellCenterX(const int levx) const
    {
        return _bot(1) + (double(levx))*_resx + _resx/2;
    }

    /*!
     * \brief Gives the Y coordinate of the center of cells of level levy
     * \param levy Column, first one is 0
     * \return Y coordinate
     */
    inline double getCellCenterY(const int levy) const
    {
        return _bot(2) + (double(levy))*_resy + _resy/2;
    }

    /*!
     * \brief Gives the Z coordinate of the center of cells of level levz
     * \param levz Column, first one is 0
     * \return Z coordinate
     */
    inline double getCellCenterZ(const int levz) const
    {
        return _bot(3) + (double(levz))*_resz + _resz/2;
    }

    /*!
     * \brief getCellCoordinates Give min and max coordinates of a cell
     * \param index Index of the cell
     * \param bottom  Min coordinates
     * \param top Max coordinates
     * \return true if index is in the grid
     */
    inline bool getCellCoordinates(const size_t index, Eigen::Vector4d &bottom, Eigen::Vector4d &top) const
    {
        int levw, levx, levy, levz;
        if( !indexToGrid(index, levw, levx, levy, levz) )
        {
            return false;
        }

        bottom(0) = _bot(0) + levw*_resw;
        bottom(1) = _bot(1) + levx*_resx;
        bottom(2) = _bot(2) + levy*_resy;
        bottom(3) = _bot(3) + levz*_resz;

        top(0) = bottom(0) + _resw;
        top(1) = bottom(1) + _resx;
        top(2) = bottom(2) + _resy;
        top(3) = bottom(3) + _resz;

        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell defined by (levw, levx, levy, levz)
     * \param levw W level
     * \param levx X level
     * \param levy Y level
     * \param levz Z level
     * \param bottom Output coordinates
     * \return A Eigen::Vector4d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCorner(const int levw, const int levx, const int levy, const int levz, Eigen::Vector4d &bottom) const
    {
        if( (levw >= _dimw) || (levx >= _dimx) || (levy >= _dimy) || (levz >= _dimz) )
        {
            return false;
        }

        bottom(0) = _bot(0) + levw*_resw;
        bottom(1) = _bot(1) + levx*_resx;
        bottom(2) = _bot(2) + levy*_resy;
        bottom(3) = _bot(3) + levz*_resz;

        return true;
    }

    /*!
     * \brief Get the left corner coordinates of the cell containing (w, x, y, z)
     * \param w W coordinate
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \param bottom Output coordinates
     * \return A Eigen::Vector4d coordinates for the bottom left corner
     */
    inline bool getCellBottomLeftCornerAtXYZ(const double w, const double x, const double y, const double z, Eigen::Vector4d &bottom) const
    {

        return getCellBottomLeftCorner( int(floor( (w - _bot(0) ) / _resw)),
                                        int(floor( (x - _bot(1) ) / _resx)),
                                        int(floor( (y - _bot(2) ) / _resy)),
                                        int(floor( (z - _bot(3) ) / _resz)),
                                        bottom);
    }

//**********************************************//
//                   Setters                    //
//**********************************************//

//**********************************************//
//                  Setter Tools                //
//**********************************************//
    /*!
     * \brief Generic setter taking a double as input value
     * \param index Index of cell to modify
     * \param value Double value to cast in effective type
     */
    virtual void setValueAtIndexFromDouble(const size_t &index, const double &value) = 0;

//**********************************************//
//                Generic Tools                 //
//**********************************************//
    /*!
     * \brief Compute min and max values
     */
    virtual void computeMinMax() = 0;

//**********************************************//
//          Coordinates transformation          //
//**********************************************//
    /*!
     * \brief Compute index for given levels on (w,x,y,z)
     * \param levw w level (0 if the first)
     * \param levx x level (0 if the first)
     * \param levy y level (0 if the first)
     * \param levz z level (0 if the first)
     * \param index Returned index
     * \return false if index is invalid
     */
    inline bool index(const int levw, const int levx, const int levy, const int levz, size_t &returnedIndex) const
    {
        if( (levw >= _dimw) || (levx >= _dimx) || (levy >= _dimy) || (levz >= _dimz) )
        {
            return false;
        }

        else
        {
            returnedIndex = size_t(levw)*size_t(_dimx)*size_t(_dimy)*size_t(_dimz) + size_t(levx)*size_t(_dimy)*size_t(_dimz) + size_t(levy)*size_t(_dimz) + size_t(levz);
            return true;
        }
    }

    /*!
     * \brief Return column for specified w coordinate in cartesian system
     * \param w W coordinate
     * \param levw Returned level on W containing w
     * \return false if w invalid
     */
    inline bool levW(const double w, int &levw) const
    {
        if ( w < _bot(0) || w > _top(0) )
        {
            return false;
        }

        if ( qFuzzyCompare(w, _top(0)) )
        {
            levw =  _dimw - 1;
        }

        else
        {
            levw = int(floor( (w - _bot(0)) / _resw ));
        }
        return true;
    }

    /*!
     * \brief Return column for specified x coordinate in cartesian system
     * \param x X coordinate
     * \param levx Returned level on X containing x
     * \return false if x invalid
     */
    inline bool levX(const double x, int &levx) const
    {
        if ( x < _bot(1) || x > _top(1) )
        {
            return false;
        }

        if ( qFuzzyCompare(x, _top(1)) )
        {
            levx =  _dimx - 1;
        }

        else
        {
            levx = int(floor( (x - _bot(1)) / _resx ));
        }
        return true;
    }

    /*!
     * \brief Return column for specified y coordinate in cartesian system
     * \param y Y coordinate
     * \param levy Returned level on Y containing y
     * \return false if y invalid
     */
    inline bool levY(const double y, int &levy) const
    {
        if ( y < _bot(2) || y > _top(2) )
        {
            return false;
        }

        if ( qFuzzyCompare(y, _top(2)) )
        {
            levy =  _dimy - 1;
        }

        else
        {
            levy = int(floor( (y - _bot(2)) / _resy ));
        }
        return true;
    }

    /*!
     * \brief Return column for specified z coordinate in cartesian system
     * \param z Z coordinate
     * \param levz Returned level on Z containing z
     * \return false if z invalid
     */
    inline bool levZ(const double z, int &levz) const
    {
        if ( z < _bot(3) || z > _top(3) )
        {
            return false;
        }

        if ( qFuzzyCompare(z, _top(3)) )
        {
            levz =  _dimz - 1;
        }

        else
        {
            levz = int(floor( (z - _bot(3)) / _resz ));
        }
        return true;
    }

    /*!
     * \brief indexToGrid Convert index in (levw, levx, levy, levz) grid coordinates
     * \param index Index of the cell
     * \param levw Resulting level of the cell along w
     * \param levx Resulting level of the cell along x
     * \param levy Resulting level of the cell along y
     * \param levz Resulting level of the cell along z
     * \return true if the index is in the grid
     */
    inline bool indexToGrid(const size_t index, int &levw, int &levx, int &levy, int &levz) const
    {
        if ( index >= nCells() )
        {
            return false;
        }

        int mod;

        levw = int(index / size_t(_dimx)*size_t(_dimy)*size_t(_dimz));
        mod = int(index % size_t(_dimx)*size_t(_dimy)*size_t(_dimz));

        levx = int(mod / (_dimy*_dimz));
        mod = int(mod % (_dimy*_dimz));

        levy = int(mod / _dimz);
        mod = int(mod % _dimz);

        levz = mod;

        return true;
    }

    /*!
     * \brief Compute index for given (w, x, y, z) coordinate in cartesian system
     * \param w W coordinate
     * \param x X coordinate
     * \param y Y coordinate
     * \param z Z coordinate
     * \param index Returned index
     * \return false if index is invalid
     */
    inline bool indexAtWXYZ(const double w, const double x, const double y, const double z, size_t &returnedIndex) const
    {
        int levw, levx, levy, levz;
        if ( !levW(w, levw) || !levX(x, levx) || !levY(y, levy) || !levZ(z, levz) )
        {
            return false;
        }

        return index(levw, levx, levy, levz, returnedIndex);
    }

    /*!
     * \brief coordAtWXYZ Convert index for given (w, x, y, z) coordinate in cartesian system to grid coordinates
     * \return true if the index is in the grid
     */
    inline bool coordAtWXYZ(const double w, const double x, const double y, const double z,
                                int& outLevW, int& outLevX, int& outLevY, int& outLevZ) const
    {
        if ( !levW( w, outLevW ) )
        {
            return false;
        }

        if ( !levX( x, outLevX ) )
        {
            return false;
        }

        if ( !levY( y, outLevY ) )
        {
            return false;
        }

        if ( !levZ( z, outLevZ ) )
        {
            return false;
        }

        return true;
    }

    virtual QString valueAtIndexAsString(const size_t index) const = 0;

    virtual QString NAAsString() const = 0;

    Eigen::Vector4d boundingBoxBot() const { return _bot; }
    void setBoundingBoxBot(const Eigen::Vector4d &bot) { _bot = bot; }

    Eigen::Vector4d boundingBoxTop() const { return _top; }
    void setBoundingBoxTop(const Eigen::Vector4d &top) { _top = top; }

    Eigen::Vector4d res() const { return Eigen::Vector4d( _resw, _resx, _resy, _resz ); }
    void setRes(const Eigen::Vector4d &res) { _resw = res.w();
                                              _resx = res.x();
                                              _resy = res.y();
                                              _resz = res.z(); }

    Eigen::Vector4i dim() const { return Eigen::Vector4i( _dimw, _dimx, _dimy, _dimz ); }
    void setDim(const Eigen::Vector4i &dim) { _dimw = dim.w();
                                              _dimx = dim.x();
                                              _dimy = dim.y();
                                              _dimz = dim.z();
                                            }

protected:
//**********************************************//
//                  Attributes                  //
//**********************************************//
    Eigen::Vector4d     _bot;       /*!< 4D Bounding box of the Grid*/
    Eigen::Vector4d     _top;       /*!< 4D Bounding box of the Grid*/
    int                 _dimw;      /*!< Nombre de cases selon w du grid*/
    int                 _dimx;      /*!< Nombre de cases selon x du grid*/
    int                 _dimy;      /*!< Nombre de cases selon y du grid*/
    int                 _dimz;      /*!< Nombre de cases selon z du grid*/
    double              _resx;      /*!< Resolution de la grille (taille d'une case en x) */
    double              _resy;      /*!< Resolution de la grille (taille d'une case en y) */
    double              _resz;      /*!< Resolution de la grille (taille d'une case en z) */
    double              _resw;      /*!< Resolution de la grille (taille d'une case en r) */

};

#endif // CT_ABSTRACTGRID4D_H
