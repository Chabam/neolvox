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

#include "tools/3dgrid/lvox3_grid3d.h"
#include "tools/drawmanager/lvox3_standardgrid3ddrawmanager.h"

#include <math.h>
#include <typeinfo>
#include <limits>

#include "ct_math/ct_mathpoint.h"
#include "qdebug.h"

template< typename DataT>
const LVOX3_StandardGrid3DDrawManager<DataT> LVOX3_Grid3D<DataT>::ABSGRID3D_DRAW_MANAGER;

template< typename DataT>
LVOX3_Grid3D<DataT>::LVOX3_Grid3D() : LVOX3_AbstractGrid3D()
{    
    _resx = 1;
    _resy = 1;
    _resz = 1;
    _dimx = 0;
    _dimy = 0;
    _dimz = 0;

    _NAdata = -1;
    _dataMax = -1;
    _dataMin = -1;
    setBoundingBox(0, 0, 0, 0, 0, 0);
    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D<DataT>::LVOX3_Grid3D(
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
                            DataT initValue) : LVOX3_AbstractGrid3D()
{
    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;    

    _NAdata = na;

    setBoundingBox(xmin, ymin, zmin, xmin + _resx * _dimx, ymin + _resy * _dimy, zmin + _resz * _dimz);
    _data.resize(nCells());

    _sommaDelta.resize(nCells());
    _sommaFree.resize(nCells());
    //_sommaI.resize(nCells());
    _sommaEffectiveFree.resize(nCells());
    _sommaEffectiveDelta.resize(nCells());
    _sommaEffectiveHits.resize(nCells());
    _sommaDeltaSquare.resize(nCells());
    initGridWithValue(initValue);
    //_distances = Eigen::MatrixXd(nCells(),2);
    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D<DataT>::LVOX3_Grid3D(
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
                            DataT initValue) : LVOX3_AbstractGrid3D()
{

    Eigen::Vector3d minCoord;
    minCoord(0) = (xmin);
    minCoord(1) = (ymin);
    minCoord(2) = (zmin);

    _resx = resolutionx;
    _resy = resolutiony;
    _resz = resolutionz;

    _dimx = ceil((xmax - xmin)/_resx);
    _dimy = ceil((ymax - ymin)/_resy);
    _dimz = ceil((zmax - zmin)/_resz);

    Eigen::Vector3d maxCoord;
    maxCoord(0) = (minX() + _resx * _dimx);
    maxCoord(1) = (minY() + _resy * _dimy);
    maxCoord(2) = (minZ() + _resz * _dimz);

    _NAdata = na;

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= maxX())
    {
        _dimx++;
        maxCoord(0) = (maxX() + _resx);
    }

    while (ymax >= maxY())
    {
        _dimy++;
        maxCoord(1) = (maxY() + _resy);
    }

    while (zmax >= maxZ())
    {
        _dimz++;
        maxCoord(2) = (maxZ() + _resz);
    }

    setBoundingBox(minCoord, maxCoord);

    setCenterX (minX() + (maxX() - minX())/2.0);
    setCenterY (minY() + (maxY() - minY())/2.0);
    setCenterZ (minZ() + (maxZ() - minZ())/2.0);

    _data.resize(nCells());

    _sommaDelta.resize(nCells());
    _sommaFree.resize(nCells());
    //_sommaI.resize(nCells());
    _sommaEffectiveFree.resize(nCells());
    _sommaEffectiveDelta.resize(nCells());
    _sommaEffectiveHits.resize(nCells());
    _sommaDeltaSquare.resize(nCells());
    //_distances = Eigen::MatrixXd(nCells(),2);
    initGridWithValue(initValue);

    setBaseDrawManager(&ABSGRID3D_DRAW_MANAGER);
}

template< typename DataT>
LVOX3_Grid3D<DataT>* LVOX3_Grid3D<DataT>::createGrid3DFromXYZCoords(
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
    //qDebug()<<"@dim float = "<< (xmax - xmin)/resolutionx<<(ymax - ymin)/resolutiony<<(zmax - zmin)/resolutionz;
    //qDebug()<<"@creation of grid with size = " << dimx << dimy << dimz;

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

    return new LVOX3_Grid3D<DataT>(xmin, ymin, zmin, dimx, dimy, dimz, resolutionx,resolutiony,resolutionz, na, initValue);
}

template< typename DataT>
LVOX3_Grid3D<DataT>::~LVOX3_Grid3D()
{
    _data.clear();
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::initGridWithValue(const DataT val)
{
    for (size_t  i = 0 ; i < nCells() ; i++)
    {
        _data[i] = val;
    }
    _dataMin = val;
    _dataMax = val;
}


template< typename DataT>
void LVOX3_Grid3D<DataT>::setValueAtIndexFromDouble(const size_t &index, const double &value)
{
    setValueAtIndex(index, (DataT) value);
}


template< typename DataT>
void LVOX3_Grid3D<DataT>::computeMinMax()
{
    size_t ncells = nCells();
    if (ncells > 0) {
        _dataMin = _data[0];
        _dataMax = _data[0];

        for (size_t i = 1 ; i < ncells ; i++)
        {
            if (_data[i] != NA())
            {
                if (_dataMax==NA() || _data[i] > _dataMax) {_dataMax = _data[i];}
                if (_dataMin==NA() || _data[i] < _dataMin) {_dataMin = _data[i];}
            }
        }
    }
}

template< typename DataT>
bool LVOX3_Grid3D<DataT>::setValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}
    _data[index] = value;

    return true;
}


