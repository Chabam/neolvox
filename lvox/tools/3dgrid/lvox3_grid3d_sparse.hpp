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

#ifdef USE_OPENCV

#include "tools/3dgrid/lvox3_grid3d_sparse.h"
#include "tools/drawmanager/lvox3_standardgrid3d_sparsedrawmanager.h"

#include <math.h>
#include <typeinfo>
#include <limits>

#include "qdebug.h"

template< typename DataT>
const LVOX3_StandardGrid3D_SparseDrawManager<DataT> LVOX3_Grid3D_Sparse<DataT>::GRID3D_SPARSE_DRAW_MANAGER;

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::LVOX3_Grid3D_Sparse() : LVOX3_AbstractGrid3D()
{
    this->_minCoordinates(0) = 0;
    this->_minCoordinates(1) = 0;
    this->_minCoordinates(2) = 0;
    _resx = 1;
    _resy = 1;
    _resz = 1;
    _dimx = 1;
    _dimy = 1;
    _dimz = 1;
    this->_maxCoordinates(0) = 0;
    this->_maxCoordinates(1) = 0;
    this->_maxCoordinates(2) = 0;
    _NAdata = -1;
    _initData = -1;
    _dataMax = -1;
    _dataMin = -1;

    int ncells[1];
    ncells[0] = 1;
    this->_data.create(1, ncells);

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::LVOX3_Grid3D_Sparse(const CT_OutAbstractSingularItemModel *model,
                            const CT_AbstractResult *result,
                            double xmin,
                            double ymin,
                            double zmin,
                            size_t dimx,
                            size_t dimy,
                            size_t dimz,
                            double resolutionx,
                            double resolutiony,
                            double resolutionz,
                            DataT na,
                            DataT initValue) : LVOX3_AbstractGrid3D(model, result)
{
    this->_minCoordinates(0) = xmin;
    this->_minCoordinates(1) = ymin;
    this->_minCoordinates(2) = zmin;

    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;

    this->_maxCoordinates(0) = minX() + _resx * _dimx;
    this->_maxCoordinates(1) = minY() + _resy * _dimy;
    this->_maxCoordinates(2) = minZ() + _resz * _dimz;

    _NAdata = na;
    _initData = initValue;

    setCenterX (minX() + (maxX() - minX())/2.0);
    setCenterY (minY() + (maxY() - minY())/2.0);
    setCenterZ (minZ() + (maxZ() - minZ())/2.0);

    int ncells[1];
    ncells[0] = 1;
    this->_data.create(1, ncells);

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::LVOX3_Grid3D_Sparse(const QString &modelName,
                            const CT_AbstractResult *result,
                            double xmin,
                            double ymin,
                            double zmin,
                            size_t dimx,
                            size_t dimy,
                            size_t dimz,
                            double resolutionx,
                            double resolutiony,
                            double resolutionz,
                            DataT na,
                            DataT initValue) : LVOX3_AbstractGrid3D(modelName, result)
{
    this->_minCoordinates(0) = xmin;
    this->_minCoordinates(1) = ymin;
    this->_minCoordinates(2) = zmin;

    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;

    this->_maxCoordinates(0) = minX() + _resx * _dimx;
    this->_maxCoordinates(1) = minY() + _resy * _dimy;
    this->_maxCoordinates(2) = minZ() + _resz * _dimz;

    _NAdata = na;
    _initData = initValue;

    setCenterX (minX() + (maxX() - minX())/2.0);
    setCenterY (minY() + (maxY() - minY())/2.0);
    setCenterZ (minZ() + (maxZ() - minZ())/2.0);

    int ncells[1];
    ncells[0] = 1;
    this->_data.create(1, ncells);

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::LVOX3_Grid3D_Sparse(const CT_OutAbstractSingularItemModel *model,
                            const CT_AbstractResult *result,
                            double xmin,
                            double ymin,
                            double zmin,
                            double xmax,
                            double ymax,
                            double zmax,
                            double resolutionx,
                            double resolutiony,
                            double resolutionz,
                            DataT na,
                            DataT initValue) : LVOX3_AbstractGrid3D(model, result)
{
    this->_minCoordinates(0) = (xmin);
    this->_minCoordinates(1) = (ymin);
    this->_minCoordinates(2) = (zmin);

    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;

    _dimx = ceil((xmax - xmin)/_resx);
    _dimy = ceil((ymax - ymin)/_resy);
    _dimz = ceil((zmax - zmin)/_resz);

    this->_maxCoordinates(0) = (minX() + _resx * _dimx);
    this->_maxCoordinates(1) = (minY() + _resy * _dimy);
    this->_maxCoordinates(2) = (minZ() + _resz * _dimz);

    _NAdata = na;
    _initData = initValue;

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= maxX())
    {
        _dimx++;
        this->_maxCoordinates(0) = (maxX() + _resx);
    }

    while (ymax >= maxY())
    {
        _dimy++;
        this->_maxCoordinates(1) = (maxY() + _resy);
    }

    while (zmax >= maxZ())
    {
        _dimz++;
        this->_maxCoordinates(2) = (maxZ() + _resz);
    }

    setCenterX (minX() + (maxX() - minX())/2.0);
    setCenterY (minY() + (maxY() - minY())/2.0);
    setCenterZ (minZ() + (maxZ() - minZ())/2.0);

    int ncells[1];
    ncells[0] = 1;
    this->_data.create(1, ncells);

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::LVOX3_Grid3D_Sparse(const QString& model,
                            const CT_AbstractResult *result,
                            double xmin,
                            double ymin,
                            double zmin,
                            double xmax,
                            double ymax,
                            double zmax,
                            double resolutionx,
                            double resolutiony,
                            double resolutionz,
                            DataT na,
                            DataT initValue) : LVOX3_AbstractGrid3D(model, result)
{
    this->_minCoordinates(0) = (xmin);
    this->_minCoordinates(1) = (ymin);
    this->_minCoordinates(2) = (zmin);

    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;

    _dimx = ceil((xmax - xmin)/_resx);
    _dimy = ceil((ymax - ymin)/_resy);
    _dimz = ceil((zmax - zmin)/_resz);

    this->_maxCoordinates(0) = (minX() + _resx * _dimx);
    this->_maxCoordinates(1) = (minY() + _resy * _dimy);
    this->_maxCoordinates(2) = (minZ() + _resz * _dimz);

    _NAdata = na;
    _initData = initValue;

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= maxX())
    {
        _dimx++;
        this->_maxCoordinates(0) = (maxX() + _resx);
    }

    while (ymax >= maxY())
    {
        _dimy++;
        this->_maxCoordinates(1) = (maxY() + _resy);
    }

    while (zmax >= maxZ())
    {
        _dimz++;
        this->_maxCoordinates(2) = (maxZ() + _resz);
    }

    setCenterX (minX() + (maxX() - minX())/2.0);
    setCenterY (minY() + (maxY() - minY())/2.0);
    setCenterZ (minZ() + (maxZ() - minZ())/2.0);

    int ncells[1];
    ncells[0] = 1;
    this->_data.create(1, ncells);

    setBaseDrawManager(&GRID3D_SPARSE_DRAW_MANAGER);
}


template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>* LVOX3_Grid3D_Sparse<DataT>::createGrid3DFromXYZCoords(const CT_OutAbstractSingularItemModel *model,
                                                              const CT_AbstractResult *result,
                                                              double xmin,
                                                              double ymin,
                                                              double zmin,
                                                              double xmax,
                                                              double ymax,
                                                              double zmax,
                                                              double resolutionx,
                                                              double resolutiony,
                                                              double resolutionz,
                                                              DataT na,
                                                              DataT initValue,
                                                              bool extends)
{
    size_t dimx = ceil((xmax - xmin)/resolutionx);
    size_t dimy = ceil((ymax - ymin)/resolutiony);
    size_t dimz = ceil((zmax - zmin)/resolutionz);

    if (extends)
    {
        // to ensure a point exactly on a maximum limit of the grid will be included in the grid
        while (xmax >= (xmin + dimx * resolutionx))
        {
            dimx++;
        }

        while (ymax >= (ymin + dimy * resolutiony))
        {
            dimy++;
        }

        while (zmax >= (zmin + dimz * resolutionz))
        {
            dimz++;
        }
    }


    return new LVOX3_Grid3D_Sparse<DataT>(model, result, xmin, ymin, zmin, dimx, dimy, dimz, resolutionx,resolutiony,resolutionz, na, initValue);
}

template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>* LVOX3_Grid3D_Sparse<DataT>::createGrid3DFromXYZCoords(const QString &modelName,
                                                              const CT_AbstractResult *result,
                                                              double xmin,
                                                              double ymin,
                                                              double zmin,
                                                              double xmax,
                                                              double ymax,
                                                              double zmax,
                                                              double resolutionx,
                                                              double resolutiony,
                                                              double resolutionz,
                                                              DataT na,
                                                              DataT initValue,
                                                              bool extends)
{
    size_t dimx = ceil((xmax - xmin)/resolutionx);
    size_t dimy = ceil((ymax - ymin)/resolutiony);
    size_t dimz = ceil((zmax - zmin)/resolutionz);

    if (extends)
    {
        // to ensure a point exactly on a maximum limit of the grid will be included in the grid
        while (xmax >= (xmin + dimx * resolutionx))
        {
            dimx++;
        }

        while (ymax >= (ymin + dimy * resolutiony))
        {
            dimy++;
        }

        while (zmax >= (zmin + dimz * resolutionz))
        {
            dimz++;
        }
    }

    return new LVOX3_Grid3D_Sparse<DataT>(modelName, result, xmin, ymin, zmin, dimx, dimy, dimz, resolutionx, resolutiony, resolutionz, na, initValue);
}


template< typename DataT>
LVOX3_Grid3D_Sparse<DataT>::~LVOX3_Grid3D_Sparse()
{
    _data.release();
}

template< typename DataT>
void LVOX3_Grid3D_Sparse<DataT>::setValueAtIndexFromDouble(const size_t &index, const double &value)
{
    setValueAtIndex(index, (DataT) value);
}


template< typename DataT>
void LVOX3_Grid3D_Sparse<DataT>::computeMinMax()
{  
    _dataMin = _initData;
    _dataMax = _initData;

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

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}
    _data.ref(index) = value;

    return true;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setValue(const size_t colx, const size_t liny, const size_t levz, const DataT value)
{
    size_t i;
    if (index(colx, liny, levz, i))
    {
        return setValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setValueAtIndex(i, value);
    }
    return false;
}


template< typename DataT>
DataT LVOX3_Grid3D_Sparse<DataT>::valueAtIndex(const size_t index) const
{
    if (index >= nCells()) {return NA();}

    const DataT* val = _data.template find<DataT>(index);
    if (val == NULL) {return _initData;}

    return _data(index);
}

template< typename DataT>
double LVOX3_Grid3D_Sparse<DataT>::ratioValueAtIndex(const size_t index) const
{
    if (_dataMax <= _dataMin) {return 1;}
    DataT value = valueAtIndex(index);
    if (value == NA()) {return -1;}
    return (double) (((double)(value - _dataMin))/((double)(_dataMax - _dataMin)));
}

template< typename DataT>
double LVOX3_Grid3D_Sparse<DataT>::valueAtIndexAsDouble(const size_t index) const
{
    DataT value = valueAtIndex(index);
    if (value == NA()) {return std::numeric_limits<DataT>::quiet_NaN();}
    return value;
}

template< typename DataT>
QString LVOX3_Grid3D_Sparse<DataT>::valueAtIndexAsString(const size_t index) const
{
    DataT value = valueAtIndex(index);
    return QVariant(value).toString();
}

template< typename DataT>
QString LVOX3_Grid3D_Sparse<DataT>::NAAsString() const
{
    return QVariant(NA()).toString();
}

template< typename DataT>
DataT LVOX3_Grid3D_Sparse<DataT>::value(const size_t colx, const size_t liny, const size_t levz) const
{
    size_t i;
    if (index(colx, liny, levz, i))
    {
        return valueAtIndex(i);
    }
    return NA();
}

template< typename DataT>
DataT LVOX3_Grid3D_Sparse<DataT>::dataFromArray(const size_t &x, const size_t &y, const size_t &z) const
{
    size_t i;
    if (index(x, y, z, i))
    {
        return valueAtIndex(i);
    }
    return NA();
}

template< typename DataT>
DataT LVOX3_Grid3D_Sparse<DataT>::dataFromArray(const size_t &index) const
{
    if (index >= nCells()) {return NA();}

    const DataT* val = _data.template find<DataT>(index);
    if (val == NULL) {return _initData;}

    return _data(index);
}

template< typename DataT>
DataT LVOX3_Grid3D_Sparse<DataT>::valueAtXYZ(const double x, const double y, const double z) const
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return valueAtIndex(i);
    }
    return false;
}


