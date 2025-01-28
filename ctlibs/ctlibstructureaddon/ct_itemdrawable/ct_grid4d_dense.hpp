#include "ct_itemdrawable/ct_grid4d_dense.h"
#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif
#include <math.h>
#include <typeinfo>

template< typename DataT>
CT_Grid4D_Dense<DataT>::CT_Grid4D_Dense() : SuperClass()
{
    int ncells[1];
    ncells[0] = static_cast<int>(this->nCells());
    this->_data.create(1, ncells);
}

template< typename DataT>
CT_Grid4D_Dense<DataT>::CT_Grid4D_Dense(const CT_Grid4D_Dense<DataT>& other) : SuperClass(other)
{
    _data = other._data.clone();
}

template< typename DataT>
CT_Grid4D_Dense<DataT>::CT_Grid4D_Dense(double wmin,
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
    int ncells[1];
    ncells[0] = static_cast<int>(this->nCells());
    this->_data.create(1, ncells);
    initGridWithValue(initValue);
}

template< typename DataT>
CT_Grid4D_Dense<DataT>::CT_Grid4D_Dense(double wmin,
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
    int ncells[1];
    ncells[0] = this->nCells();
    initGridWithValue(initValue);
}

template< typename DataT>
CT_Grid4D_Dense<DataT>* CT_Grid4D_Dense<DataT>::createGrid4DFromWXYZCoords(double wmin,
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
    int dimw = int(ceil((wmax - wmin)/resw));
    int dimx = int(ceil((xmax - xmin)/resx));
    int dimy = int(ceil((ymax - ymin)/resy));
    int dimz = int(ceil((zmax - zmin)/resz));

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

    return new CT_Grid4D_Dense<DataT>(wmin, xmin, ymin, zmin, dimw, dimx, dimy, dimz, resw, resx, resy, resz, na, initValue);
}

template< typename DataT>
CT_Grid4D_Dense<DataT>::~CT_Grid4D_Dense()
{
    _data.release();
}

template< typename DataT>
void CT_Grid4D_Dense<DataT>::initGridWithValue(const DataT val)
{
    for (size_t  i = 0 ; i < this->nCells() ; i++)
    {
        _data(i) = val;
    }
    this->_dataMin = val;
    this->_dataMax = val;
}
