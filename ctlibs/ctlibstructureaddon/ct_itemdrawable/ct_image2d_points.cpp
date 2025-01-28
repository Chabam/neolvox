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

#include "ct_image2d_points.h"
#include "ct_log/ct_logmanager.h"

#include "ct_itemdrawable/tools/drawmanager/ct_standardimage2d_pointdrawmanager.h"

const CT_StandardImage2D_PointDrawManager CT_Image2D_Points::IMAGE2D_POINT_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_Image2D_Points)

CT_Image2D_Points::CT_Image2D_Points() : SuperClass()
{
    _emptyList = new QList<size_t>();
    setBaseDrawManager(&IMAGE2D_POINT_DRAW_MANAGER);
}

CT_Image2D_Points::CT_Image2D_Points(const CT_Image2D_Points& other) : SuperClass(other)
{
    QMapIterator<size_t, QList<size_t>* > it(other._cells);

    while(it.hasNext()) {
        it.next();
        _cells.insert(it.key(), new QList<size_t>(*it.value()));
    }

    _res = other._res;
    _dimx = other._dimx;
    _dimy = other._dimy;
    _minXCoord = other._minXCoord;
    _minYCoord = other._minYCoord;
    _level = other._level;

    setBoundingBox(_minXCoord,
                         _minYCoord,
                         0,
                         _minXCoord + _res * _dimx,
                         _minYCoord + _res * _dimy,
                         0);


    _emptyList = new QList<size_t>();

    setBaseDrawManager(&IMAGE2D_POINT_DRAW_MANAGER);
}

CT_Image2D_Points::~CT_Image2D_Points()
{
    qDeleteAll(_cells.begin(), _cells.end());
    _cells.clear();

    delete _emptyList;
}

CT_Image2D_Points::CT_Image2D_Points(double xmin,
                                   double ymin,
                                   int dimx,
                                   int dimy,
                                   double resolution)
{

    _res = resolution;
    _dimx = dimx;
    _dimy = dimy;
    _minXCoord = xmin;
    _minYCoord = ymin;
    _level = 0;

    setBoundingBox(_minXCoord,
                         _minYCoord,
                         0,
                         _minXCoord + _res * _dimx,
                         _minYCoord + _res * _dimy,
                         0);


    _emptyList = new QList<size_t>();
    setBaseDrawManager(&IMAGE2D_POINT_DRAW_MANAGER);
}

CT_Image2D_Points* CT_Image2D_Points::createGrid3DFromXYCoords(double xmin,
                                                              double ymin,
                                                              double xmax,
                                                              double ymax,
                                                              double resolution)
{
    int dimx = int(std::ceil((xmax - xmin)/resolution));
    int dimy = int(std::ceil((ymax - ymin)/resolution));

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (xmax >= (xmin + dimx * resolution))
    {
        dimx++;
    }

    while (ymax >= (ymin + dimy * resolution))
    {
        dimy++;
    }

    CT_Image2D_Points* grid = new CT_Image2D_Points(xmin, ymin, dimx, dimy, resolution);

    return grid;
}


bool CT_Image2D_Points::addPoint(size_t pointGlobalIndex)
{
    const CT_Point& point = _pointAccessor.constPointAt(pointGlobalIndex);

    size_t cellIndex;
    if (indexAtCoords(point(0), point(1), cellIndex))
    {
        if (!addPointAtCellIndex(cellIndex, pointGlobalIndex)) {return false;}
    } else {
        qDebug() << "ERROR: CT_Image2D_Points::addPoint";
        return false;
    }
    return true;
}


bool CT_Image2D_Points::addPoint(size_t pointLocalIndex, double x, double y)
{
    size_t cellIndex;
    if (indexAtCoords(x, y, cellIndex))
    {
        if (!addPointAtCellIndex(cellIndex, pointLocalIndex)) {return false;}
    } else {
        qDebug() << "ERROR: CT_Image2D_Points::addPoint";
        return false;
    }
    return true;
}


bool CT_Image2D_Points::addPointAtCellIndex(size_t cellIndex, size_t pointLocalIndex)
{
    if (cellIndex >= nCells()) {return false;}

    QList<size_t>* list = _cells[cellIndex];
    if (list == nullptr)
    {
        list = new QList<size_t>();
        list->append(pointLocalIndex);
        _cells.insert(cellIndex, list);
    } else {
        list->append(pointLocalIndex);
    }

    return true;
}


const QList<size_t> *CT_Image2D_Points::getConstPointIndexList(size_t cellIndex) const
{
    return _cells.value(cellIndex, _emptyList);
}