template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setMaxValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data(index);

    if ((currentValue == NA()) || (value > currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setMaxValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setMaxValueAtIndex(i, value);
    }
    return false;
}


template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setMinValueAtIndex(const size_t index, DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data(index);

    if ((currentValue == NA()) || (value < currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::setMinValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setMinValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::addValueAtIndex(const size_t index, DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data(index);

    if (currentValue == NA())
    {
        return setValueAtIndex(index, value);
    } else if (value != NA())
    {
        return setValueAtIndex(index, value + currentValue);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D_Sparse<DataT>::addValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return addValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
QList<DataT> LVOX3_Grid3D_Sparse<DataT>::neighboursValues(const size_t colx, const size_t liny, const size_t levz, const size_t distance, const bool keepNAs, const CenterMode centermode) const
{
    QList<DataT> liste;

    if (distance < 1) {return liste;}
    if (colx >= _dimx) {return liste;}
    if (liny >= _dimy) {return liste;}
    if (levz >= _dimz) {return liste;}

    size_t firstColx = colx-distance;
    size_t lastColx = colx+distance;
    size_t firstLiny = liny-distance;
    size_t lastLiny = liny+distance;
    size_t firstLinz = levz-distance;
    size_t lastLinz = levz+distance;

    if (firstColx >= _dimx) {firstColx = 0;}
    if (lastColx >= _dimx) {lastColx = _dimx - 1;}
    if (firstLiny >= _dimy) {firstLiny = 0;}
    if (lastLiny >= _dimy) {lastLiny = _dimy - 1;}
    if (firstLinz >= _dimz) {firstLinz = 0;}
    if (lastLinz >= _dimz) {lastLinz = _dimz - 1;}


    for (size_t xx = firstColx ; xx <= lastColx ; xx++)
    {
        for (size_t yy = firstLiny ; yy <= lastLiny ; yy++)
        {
            for (size_t zz = firstLinz ; zz <= lastLinz ; zz++)
            {

                DataT val = value(xx, yy, zz);
                if ((xx == colx) && (yy == liny) && (zz == levz)) {
                    if (centermode == CM_KeepCenter) {
                        if ((val != NA()) || keepNAs) {liste.append(val);}
                    } else if (centermode == CM_NAasCenter) {
                        liste.append(NA());
                    }
                } else {
                    if ((val != NA()) || keepNAs) {liste.append(val);}
                }
            }
        }
    }

    return liste;
}

template< typename DataT>
void LVOX3_Grid3D_Sparse<DataT>::getIndicesWithData(QList<size_t> &list) const
{
    cv::SparseMatConstIterator it = _data.begin(), it_end = _data.end();
    for( ; it != it_end; ++it )
    {
        size_t ind = it.node()->idx[0];
        list.append(ind);
    }
}

#endif
