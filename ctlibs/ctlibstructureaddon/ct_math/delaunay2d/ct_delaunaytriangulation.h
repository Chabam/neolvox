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

#ifndef CT_DELAUNAYTRIANGULATION_H
#define CT_DELAUNAYTRIANGULATION_H

#include "ct_delaunayvertex.h"
#include "ct_delaunaytriangle.h"
#include "ct_delaunayoutline.h"
#include "ct_delaunayvorovertex.h"

#include <QList>
#include <QVector>
#include <QMap>

#include <numeric>

/**
 * DelaunayTriangulation - Delaunay triangulation.
 *
 * @author A. Piboule - february 2004
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunayTriangulation
{

public:
    CT_DelaunayTriangulation ();
    ~CT_DelaunayTriangulation ();

    void clear(bool clearToInsert = true);


    // Construct the 4 corners of the triangulation, and the 2 first triangles
    void init (double x1, double y1, double x2, double y2);


    // idem but corners are calculated from others points
    bool init ();

    bool isInBBox(double x, double y);

    // Accesors:

    // gives all vertices successfully inserted in the triangulation
    inline const QList<CT_DelaunayVertex*> &getInsertedVertices () {return _insertedVertices;}

    // gives all out of bounds vertices (which have NOT been inserted)
    inline const QList<CT_DelaunayVertex*> &getOutOfBoundsVertices () {return _outOfBoundsVertices;}

    // the returned hashMap gives for each (not inserted) duplicated vertex (in term of position)
    // a link to the same position vertex which has been inserted.
    inline const QMap<CT_DelaunayVertex*, CT_DelaunayVertex*> &getDuplicatePositions () {return _duplicatePositions;}

    // gives the 4 bounds conners which have been added in init (),
    // these 4 points are NOT in insertedVertex List
    inline const QVector<CT_DelaunayVertex*> &getCorners () {return _corners;}

    // gives the triangle used to enter in triangulation to find "to insert" vertex triangle
    const CT_DelaunayTriangle* getRefTriangle () {return _refTriangle;}

    inline bool isInitialized () {return _initialized;}



    // gives the up to date list of all triangles
    const QList<CT_DelaunayTriangle*> &getTriangles ();

    // gives the insertedVertices list with up to date computed neighbors for each vertex
    const QList<CT_DelaunayVertex*> &getVerticesNeighbors ();

    // gives the insertedVertices list with up to date computed Voronoi Diagram for each vertex
    // Voronoi diagram consist for each vertex, in a sorted list of DelaunayVoroVertex defining the Voronoi Polygon for this vertex
    const QList<CT_DelaunayVertex*> &getVoronoiDiagram ();


    // gives the same thing that getVoronoiDiagram
    // but without voronoi polygon of corners neighbors (to avoid border effects)

    const QList<CT_DelaunayVertex*> &getCleanVoronoiDiagram ();

    // gives the outline defined by the triangulation (corners excluded)
    const CT_DelaunayOutline &getOutline();

    QMultiMap<CT_DelaunayVertex*, CT_DelaunayVertex*> getEdges();

    // compute for each vertex its neighbors
    const QList<CT_DelaunayVertex*> &computeVerticesNeighbors ();


    // create an Map : the keys are inserted vertex
    //					   the values are voronoi's polygons for these inserted vertex
    const QList<CT_DelaunayVertex*> &computeVoronoiDiagram ();


    // compute the Outline of the triangulation
    const CT_DelaunayOutline &computeOutline ();


    // add a DelaunayVertex to the toInsert list
    CT_DelaunayVertex* addVertex (Eigen::Vector3d *data, bool deleteData);

    // add all vertices present in toInsert list to the triangulation
    bool doInsertion (bool sort = false, double cellSize = 0);

    void updateCornersZValues();

    CT_DelaunayTriangulation *copy();

    CT_DelaunayTriangle *findTriangleContainingPoint(double x, double y, CT_DelaunayTriangle *refTriangle = nullptr);

    CT_DelaunayTriangle *getZCoordForXY(double x, double y, double &outZ, CT_DelaunayTriangle *refTriangle = nullptr, bool cornersIncluded = true);

    QList<CT_DelaunayVertex *> getNeighboursForCoordinates(double x, double y, bool cornersIncluded = true);

    bool isCorner(CT_DelaunayVertex *vertex) const;

    inline double getMinX() const {return _minx;}
    inline double getMinY() const {return _miny;}
    inline double getMaxX() const {return _maxx;}
    inline double getMaxY() const {return _maxy;}

private:
    QList<CT_DelaunayVertex*> _toInsert; // working list of to be inserted vertex


    QList<CT_DelaunayVertex*>                  _insertedVertices;     // list of inserted vertices
    QList<CT_DelaunayVertex*>                  _outOfBoundsVertices;  // list of not inserted vertices because a position out of bounds
    QMap<CT_DelaunayVertex*, CT_DelaunayVertex*>   _duplicatePositions;   // list of not inserted vertices because an already existing position
    QList<CT_DelaunayTriangle*>                _triangles;            // list of all triangles of the triangulation (created in compute ())

    QVector<CT_DelaunayVertex*>                _corners;              // list of the 4 created corners (not present in insertedVertices list)

    double _minx; // working bounds of the triangulation
    double _miny;
    double _maxx;
    double _maxy;

    CT_DelaunayTriangle* _refTriangle; // actual reference triangle
                                    // used as start point to find the inserted vertex triangle

    CT_DelaunayOutline      _outline; // outline defined by the triangulation

    bool _initialized; // check if bounds have been set
    bool _neighborsComputed; // check if neighbors are up to date
    bool _voronoiDiagramComputed; // check if voronoi diagram is up to date



    class VertexSorter {
    public:
        VertexSorter(double cellSize)
        {
            _cellsize = cellSize;
        }


        void orderVerticesByXY(QList<CT_DelaunayVertex*>& pointList)
        {
            std::sort(pointList.begin(), pointList.end(), [this] (const CT_DelaunayVertex* a, const CT_DelaunayVertex* b) {
                return compare(a, b); });
        }

        void orderVerticesByX(QList<CT_DelaunayVertex*>& pointList)
        {
            std::sort(pointList.begin(), pointList.end(), [this] (const CT_DelaunayVertex* a, const CT_DelaunayVertex* b) {
                return compareX(a, b); });
        }


    private:
        bool compare(const CT_DelaunayVertex* p1, const CT_DelaunayVertex* p2)
        {
            if (int(std::floor(p1->x() / _cellsize)) == int(std::floor(p2->x() / _cellsize)))
            {
                return (p1->x() < p2->x());
            } else {
                return (p1->y() < p2->y());
            }
        }

        bool compareX(const CT_DelaunayVertex* p1, const CT_DelaunayVertex* p2)
        {
            return (p1->x() < p2->x());
        }


        double _cellsize;
    };

    class VertexSorterHilbert {
    public:
        VertexSorterHilbert(double offsetX, double offsetY)
        {
            _offsetX = offsetX;
            _offsetY = offsetY;
        }

        void orderVerticesByHilbertCurve(QList<CT_DelaunayVertex*>& pointList)
        {
            std::sort(pointList.begin(), pointList.end(), [this] (const CT_DelaunayVertex* a, const CT_DelaunayVertex* b) {
                return compareHilbert(a, b); });
        }


        bool compareHilbert(const CT_DelaunayVertex* p1, const CT_DelaunayVertex* p2)
        {
            int ax = int(100.0*(p1->x() - _offsetX));
            int ay = int(100.0*(p1->y() - _offsetY));

            int bx = int(100.0*(p2->x() - _offsetX));
            int by = int(100.0*(p2->y() - _offsetY));

            // Calculate Hilbert order for each point
            int maxCoord = std::max(ax, ay);
            int n = std::pow(2, std::ceil(std::log2(maxCoord))); // Calculate nearest power of 2
            int orderA = 0, orderB = 0;

            for (int s = n / 2; s > 0; s /= 2) {
                int rx = (ax & s) > 0;
                int ry = (ay & s) > 0;
                orderA += s * s * ((3 * rx) ^ ry);
                std::tie(ax, ay) = rotate(s, ax, ay, rx, ry);
            }

            for (int s = n / 2; s > 0; s /= 2) {
                int rx = (bx & s) > 0;
                int ry = (by & s) > 0;
                orderB += s * s * ((3 * rx) ^ ry);
                std::tie(bx, by) = rotate(s, bx, by, rx, ry);
            }

            return orderA < orderB;
        }

        std::pair<int, int> rotate(int n, int x, int y, int rx, int ry) {
            if (ry == 0) {
                if (rx == 1) {
                    x = n - 1 - x;
                    y = n - 1 - y;
                }
                std::swap(x, y);
            }
            return std::make_pair(x, y);
        }

        double _offsetX;
        double _offsetY;
    };



    static bool compareDelaunayVertices(const CT_DelaunayVertex* p1, const CT_DelaunayVertex* p2)
    {
         return (qFuzzyCompare(p1->x(), p2->x()) && p1->y() < p2->y()) ||  (p1->x() < p2->x());
    }

    static double cross(const CT_DelaunayVertex* O, const CT_DelaunayVertex* A, const CT_DelaunayVertex* B);


};


#endif // CT_DELAUNAYTRIANGULATION_H
