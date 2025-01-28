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

#include "ct_grid3d_points.h"
#include "ct_log/ct_logmanager.h"

#include "ct_itemdrawable/tools/drawmanager/ct_standardgrid3d_pointdrawmanager.h"
const CT_StandardGrid3D_PointDrawManager CT_Grid3D_Points::GRID3D_POINT_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_Grid3D_Points)

CT_Grid3D_Points::CT_Grid3D_Points() : SuperClass()
{
    _emptyList = new QList<size_t>();
    setBaseDrawManager(&GRID3D_POINT_DRAW_MANAGER);
}

CT_Grid3D_Points::CT_Grid3D_Points(const CT_Grid3D_Points& other) : SuperClass(other)
{
    QMapIterator<size_t, QList<size_t>* > it(other._cells);

    while(it.hasNext()) {
        it.next();
        _cells.insert(it.key(), new QList<size_t>(*it.value()));
    }

    _emptyList = new QList<size_t>();
}

CT_Grid3D_Points::~CT_Grid3D_Points()
{
    qDeleteAll(_cells.begin(), _cells.end());
    _cells.clear();

    delete _emptyList;
}

CT_Grid3D_Points::CT_Grid3D_Points(double xmin,
                                   double ymin,
                                   double zmin,
                                   int dimx,
                                   int dimy,
                                   int dimz,
                                   double resolution) : SuperClass(xmin, ymin, zmin, dimx, dimy, dimz, resolution)
{
    _emptyList = new QList<size_t>();
    setBaseDrawManager(&GRID3D_POINT_DRAW_MANAGER);
}

CT_Grid3D_Points* CT_Grid3D_Points::createGrid3DFromXYZCoords(double xmin,
                                                              double ymin,
                                                              double zmin,
                                                              double xmax,
                                                              double ymax,
                                                              double zmax,
                                                              double resolution,
                                                              bool extends)
{
    return CreateGrid3DTFromXYZCoords<CT_Grid3D_Points>(xmin, ymin, zmin, xmax, ymax, zmax, resolution, extends);
}

bool CT_Grid3D_Points::addPoint(size_t pointGlobalIndex)
{
    const CT_Point& point = _pointAccessor.constPointAt(pointGlobalIndex);

    size_t cellIndex;
    if (this->indexAtXYZ(point(0), point(1), point(2), cellIndex))
    {
        if (!addPointAtIndex(cellIndex, pointGlobalIndex)) {return false;}

    } else {
        return false;
    }
    return true;
}

bool CT_Grid3D_Points::addPoint(size_t pointLocalIndex, double x, double y, double z)
{
    size_t cellIndex;
    if (this->indexAtXYZ(x, y, z, cellIndex))
    {
        if (!addPointAtIndex(cellIndex, pointLocalIndex)) {return false;}
    } else {
        return false;
    }
    return true;
}

