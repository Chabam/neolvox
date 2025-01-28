/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
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

#include "ct_plotlistingrid.h"
#include "ct_shape2ddata/ct_circle2ddata.h"
#include "ct_shape2ddata/ct_box2ddata.h"

CT_DEFAULT_IA_INIT(CT_PlotListInGrid)

CT_PlotListInGrid::CT_PlotListInGrid() : SuperClass(),
    _areaXY(nullptr),
    _spacing(1),
    _size(0),
    _firstIndex( 0),
    _indexJumpAtEOL(0)
{
}

CT_PlotListInGrid::CT_PlotListInGrid(const CT_AreaShape2DData* areaShape2D,
                                     const Eigen::Vector2d& refCoords,
                                     double spacing,
                                     double size) : SuperClass(),
    _areaXY(areaShape2D == nullptr ? nullptr : static_cast<CT_AreaShape2DData*>(areaShape2D->copy())),
    _spacing(spacing),
    _size(size),
    _firstIndex( 0),
    _indexJumpAtEOL(0)
{
    computeMinMax(refCoords);
}

CT_PlotListInGrid::CT_PlotListInGrid(const CT_PlotListInGrid& other) : SuperClass(other),
    _areaXY(other._areaXY == nullptr ? nullptr : static_cast<CT_AreaShape2DData*>(other._areaXY->copy())),
    _min(other._min),
    _max(other._max),
    _spacing(other._spacing),
    _size(other._size),
    _firstIndex(other._firstIndex),
    _indexJumpAtEOL(other._indexJumpAtEOL)
{
}

CT_PlotListInGrid::~CT_PlotListInGrid()
{
    delete _areaXY;
}

void CT_PlotListInGrid::computeMinMax(const Eigen::Vector2d &refCoords)
{
    if (_areaXY != nullptr)
    {
        Eigen::Vector3d minShape, maxShape;
        _areaXY->getBoundingBox(minShape, maxShape);

        _min(0) = std::floor((minShape(0) - refCoords(0)) / _spacing) * _spacing + refCoords(0);
        _min(1) = std::floor((minShape(1) - refCoords(1)) / _spacing) * _spacing + refCoords(1);

        while (_min(0) < minShape(0)) {_min(0) += _spacing;}
        while (_min(1) < minShape(1)) {_min(1) += _spacing;}

        _max(0) = _min(0);
        _max(1) = _min(1);

        size_t ncol = 0;
        size_t nrow = 0;

        while (_max(0) < maxShape(0)) {ncol++; _max(0) += _spacing;}
        while (_max(1) < maxShape(1)) {nrow++; _max(1) += _spacing;}
        if (_max(0) > maxShape(0)) {_max(0) -= _spacing;}
        if (_max(1) > maxShape(1)) {_max(1) -= _spacing;}

        setBoundingBox(_min(0), _min(1), 0, _max(0), _max(1), 0);
    }
}

void CT_PlotListInGrid::getBoundingBox2D(Eigen::Vector2d &min, Eigen::Vector2d &max) const
{
    min = _min;
    max = _max;

    min(0)-= _size;
    min(1)-= _size;
    max(0)+= _size;
    max(1)+= _size;
}

void CT_PlotListInGrid::setIndices(size_t firstIndex, size_t indexJumpAtEOL)
{
    _firstIndex = firstIndex;
    _indexJumpAtEOL = indexJumpAtEOL;
}

QHash<CT_AreaShape2DData*, size_t> CT_PlotListInGrid::createPlots(CT_PlotListInGrid::Type type)
{
    double size2 = 2.0*_size;
    size_t index = _firstIndex;
    size_t lastBeginningIndex = _firstIndex;

    QHash<CT_AreaShape2DData*, size_t> plots;
    for (double y = _max(1) ; y >= _min(1) ; y -= _spacing)
    {
        for (double x = _min(0) ; x <= _max(0) ; x += _spacing)
        {
            CT_AreaShape2DData* shape = nullptr;

            if (type == CT_PlotListInGrid::T_Circle)
            {
                shape = new CT_Circle2DData(Eigen::Vector2d(x, y), _size);
            } else {
                shape = new CT_Box2DData(Eigen::Vector2d(x, y), size2, size2);
            }
            plots.insert(shape, index++);
        }
        index = lastBeginningIndex + _indexJumpAtEOL;
        lastBeginningIndex = index;
    }
    return plots;
}
