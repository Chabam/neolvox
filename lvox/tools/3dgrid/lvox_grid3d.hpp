#ifndef LVOX_GRID3D_HPP
#define LVOX_GRID3D_HPP

#include "lvox_grid3d.h"

template <typename DataT>
LVOX_Grid3D<DataT>* LVOX_Grid3D<DataT>::createGrid3DFromXYZCoords(double xmin,
                                            double ymin,
                                            double zmin,
                                            double xmax,
                                            double ymax,
                                            double zmax,
                                            double resolution,
                                            DataT na,
                                            DataT initValue,
                                            bool extends)
    {
      const CT_Grid3D<DataT>* newGrid = CT_Grid3D<DataT>::createGrid3DFromXYZCoords(xmin, ymin, zmin, xmax, ymax, zmax, resolution, na, initValue, extends);

      return new LVOX_Grid3D<DataT>(
          xmin,
          ymin,
          zmin,
          newGrid->xdim(),
          newGrid->ydim(),
          newGrid->zdim(),
          resolution,
          na,
          initValue);
  }
template <typename DataT>
void LVOX_Grid3D<DataT>::setLambda1(float vegetation)
{
  _lambda1 = vegetation;
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getLambda1() const
{
  return _lambda1;
}
template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaDelta(size_t i,float value)
{
  _sommaDelta[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaFree(size_t i, float value)
{
  _sommaFree[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaEffectiveFree(size_t i,float value)
{
  _sommaEffectiveFree[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaEffectiveDelta(size_t i, float value)
{
  _sommaEffectiveDelta[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaEffectiveHits(size_t i,float value)
{
  _sommaEffectiveHits[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::setSommaDeltaSquare(size_t i,float value)
{
  _sommaDeltaSquare[i] = value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaDelta(size_t i,float value)
{
  _sommaDelta[i] += value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaFree(size_t i, float value)
{
  _sommaFree[i] += value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaEffectiveFree(size_t i,float value)
{
  _sommaEffectiveFree[i] += value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaEffectiveDelta(size_t i, float value)
{
  _sommaEffectiveDelta[i] += value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaEffectiveHits(size_t i,float value)
{
  _sommaEffectiveHits[i] += value;
}

template <typename DataT>
void LVOX_Grid3D<DataT>::addSommaDeltaSquare(size_t i, float value)
{
  _sommaDeltaSquare[i] += value;
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaDelta(size_t i) const
{
  return _sommaDelta[i];
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaFree(size_t i) const
{
  return _sommaFree[i];
}
template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaEffectiveFree(size_t i) const
{
  return _sommaEffectiveFree[i];
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaEffectiveDelta(size_t i) const
{
  return _sommaEffectiveDelta[i];
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaEffectiveHits(size_t i) const
{
  return _sommaEffectiveHits[i];
}

template <typename DataT>
float LVOX_Grid3D<DataT>::getSommaDeltaSquare(size_t i) const
{
  return _sommaDeltaSquare[i];
}

template <typename DataT>
void LVOX_Grid3D<DataT>::doSommation(size_t index, float delta, float free) {
  float attenuationCoefficient = getLambda1();
  int sommaI = 0;
  if (free < delta)
    sommaI = 1;
  addSommaDelta(index, delta);
  addSommaFree(index, free);
  addSommaEffectiveDelta(index, -(log(1 - delta * attenuationCoefficient) /
                                  attenuationCoefficient));
  addSommaEffectiveHits(index,
                        -(sommaI * log(1 - free * attenuationCoefficient) /
                          attenuationCoefficient));
  addSommaEffectiveFree(index, -(log(1 - free * attenuationCoefficient) /
                                 attenuationCoefficient));
  addSommaDeltaSquare(index, pow(-(log(1 - delta * attenuationCoefficient) /
                                   attenuationCoefficient),
                                 2));
}

#endif // LVOX_GRID3D_HPP