bool CT_Grid3D_Points::addPointAtIndex(size_t cellIndex, size_t pointLocalIndex)
{
    if (cellIndex >= this->nCells()) {return false;}

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



const QList<size_t> *CT_Grid3D_Points::getConstPointIndexList(size_t cellIndex) const
{
    return _cells.value(cellIndex, _emptyList);
}


int CT_Grid3D_Points::getPointsInCellsIntersectingSphere(size_t gridIndex, double radius, QList<size_t> *indexList) const
{
    // point number
    int n = 0;
    double radius2 = radius*radius;

    // center of reference cell (center of the sphere)
    Eigen::Vector3d refCenter;
    Eigen::Vector3d currentCenter;
    size_t cellIndex;
    if (this->getCellCenterCoordinates(gridIndex, refCenter))
    {
        // Compute bounding box for search
        int minXcol, maxXcol, minYlin, maxYlin, minZlev, maxZlev;
        if (!this->colX(refCenter(0) - radius, minXcol)) {minXcol = 0;}
        if (!this->colX(refCenter(0) + radius, maxXcol)) {maxXcol = this->xdim() - 1;}
        if (!this->linY(refCenter(1) - radius, minYlin)) {minYlin = 0;}
        if (!this->linY(refCenter(1) + radius, maxYlin)) {maxYlin = this->ydim() - 1;}
        if (!this->levelZ(refCenter(2) - radius, minZlev)) {minZlev = 0;}
        if (!this->levelZ(refCenter(2) + radius, maxZlev)) {maxZlev = this->zdim() - 1;}

        for (int xx = minXcol ; xx <= maxXcol ; xx++)
        {
            for (int yy = minYlin ; yy <= maxYlin ; yy++)
            {
                for (int zz = minZlev ; zz <= maxZlev ; zz++)
                {
                    if (this->index(xx, yy, zz, cellIndex))
                    {
                        if (this->getCellCenterCoordinates(cellIndex, currentCenter))
                        {
                            double dist2 = pow(currentCenter(0) - refCenter(0), 2) + pow(currentCenter(1) - refCenter(1), 2) + pow(currentCenter(2) - refCenter(2), 2);

                            if (dist2 <= radius2)
                            {
                                const QList<size_t> *indices = this->getConstPointIndexList(cellIndex);
                                n += indices->size();

                                if (indexList != nullptr)
                                {
                                    if (indices->size() > 0)
                                    {
                                        indexList->append(*indices);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return n;
}

int CT_Grid3D_Points::getPointIndicesIncludingKNearestNeighbours(Eigen::Vector3d position, int k, double maxDist, QList<size_t> &indexList) const
{
    int n = 0;

    size_t index;
    if (this->indexAtXYZ(position(0), position(1), position(2), index))
    {
        double radius = this->resolution() / 100.0;

        while (n < k && radius < maxDist)
        {
            n = this->getPointsInCellsIntersectingSphere(index, radius, nullptr);
            radius += this->resolution();
        }

        n = this->getPointsInCellsIntersectingSphere(index, radius, &indexList);
    }

    return n;
}


void CT_Grid3D_Points::getCellIndicesAtNeighbourhoodN(size_t originIndex, int n, QList<size_t> &indices) const
{
    if (n == 0)
    {
        indices.append(originIndex);
    } else {
        int lin, col, levz;

        if (this->indexToGrid(originIndex, col, lin, levz))
        {
            int minlin, mincol, minlevz;
            int maxlin, maxcol, maxlevz;

            if (lin  > n) {minlin  = lin - n;}  else {minlin  = 0;}
            if (col  > n) {mincol  = col - n;}  else {mincol  = 0;}
            if (levz > n) {minlevz = levz - n;} else {minlevz = 0;}

            maxlin  = lin  + n; if (maxlin  >= this->_dimy) {maxlin  = this->_dimy - 1;}
            maxcol  = col  + n; if (maxcol  >= this->_dimx) {maxcol  = this->_dimx - 1;}
            maxlevz = levz + n; if (maxlevz >= this->_dimz) {maxlevz = this->_dimz - 1;}

            for (int yy = minlin ; yy <= maxlin ; yy++)
            {
                for (int xx = mincol ; xx <= maxcol ; xx++)
                {
                    for (int zz = minlevz ; zz <= maxlevz ; zz++)
                    {
                        size_t neighbIndex;
                        if (this->index(xx, yy, zz, neighbIndex))
                        {
                            indices.append(neighbIndex);
                        }
                    }
                }
            }
        } else {
            PS_LOG->addErrorMessage(LogInterface::itemdrawable, tr("Point grid index does not exist !"));
        }
    }
}

size_t CT_Grid3D_Points::getPointsInCellsIntersectingSphere(Eigen::Vector3d center, double radius, QList<size_t> *indexList) const
{
    // point number
    size_t n = 0;
    size_t cellIndex;

    // Compute bounding box for search
    int minXcol, maxXcol, minYlin, maxYlin, minZlev, maxZlev;

    if (!this->colX(center(0) - radius, minXcol)) {minXcol = 0;}
    if (!this->colX(center(0) + radius, maxXcol)) {maxXcol = this->xdim() - 1;}
    if (!this->linY(center(1) - radius, minYlin)) {minYlin = 0;}
    if (!this->linY(center(1) + radius, maxYlin)) {maxYlin = this->ydim() - 1;}
    if (!this->levelZ(center(2) - radius, minZlev)) {minZlev = 0;}
    if (!this->levelZ(center(2) + radius, maxZlev)) {maxZlev = this->zdim() - 1;}

    for (int xx = minXcol ; xx <= maxXcol ; xx++)
    {
        for (int yy = minYlin ; yy <= maxYlin ; yy++)
        {
            for (int zz = minZlev ; zz <= maxZlev ; zz++)
            {
                if (this->index(xx, yy, zz, cellIndex))
                {
                    const QList<size_t> *indices = this->getConstPointIndexList(cellIndex);

                    if (indexList != nullptr)
                    {
                        indexList->append(*indices);
                        n += size_t(indices->size());
                    }
                }
            }
        }
    }
    return n;
}
size_t CT_Grid3D_Points::getPointsInCellsIntersectingCylinder(Eigen::Vector3d center, double radius, double height, QList<size_t> *indexList) const
{
    // point number
    size_t n = 0;
    size_t cellIndex;

    // Compute bounding box for search
    int minXcol, maxXcol, minYlin, maxYlin, minZlev, maxZlev;

    if (!this->colX(center(0) - radius, minXcol)) {minXcol = 0;}
    if (!this->colX(center(0) + radius, maxXcol)) {maxXcol = this->xdim() - 1;}
    if (!this->linY(center(1) - radius, minYlin)) {minYlin = 0;}
    if (!this->linY(center(1) + radius, maxYlin)) {maxYlin = this->ydim() - 1;}
    if (!this->levelZ(center(2) - height/2.0, minZlev)) {minZlev = 0;}
    if (!this->levelZ(center(2) + height/2.0, maxZlev)) {maxZlev = this->zdim() - 1;}
    for (int xx = minXcol ; xx <= maxXcol ; xx++)
    {
        for (int yy = minYlin ; yy <= maxYlin ; yy++)
        {
            for (int zz = minZlev ; zz <= maxZlev ; zz++)
            {
                if (this->index(xx, yy, zz, cellIndex))
                {
                    const QList<size_t> *indices = this->getConstPointIndexList(cellIndex);
                    if (indexList != nullptr)
                    {
                        indexList->append(*indices);
                        n += size_t(indices->size());
                    }
                }
            }
        }
    }
    return n;
}
