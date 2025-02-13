/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
                     and the Association de Recherche Technologie et Sciences
(ARTS), Ecole Nationale Suprieure d'Arts et Metiers (ENSAM), Cluny, France. All
rights reserved.

 Contact : felix.chabot@usherbrooke.ca

 Developers : Félix Chabot (UdeS)

 This file is part of LVOX

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

#ifndef LVOX_3DGRIDDEFS
#define LVOX_3DGRIDDEFS

#include <ct_itemdrawable/abstract/ct_abstractgrid3d.h>
#include <ct_itemdrawable/ct_grid3d.h>

using LVOX3_AbstractGrid3D = CT_AbstractGrid3D;

template <typename DataT>
class LVOX3_Grid3D : public CT_Grid3D<DataT> {
public:
    using CT_Grid3D<DataT>::CT_Grid3D;

    static LVOX3_Grid3D* createGrid3DFromXYZCoords(double xmin,
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

      return new LVOX3_Grid3D<DataT>(
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


private:
    std::vector<float> _sommaDelta;
    std::vector<float> _sommaFree;
    std::vector<float> _sommaEffectiveFree;
    std::vector<float> _sommaEffectiveDelta;
    std::vector<float> _sommaEffectiveHits;
    std::vector<float> _sommaDeltaSquare;

    double _lambda1;

public:

    /*!
    * \brief setLambda1 Set the attenuation coefficient divided by the volume of
    * a voxel
    * \param vegetation Vegetation element to divide by the volume
    */
    void setLambda1(float vegetation)
    {
      _lambda1 = vegetation;
    }

    /*!
    * \brief getLambda1 Get the attenuation coefficient
    * \return The attenuation coefficient
    */
    float getLambda1() const
    {
      return _lambda1;
    }

    void setSommaDelta(size_t i,float value)
    {
      _sommaDelta[i] = value;
    }


    void setSommaFree(size_t i, float value)
    {
      _sommaFree[i] = value;
    }

    void setSommaEffectiveFree(size_t i,float value)
    {
      _sommaEffectiveFree[i] = value;
    }


    void setSommaEffectiveDelta(size_t i, float value)
    {
      _sommaEffectiveDelta[i] = value;
    }


    void setSommaEffectiveHits(size_t i,float value)
    {
      _sommaEffectiveHits[i] = value;
    }


    void setSommaDeltaSquare(size_t i,float value)
    {
      _sommaDeltaSquare[i] = value;
    }

    void addSommaDelta(size_t i,float value)
    {
      _sommaDelta[i] += value;
    }


    void addSommaFree(size_t i, float value)
    {
      _sommaFree[i] += value;
    }

    void addSommaEffectiveFree(size_t i,float value)
    {
      _sommaEffectiveFree[i] += value;
    }


    void addSommaEffectiveDelta(size_t i, float value)
    {
      _sommaEffectiveDelta[i] += value;
    }


    void addSommaEffectiveHits(size_t i,float value)
    {
      _sommaEffectiveHits[i] += value;
    }


    void addSommaDeltaSquare(size_t i, float value)
    {
      _sommaDeltaSquare[i] += value;
    }


    float getSommaDelta(size_t i) const
    {
      return _sommaDelta[i];
    }


    float getSommaFree(size_t i) const
    {
      return _sommaFree[i];
    }

    float getSommaEffectiveFree(size_t i) const
    {
      return _sommaEffectiveFree[i];
    }


    float getSommaEffectiveDelta(size_t i) const
    {
      return _sommaEffectiveDelta[i];
    }


    float getSommaEffectiveHits(size_t i) const
    {
      return _sommaEffectiveHits[i];
    }


    float getSommaDeltaSquare(size_t i) const
    {
      return _sommaDeltaSquare[i];
    }

    /*!
     * \brief doSommation Call the other summation functions
     * \param index index to assign to
     * \param delta Distance a ray would parcour inside the voxel
     * \param free Real distance the ray parcour inside the voxel
     */
    void doSommation(size_t index, float delta, float free) {
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
};

#endif // LVOX_3DGRIDDEFS