int CT_Image2D_Points::getPointsInCellsIntersectingCircle(size_t cellIndex, double radius, QList<size_t> &indexList) const
{
    indexList.clear();

    // point number
    int n = 0;
    double radius2 = radius*radius;

    // center of reference cell (center of the circle)
    Eigen::Vector3d refCenter;
    Eigen::Vector3d currentCenter;

    if (getCellCenterCoordinates(cellIndex, refCenter))
    {
        // Compute bounding box for search
        double minX = refCenter(0) - radius;
        double maxX = refCenter(0) + radius;
        double minY = refCenter(1) - radius;
        double maxY = refCenter(1) + radius;

        int minXcol, maxXcol, minYlin, maxYlin;

        if (!xcol(minX, minXcol)) {minXcol = 0;}
        if (!xcol(maxX, maxXcol)) {maxXcol = xdim() - 1;}
        if (!lin(minY, maxYlin)) {maxYlin = ydim() - 1;}
        if (!lin(maxY, minYlin)) {minYlin = 0;}

        for (int xx = minXcol ; xx <= maxXcol ; xx++)
        {
            for (int yy = minYlin ; yy <= maxYlin ; yy++)
            {
                size_t currentCellIndex;
                if (index(xx, yy, currentCellIndex))
                {
                    if (getCellCenterCoordinates(currentCellIndex, currentCenter))
                    {
                        double dist2 = pow(currentCenter(0) - refCenter(0), 2) + pow(currentCenter(1) - refCenter(1), 2);

                        if (dist2 <= radius2)
                        {
                            const QList<size_t> *indices = getConstPointIndexList(cellIndex);
                            n += indices->size();

                            if (indices->size() > 0)
                            {
                                indexList.append(*indices);
                            }
                        }
                    }
                }
            }
        }
    }

    return n;
}


int CT_Image2D_Points::getPointIndicesIncludingKNearestNeighbours(const Eigen::Vector3d& position, int k, double maxDist, QList<size_t> &indexList) const
{
    int n = 0;

    size_t index;
    if (indexAtCoords(position(0), position(1), index))
    {
        double radius = resolution() / 100.0;

        while (n < k && radius < maxDist)
        {
            n = getPointsInCellsIntersectingCircle(index, radius, indexList);
            radius += resolution();
        }

        n = getPointsInCellsIntersectingCircle(index, radius, indexList);
    }

    return n;
}


void CT_Image2D_Points::getCellIndicesAtNeighbourhoodN(size_t originIndex, int n, QList<size_t> &indices) const
{
    if (n == 0)
    {
        indices.append(originIndex);
    } else {
        int lin, col;

        if (indexToGrid(originIndex, col, lin))
        {
            int minlin, mincol;
            int maxlin, maxcol;

            if (lin  > n) {minlin  = lin - n;}  else {minlin  = 0;}
            if (col  > n) {mincol  = col - n;}  else {mincol  = 0;}

            maxlin  = lin  + n; if (maxlin  >= _dimy) {maxlin  = _dimy - 1;}
            maxcol  = col  + n; if (maxcol  >= _dimx) {maxcol  = _dimx - 1;}

            for (int yy = minlin ; yy <= maxlin ; yy++)
            {
                for (int xx = mincol ; xx <= maxcol ; xx++)
                {
                    size_t neighbIndex;
                    if (index(xx, yy, neighbIndex))
                    {
                        indices.append(neighbIndex);
                    }
                }
            }
        } else {
            PS_LOG->addErrorMessage(LogInterface::itemdrawable, tr("Point grid index does not exist !"));
        }
    }
}


size_t CT_Image2D_Points::getPointsInCellsIntersectingCircle(const Eigen::Vector3d& center, double radius, QList<size_t> &indexList) const
{
    indexList.clear();

    // point number
    size_t n = 0;
    size_t cellIndex;

    // Compute bounding box for search
    double minX = center(0) - radius;
    double maxX = center(0) + radius;
    double minY = center(1) - radius;
    double maxY = center(1) + radius;

    int minXcol, maxXcol, minYlin, maxYlin;

    if (!xcol(minX, minXcol)) {minXcol = 0;}
    if (!xcol(maxX, maxXcol)) {maxXcol = xdim() - 1;}
    if (!lin(minY, maxYlin)) {maxYlin = ydim() - 1;}
    if (!lin(maxY, minYlin)) {minYlin = 0;}

    for (int xx = minXcol ; xx <= maxXcol ; xx++)
    {
        for (int yy = minYlin ; yy <= maxYlin ; yy++)
        {
            if (index(xx, yy, cellIndex))
            {
                const QList<size_t> *indices = getConstPointIndexList(cellIndex);

                indexList.append(*indices);
                n += size_t(indices->size());
            }
        }
    }
    return n;
}

size_t CT_Image2D_Points::getPointsInCircle(const Eigen::Vector3d& center, double radius, QList<size_t> &indexList) const
{
    indexList.clear();

    // point number
    size_t n = 0;
    size_t cellIndex;

    // Compute bounding box for search
    double minX = center(0) - radius;
    double maxX = center(0) + radius;
    double minY = center(1) - radius;
    double maxY = center(1) + radius;

    int minXcol, maxXcol, minYlin, maxYlin;

    if (!xcol(minX, minXcol)) {minXcol = 0;}
    if (!xcol(maxX, maxXcol)) {maxXcol = xdim() - 1;}
    if (!lin(minY, maxYlin)) {maxYlin = ydim() - 1;}
    if (!lin(maxY, minYlin)) {minYlin = 0;}


    for (int xx = minXcol ; xx <= maxXcol ; xx++)
    {
        for (int yy = minYlin ; yy <= maxYlin ; yy++)
        {
            if (index(xx, yy, cellIndex))
            {
                const QList<size_t> *indices = getConstPointIndexList(cellIndex);

                for (int i = 0 ; i < indices->size() ; i++)
                {
                    const CT_Point& point = _pointAccessor.constPointAt(indices->at(i));
                    double dist = sqrt(pow(point(0) - center(0), 2) + pow(point(1) - center(1), 2));


                    if (dist < radius)
                    {
                        indexList.append(indices->at(i));
                        n++;
                    }
                }
            }
        }
    }

    return n;
}
