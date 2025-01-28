/****************************************************************************

 Copyright (C) 2010-2015 the Office National des Forêts (ONF), France
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre Piboule (ONF)

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

#include "ct_delaunayoutline.h"

CT_DelaunayOutline::CT_DelaunayOutline()
{
    _optimizationGrid = nullptr;
}

CT_DelaunayOutline::~CT_DelaunayOutline()
{
    _vertices.clear();
    if (_optimizationGrid != nullptr) {delete _optimizationGrid;}
}

void CT_DelaunayOutline::addVertex(CT_DelaunayVertex *vt)
{
    for (int i = 0 ; i < _vertices.size () ; i++)
    {
        CT_DelaunayVertex* vt2 = _vertices.at(i);
        if (vt2->equals (vt)) {return;}
    }

    _vertices.append(vt);
}

double CT_DelaunayOutline::area()
{
    double sum = 0;
    CT_DelaunayVertex* v1;
    CT_DelaunayVertex* v2;

    _vertices.append(_vertices.at(0));

    for (int i = 0 ; i < _vertices.size () - 1 ; i++)
    {
        v1 = _vertices.at(i);
        v2 = _vertices.at(i+1);
        sum += (v1->x() * v2->y()) - (v1->y() * v2->x());
    }

    _vertices.removeLast();

    return std::abs(sum / 2.0);
}

bool CT_DelaunayOutline::contains(double x, double y, bool optimize) const
{
    if (optimize && _optimizationGrid != nullptr)
    {
        if (_optimizationGrid->valueAtCoords(x, y) == 4)
        {
            return true;
        }
    }

    double a, b, a2, b2, xInter, yInter, maxx, maxy, minx, miny;
    bool ok = false;
    CT_DelaunayVertex* vt1;
    CT_DelaunayVertex* vt2;
    int nbInter = 0;

    // if the point (x,y) is a vertex, it's contained in the polygon
    for (int i = 0 ; i < _vertices.size() ; i++)
    {
        vt1 = _vertices.at(i);
        if ((x == vt1->x()) && (y == vt1->y())) {return true;}
    }

    while (!ok)
    {
        a = ((double) std::rand() / (RAND_MAX));
        b = y - a*x;

        ok = true;
        nbInter = 0;

        for (int i = 0; i < _vertices.size() ; i++)
        {
            vt1 = _vertices.at(i);

            if (i == _vertices.size() - 1)
            {
                vt2 = _vertices.at(0);
            } else {
                vt2 = _vertices.at(i+1);
            }

            if (vt1->x() != vt2->x())
            {
                a2 = (vt1->y() - vt2->y()) / (vt1->x() - vt2->x());
                b2 = vt1->y() - a2*vt1->x();

                if (a != a2)
                {
                    xInter = (b2-b)/(a-a2);
                } else {
                    ok = false;
                    break;
                }
            } else
            {
                xInter = vt1->x();
            }

            yInter = a*xInter + b;

            if (xInter <= x)
            {

                if (vt1->x() > vt2->x()) {maxx = vt1->x(); minx = vt2->x();}
                else               {maxx = vt2->x(); minx = vt1->x();}

                if (vt1->y() > vt2->y()) {maxy = vt1->y(); miny = vt2->y();}
                else               {maxy = vt2->y(); miny = vt1->y();}

                if ((xInter >= minx) && (xInter <= maxx) && (yInter >= miny) && (yInter <= maxy)) {

                    nbInter = nbInter + 1;
                }
            }


            for (int j = 0 ; j < _vertices.size() ; j++)
            {
                vt1 = _vertices.at(j);

                if ((vt1->x() == xInter) && (vt1->y() == yInter))
                {
                    ok = false;
                    break;
                }
            }
        }
    }

    if ((nbInter == 0) || ((nbInter % 2) == 0)) {return false;}
    else                                        {return true;}
}

void CT_DelaunayOutline::clear()
{
    _vertices.clear();
}

CT_Polygon2DData* CT_DelaunayOutline::getShape()
{
    const int size = _vertices.size();

    QVector<Eigen::Vector2d> vertices;
    vertices.resize(size);

    for(int i=0; i<size; ++i)
    {
        const CT_DelaunayVertex* vt = _vertices.at(i);

        Eigen::Vector2d& v = vertices[i];
        v(0) = vt->x();
        v(1) = vt->y();
    }

    return new CT_Polygon2DData(vertices);
}

void CT_DelaunayOutline::computeOptimizationGrid()
{
    double minx = std::numeric_limits<double>::max();
    double miny = std::numeric_limits<double>::max();
    double maxx = -std::numeric_limits<double>::max();
    double maxy = -std::numeric_limits<double>::max();

    for (int i = 0 ; i < _vertices.size() ; i++)
    {
        CT_DelaunayVertex* vertex = _vertices.at(i);

        if (vertex->x() < minx) {minx = vertex->x();}
        if (vertex->y() < miny) {miny = vertex->y();}
        if (vertex->x() > maxx) {maxx = vertex->x();}
        if (vertex->y() > maxy) {maxy = vertex->y();}
    }

    double sizex = maxx - minx;
    double sizey = maxy - miny;
    double resolution = std::floor(100.0*std::min(sizex / 50.0, sizey / 50.0)) / 100.0;
    if (resolution <= 0.25) {resolution = 0.25;}

    minx = (std::floor(minx - 1) / resolution) * resolution - resolution;
    miny = (std::floor(miny - 1) / resolution) * resolution - resolution;

    maxx = minx + sizex + 2.0*resolution;
    maxy = miny + sizey + 2.0*resolution;

    if (_optimizationGrid != nullptr) {delete _optimizationGrid;}
    _optimizationGrid = CT_Image2D<char>::createImage2DFromXYCoords(minx, miny, maxx, maxy, resolution, 0, -1, 0);

    for (int yy = 0; yy < _optimizationGrid->ydim() ; yy++)
    {
        for (int xx = 0; xx < _optimizationGrid->xdim() ; xx++)
        {
            Eigen::Vector2d corner = Eigen::Vector2d(0.0, 0.0);
            _optimizationGrid->getCellBottomLeftCorner(xx, yy, corner);

            if (this->contains(corner(0), corner(1), false))
            {
                size_t index = 0;
                _optimizationGrid->index(xx    , yy    , index);
                _optimizationGrid->addValueAtIndex(index, 1);

                _optimizationGrid->index(xx    , yy - 1, index);
                _optimizationGrid->addValueAtIndex(index, 1);

                _optimizationGrid->index(xx - 1, yy    , index);
                _optimizationGrid->addValueAtIndex(index, 1);

                _optimizationGrid->index(xx - 1, yy - 1, index);
                _optimizationGrid->addValueAtIndex(index, 1);
            }
        }
    }
}
