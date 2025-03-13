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

#ifndef LVOX_GRID3D_H
#define LVOX_GRID3D_H

#include <ct_itemdrawable/abstract/ct_abstractgrid3d.h>
#include <ct_itemdrawable/ct_grid3d.h>

template <typename DataT>
class LVOX_Grid3D : public CT_Grid3D<DataT> {
public:
    LVOX_Grid3D();

    LVOX_Grid3D(double xmin,
          double ymin,
          double zmin,
          int dimx,
          int dimy,
          int dimz,
          double resolution,
          DataT na,
          DataT initValue);

    LVOX_Grid3D(double xmin,
          double ymin,
          double zmin,
          double xmax,
          double ymax,
          double zmax,
          double resolution,
          DataT na,
          DataT initValue);

    LVOX_Grid3D(const CT_Grid3D<DataT>& other);

    static LVOX_Grid3D* createGrid3DFromXYZCoords(double xmin,
                                            double ymin,
                                            double zmin,
                                            double xmax,
                                            double ymax,
                                            double zmax,
                                            double resolution,
                                            DataT na,
                                            DataT initValue,
                                            bool extends);

    /*!
    * \brief setLambda1 Set the attenuation coefficient divided by the volume of
    * a voxel
    * \param vegetation Vegetation element to divide by the volume
    */
    void setLambda1(float vegetation);

    /*!
    * \brief getLambda1 Get the attenuation coefficient
    * \return The attenuation coefficient
    */
    float getLambda1() const;

    void setSommaDelta(size_t i,float value);

    void setSommaFree(size_t i, float value);

    void setSommaEffectiveFree(size_t i,float value);

    void setSommaEffectiveDelta(size_t i, float value);

    void setSommaEffectiveHits(size_t i,float value);

    void setSommaDeltaSquare(size_t i,float value);

    void addSommaDelta(size_t i,float value);

    void addSommaFree(size_t i, float value);

    void addSommaEffectiveFree(size_t i,float value);

    void addSommaEffectiveDelta(size_t i, float value);

    void addSommaEffectiveHits(size_t i,float value);

    void addSommaDeltaSquare(size_t i, float value);

    float getSommaDelta(size_t i) const;

    float getSommaFree(size_t i) const;

    float getSommaEffectiveFree(size_t i) const;

    float getSommaEffectiveDelta(size_t i) const;

    float getSommaEffectiveHits(size_t i) const;

    float getSommaDeltaSquare(size_t i) const;

    /*!
     * \brief doSommation Call the other summation functions
     * \param index index to assign to
     * \param delta Distance a ray would parcour inside the voxel
     * \param free Real distance the ray parcour inside the voxel
     */
    void doSommation(size_t index, float delta, float free) ;

private:
    std::map<size_t, float> _sommaDelta;
    std::map<size_t, float> _sommaFree;
    std::map<size_t, float> _sommaEffectiveFree;
    std::map<size_t, float> _sommaEffectiveDelta;
    std::map<size_t, float> _sommaEffectiveHits;
    std::map<size_t, float> _sommaDeltaSquare;

    double _lambda1;
};

#include "lvox_grid3d.hpp"

#endif // LVOX_GRID3D_H
