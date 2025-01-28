#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid4ddrawmanager.h"
#include "ct_itemdrawable/ct_grid4d.h"
#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#endif
#include <math.h>
#include <typeinfo>

#include "qdebug.h"

template< typename DataT>
const CT_StandardGrid4DDrawManager<DataT> CT_Grid4D<DataT>::ABSGRID4D_DRAW_MANAGER;

template< typename DataT>
CT_Grid4D<DataT>::CT_Grid4D() : SuperClass()
{
    _NAdata = -1;
    _dataMax = -1;
    _dataMin = -1;

    setBaseDrawManager(&ABSGRID4D_DRAW_MANAGER);
}

template< typename DataT>
CT_Grid4D<DataT>::CT_Grid4D(double wmin,
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
                            DataT na) : SuperClass(wmin,
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
                                                   resz)
{
    _NAdata = na;
    _dataMax = -1;
    _dataMin = -1;

    setBaseDrawManager(&ABSGRID4D_DRAW_MANAGER);
}

template< typename DataT>
CT_Grid4D<DataT>::CT_Grid4D(double wmin,
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
                            DataT na) : SuperClass(wmin,
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
                                                   resz)
{
    _NAdata = na;
    _dataMax = -1;
    _dataMin = -1;

    setBaseDrawManager(&ABSGRID4D_DRAW_MANAGER);
}

template< typename DataT>
double CT_Grid4D<DataT>::ratioValueAtIndex(const size_t index) const
{
    if (_dataMax <= _dataMin)
    {
        return 1;
    }

    DataT value = valueAtIndex(index);

    if ( value == NA() )
    {
        return -1;
    }

    return double((value - _dataMin)/(_dataMax - _dataMin));
}

template< typename DataT>
DataT CT_Grid4D<DataT>::value(const int levw, const int levx, const int levy, const int levz) const
{
    size_t i;
    if( index( levw, levx, levy, levz, i) )
    {
        return valueAtIndex(i);
    }
    return NA();
}

template< typename DataT>
DataT CT_Grid4D<DataT>::valueAtWXYZ(const double w, const double x, const double y, const double z) const
{
    size_t i;
    if ( indexAtWXYZ(w, x, y, z, i) )
    {
        return valueAtIndex(i);
    }
    return false;
}

template< typename DataT>
bool CT_Grid4D<DataT>::addValueAtIndex(const size_t index, const DataT value)
{
    if ( index >= nCells() )
    {
        return false;
    }

    DataT currentValue = valueAtIndex(index);

    if (currentValue == NA())
    {
        return setValueAtIndex(index, value);
    }

    else if ( value != NA() )
    {
        return setValueAtIndex(index, value + currentValue);
    }

    return false;
}

template< typename DataT>
bool CT_Grid4D<DataT>::addValueAtWXYZ(const double w, const double x, const double y, const double z, const DataT value)
{
    size_t i;

    if( indexAtWXYZ(w, x, y, z, i) )
    {
        return addValueAtIndex(i, value);
    }

    return false;
}

template< typename DataT>
bool CT_Grid4D<DataT>::addValue( int levw, int levx, int levy, int levz, DataT value )
{
    size_t i;
    if( index( levw, levx, levy, levz, i) )
    {
        setValueAtIndex(i, valueAtIndex(i) + value);
        return true;
    }
    return false;
}

template< typename DataT>
bool CT_Grid4D<DataT>::neighboursValues(const int levw,
                                        const int levx,
                                        const int levy,
                                        const int levz,
                                        const int distance,
                                        QList<DataT>& outNeighboursValues,
                                        const bool keepNAs,
                                        const CenterMode centermode) const
{
    outNeighboursValues.clear();

    if (distance < 1) {return false;}
    if (levw >= _dimw || levw < 0 ) {return false;}
    if (levx >= _dimx || levx < 0 ) {return false;}
    if (levy >= _dimy || levy < 0 ) {return false;}
    if (levz >= _dimz || levz < 0 ) {return false;}

    int firstLevw = levw-distance;
    int lastLevw = levw+distance;
    int firstLevx = levx-distance;
    int lastLevx = levx+distance;
    int firstLevy = levy-distance;
    int lastLevy = levy+distance;
    int firstLevz = levz-distance;
    int lastLevz = levz+distance;

    if (firstLevw < 0) {firstLevw = 0;}
    if (lastLevw >= _dimw) {lastLevw = _dimw - 1;}
    if (firstLevx < 0) {firstLevx = 0;}
    if (lastLevx >= _dimx) {lastLevx = _dimx - 1;}
    if (firstLevy < 0) {firstLevy = 0;}
    if (lastLevy >= _dimy) {lastLevy = _dimy - 1;}
    if (firstLevz < 0) {firstLevz = 0;}
    if (lastLevz >= _dimz) {lastLevz = _dimz - 1;}


    for (int ww = firstLevw ; ww <= lastLevw ; ww++)
    {
        for (int xx = firstLevx ; xx <= lastLevx ; xx++)
        {
            for (int yy = firstLevy ; yy <= lastLevy ; yy++)
            {
                for (int zz = firstLevz ; zz <= lastLevz ; zz++)
                {
                    DataT val = value(ww, xx, yy, zz);

                    if ( (ww == levw) && (xx == levx) && (yy == levy) && (zz == levz) )
                    {
                        if( centermode == CM_KeepCenter )
                        {
                            if ( (val != NA()) || keepNAs )
                            {
                                outNeighboursValues.append(val);
                            }
                        }

                        else if ( centermode == CM_NAasCenter )
                        {
                            outNeighboursValues.append(NA());
                        }
                    }

                    else
                    {
                        if ( (val != NA()) || keepNAs )
                        {
                            outNeighboursValues.append(val);
                        }
                    }
                }
            }
        }
    }

    return true;
}

template< typename DataT>
QString CT_Grid4D<DataT>::valueAtIndexAsString(const size_t index) const
{
    DataT value = valueAtIndex(index);
    return QString::number(value);
}

template< typename DataT>
QString CT_Grid4D<DataT>::NAAsString() const
{
    return QString::number(NA());
}

template< typename DataT>
void CT_Grid4D<DataT>::computeMinMax()
{
    size_t ncells = nCells();
    if (ncells > 0)
    {
        _dataMin = valueAtIndex(0);
        _dataMax = _dataMin;

        for (size_t i = 1 ; i < ncells ; i++)
        {
            DataT val = valueAtIndex(i);
            if (val != NA())
            {
                if (_dataMax==NA() || val > _dataMax) {_dataMax = val;}
                if (_dataMin==NA() || val < _dataMin) {_dataMin = val;}
            }
        }
    }
}