template< typename DataT>
bool LVOX3_Grid3D<DataT>::setValue(const size_t colx, const size_t liny, const size_t levz, const DataT value)
{
    size_t i;
    if (index(colx, liny, levz, i))
    {
        return setValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D<DataT>::setValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
DataT LVOX3_Grid3D<DataT>::valueAtIndex(const size_t index) const
{
    if (index >= nCells()) {return NA();}
    return _data[index];
}

template< typename DataT>
double LVOX3_Grid3D<DataT>::ratioValueAtIndex(const size_t index) const
{
    if (_dataMax <= _dataMin) {return 1;}
    DataT value = valueAtIndex(index);
    if (value == NA()) {return -1;}
    return (double) (((double)(value - _dataMin))/((double)(_dataMax - _dataMin)));
}

template< typename DataT>
double LVOX3_Grid3D<DataT>::valueAtIndexAsDouble(const size_t index) const
{
    DataT value = valueAtIndex(index);
    if (value == NA()) {return std::numeric_limits<DataT>::quiet_NaN();}
    return value;
}

template< typename DataT>
QString LVOX3_Grid3D<DataT>::valueAtIndexAsString(const size_t index) const
{
    DataT value = valueAtIndex(index);
    return QVariant(value).toString();
}

template< typename DataT>
QString LVOX3_Grid3D<DataT>::NAAsString() const
{
    return QVariant(NA()).toString();
}

template<typename DataT>
bool LVOX3_Grid3D<DataT>::isNA(size_t index) const
{
    return qFuzzyCompare(double(valueAtIndex(index)), double(NA()));
}

template< typename DataT>
DataT LVOX3_Grid3D<DataT>::value(const size_t colx, const size_t liny, const size_t levz) const
{
    size_t i;
    if (index(colx, liny, levz, i))
    {
        return valueAtIndex(i);
    }
    return NA();
}
/*
template< typename DataT>
DataT LVOX3_Grid3D<DataT>::dataFromArray(const size_t &x, const size_t &y, const size_t &z) const
{
    size_t i;
    if (index(x, y, z, i))
    {
        return valueAtIndex(i);
    }
    return NA();
}

template< typename DataT>
DataT LVOX3_Grid3D<DataT>::dataFromArray(const size_t &index) const
{
    if (index >= nCells()) {return NA();}
    return _data[index];
}
*/
template< typename DataT>
DataT LVOX3_Grid3D<DataT>::valueAtXYZ(const double x, const double y, const double z) const
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return valueAtIndex(i);
    }
    return false;
}


template< typename DataT>
bool LVOX3_Grid3D<DataT>::setMaxValueAtIndex(const size_t index, const DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value > currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D<DataT>::setMaxValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setMaxValueAtIndex(i, value);
    }
    return false;
}


