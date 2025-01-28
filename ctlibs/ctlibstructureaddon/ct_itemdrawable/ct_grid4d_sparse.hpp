#include "ct_itemdrawable/ct_grid4d_sparse.h"
#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <typeinfo>

template< typename DataT>
CT_Grid4D_Sparse<DataT>::CT_Grid4D_Sparse() : SuperClass()
{
    const int nDims = 4;
    int ncells[nDims];
    ncells[0] = _dimw;
    ncells[1] = _dimx;
    ncells[2] = _dimy;
    ncells[3] = _dimz;
    this->_data.create(nDims, ncells);
}

template< typename DataT>
CT_Grid4D_Sparse<DataT>::CT_Grid4D_Sparse(const CT_Grid4D_Sparse<DataT>& other) : SuperClass(other)
{
    _data = other._data.clone();
}

template< typename DataT>
CT_Grid4D_Sparse<DataT>::CT_Grid4D_Sparse(double wmin,
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
                                        DataT initValue) : SuperClass(wmin,
                                                                            xmin,
                                                                            ymin,
                                                                            zmin,
                                                                            dimw,
                                                                            dimx,
                                                                            dimy,
                                                                            dimz,
                                                                            resw,
                                                                            resx,
                                                                            resy,
                                                                            resz,
                                                                            na)
{
    Q_UNUSED(initValue);

    const int nDims = 4;
    int ncells[nDims];
    ncells[0] = _dimw;
    ncells[1] = _dimx;
    ncells[2] = _dimy;
    ncells[3] = _dimz;
    this->_data.create(nDims, ncells);
}

template< typename DataT>
CT_Grid4D_Sparse<DataT>::CT_Grid4D_Sparse(double wmin,
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
                                        DataT initValue) : SuperClass(wmin,
                                                                            xmin,
                                                                            ymin,
                                                                            zmin,
                                                                            wmax,
                                                                            xmax,
                                                                            ymax,
                                                                            zmax,
                                                                            resw,
                                                                            resx,
                                                                            resy,
                                                                            resz,
                                                                            na)
{
    Q_UNUSED(initValue);

    const int nDims = 4;
    int ncells[nDims];
    ncells[0] = _dimw;
    ncells[1] = _dimx;
    ncells[2] = _dimy;
    ncells[3] = _dimz;
    this->_data.create(nDims, ncells);
}

template< typename DataT>
CT_Grid4D_Sparse<DataT>* CT_Grid4D_Sparse<DataT>::createGrid4DFromWXYZCoords(double wmin,
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
                                                                           DataT initValue)
{
    size_t dimw = ceil((wmax - wmin)/resw);
    size_t dimx = ceil((xmax - xmin)/resx);
    size_t dimy = ceil((ymax - ymin)/resy);
    size_t dimz = ceil((zmax - zmin)/resz);

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (wmax >= (wmin + dimw * resw))
    {
        dimw++;
    }

    while (xmax >= (xmin + dimx * resx))
    {
        dimx++;
    }

    while (ymax >= (ymin + dimy * resy))
    {
        dimy++;
    }

    while (zmax >= (zmin + dimz * resz))
    {
        dimz++;
    }

    return new CT_Grid4D_Sparse<DataT>(wmin, xmin, ymin, zmin, dimw, dimx, dimy, dimz, resw, resx, resy, resz, na, initValue);
}

template< typename DataT>
CT_Grid4D_Sparse<DataT>::~CT_Grid4D_Sparse()
{
    _data.release();
}

template< typename DataT>
void CT_Grid4D_Sparse<DataT>::computeMinMax()
{
    _dataMin = NA();
    _dataMax = NA();

    bool first = true;
    cv::SparseMatConstIterator it = _data.begin(), it_end = _data.end();
    for( ; it != it_end; ++it )
    {
        DataT val = it.value<DataT>();
        if (first)
        {
            _dataMin = val;
            _dataMax = val;
            first = false;
        } else {
            if (val < _dataMin) {_dataMin = val;}
            if (val > _dataMax) {_dataMax = val;}
        }
    }
}