template< typename DataT>
bool LVOX3_Grid3D<DataT>::setMinValueAtIndex(const size_t index, DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data[index];

    if ((currentValue == NA()) || (value < currentValue)) {
        return setValueAtIndex(index, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D<DataT>::setMinValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return setMinValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
bool LVOX3_Grid3D<DataT>::addValueAtIndex(const size_t index, DataT value)
{
    if (index >= nCells()) {return false;}

    DataT currentValue = _data[index];

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
bool LVOX3_Grid3D<DataT>::addValueAtXYZ(const double x, const double y, const double z, const DataT value)
{
    size_t i;
    if (indexAtXYZ(x, y, z, i))
    {
        return addValueAtIndex(i, value);
    }
    return false;
}

template< typename DataT>
QList<DataT> LVOX3_Grid3D<DataT>::neighboursValues(const size_t colx, const size_t liny, const size_t levz, const size_t distance, const bool keepNAs, const CenterMode centermode) const
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

/*
template< typename DataT>
CT_AbstractItemDrawable* LVOX3_Grid3D<DataT>::copy()
{    

    LVOX3_Grid3D<DataT>* cpy = new LVOX3_Grid3D<DataT>(minX(), minY(), minZ(), _dimx, _dimy, _dimz, _resx,_resy,_resz, _NAdata, _NAdata);
    cpy->setId(id());

    for (size_t i = 0 ; i < nCells() ; i++)
    {
        cpy->setValueAtIndex(i, valueAtIndex(i));
        //cpy->setSommaI(i,getSommaI(i));
        cpy->setSommaFree(i,getSommaFree(i));
        cpy->setSommaEffectiveFree(i, getSommaEffectiveFree(i));
        cpy->setSommaDelta(i, getSommaDelta(i));
        cpy->setSommaEffectiveDelta(i, getSommaEffectiveDelta(i));
        cpy->setSommaDeltaSquare(i, getSommaDeltaSquare(i));
        cpy->setSommaEffectiveHits(i, getSommaEffectiveHits(i));
    }

    if (nCells() >0)
    {
        cpy->computeMinMax();
    }
    //@tobeverified
    //cpy->setAlternativeDrawManager(getAlternativeDrawManager());

    return cpy;
}
*/
//SET
template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaDelta(size_t i,float value)
{
    _sommaDelta[i] = value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaFree(size_t i, float value)
{
    _sommaFree[i] = value;
}

/*template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaI(int i,double value)
{
    _sommaI[i] = value;
}*/

template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaEffectiveFree(size_t i,float value)
{
    _sommaEffectiveFree[i] = value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaEffectiveDelta(size_t i, float value)
{
    _sommaEffectiveDelta[i] = value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaEffectiveHits(size_t i,float value)
{
    _sommaEffectiveHits[i] = value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::setSommaDeltaSquare(size_t i,float value)
{
    _sommaDeltaSquare[i] = value;
}

//ADD
template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaDelta(size_t i,float value)
{
    _sommaDelta[i] += value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaFree(size_t i, float value)
{
    _sommaFree[i] += value;
}

/*template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaI(int i,double value)
{
    _sommaI[i] += value;
}*/

template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaEffectiveFree(size_t i,float value)
{
    _sommaEffectiveFree[i] += value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaEffectiveDelta(size_t i, float value)
{
    _sommaEffectiveDelta[i] += value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaEffectiveHits(size_t i,float value)
{
    _sommaEffectiveHits[i] += value;
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::addSommaDeltaSquare(size_t i, float value)
{
    _sommaDeltaSquare[i] += value;
}

//GET
template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaDelta(size_t i) const
{
    return _sommaDelta[i];
}

template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaFree(size_t i) const
{
    return _sommaFree[i];
}

/*template< typename DataT>
double LVOX3_Grid3D<DataT>::getSommaI(int i) const
{
    return _sommaI[i];
}*/

template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaEffectiveFree(size_t i) const
{
    return _sommaEffectiveFree[i];
}

template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaEffectiveDelta(size_t i) const
{
    return _sommaEffectiveDelta[i];
}

template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaEffectiveHits(size_t i) const
{
    return _sommaEffectiveHits[i];
}

template< typename DataT>
float LVOX3_Grid3D<DataT>::getSommaDeltaSquare(size_t i) const
{
    return _sommaDeltaSquare[i];
}

//Vegetation Element Attenuation Coefficient (Lambda1)
template< typename DataT>
float LVOX3_Grid3D<DataT>::getLambda1() const
{
    return _lambda1;
}

template< typename DataT>
// FP: here projected area is the projected area of a vegetation element randomly oriented (ie total area/4)
// VTN : If the vegetation element area is greater than projected of a voxel than the resulting of effective delta is null
void LVOX3_Grid3D<DataT>::setLambda1(float projectedArea)
{
    _lambda1 = projectedArea / (_resx * _resy * _resz);    
}

template< typename DataT>
void LVOX3_Grid3D<DataT>::doSommation(size_t index, float delta, float free)
{
    float attenuationCoefficient = getLambda1();    
    int sommaI = 0;
    if(free < delta)
        sommaI = 1;
    addSommaDelta(index,delta);
    addSommaFree(index,free);
    addSommaEffectiveDelta(index,-(log(1-delta*attenuationCoefficient) / attenuationCoefficient));
    addSommaEffectiveHits(index,-(sommaI*log(1-free*attenuationCoefficient) / attenuationCoefficient));
    addSommaEffectiveFree(index,-(log(1-free*attenuationCoefficient) / attenuationCoefficient));
    addSommaDeltaSquare(index,pow(-(log(1-delta*attenuationCoefficient) / attenuationCoefficient),2));
}
