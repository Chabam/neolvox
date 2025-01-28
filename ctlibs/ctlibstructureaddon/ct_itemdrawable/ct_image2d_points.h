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

#ifndef CT_IMAGE2D_POINTS_H
#define CT_IMAGE2D_POINTS_H

#include "ct_itemdrawable/abstract/ct_abstractimage2d.h"
#include "ct_accessor/ct_pointaccessor.h"

class CT_StandardImage2D_PointDrawManager;

class CTLIBSTRUCTUREADDON_EXPORT CT_Image2D_Points : public CT_AbstractImage2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_Image2D_Points, CT_AbstractImage2D, Point 2D Grid)

    using SuperClass = CT_AbstractImage2D;

public:
    CT_Image2D_Points();
    CT_Image2D_Points(const CT_Image2D_Points& other);
    ~CT_Image2D_Points() override;


    /*!
     * \brief Contructor with integer column and row coordinates
     *
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param dimx Number of colums
     * \param dimy Number of rows
     * \param resolution Size of a cell
     */
    CT_Image2D_Points(double xmin,
              double ymin,
              int dimx,
              int dimy,
              double resolution);

    /*!
     * \brief Factory with min and max (X,Y) coordinates
     *
     * \param model Item model for creation
     * \param result Result containing the item
     * \param xmin Minimum X coordinate (bottom left corner)
     * \param ymin Minimum Y coordinate (bottom left corner)
     * \param xmax Maximum X coordinate (upper right corner)
     * \param ymax Maximum Y coordinate (upper right corner)
     * \param resolution Size of a cell
     * \param coordConstructor Not used, only to ensure constructor different signatures
     */
    static CT_Image2D_Points* createGrid3DFromXYCoords(double xmin,
                                                          double ymin,
                                                          double xmax,
                                                          double ymax,
                                                          double resolution);


    bool addPoint(size_t pointGlobalIndex);
    bool addPoint(size_t pointLocalIndex, double x, double y);
    bool addPointAtCellIndex(size_t cellIndex, size_t pointLocalIndex);

    const QList<size_t>* getConstPointIndexList(size_t cellIndex) const;

    void getCellIndicesAtNeighbourhoodN(size_t originIndex, int n, QList<size_t> &indices) const;

    int getPointsInCellsIntersectingCircle(size_t cellIndex, double radius, QList<size_t> &indexList) const;

    size_t getPointsInCellsIntersectingCircle(const Eigen::Vector3d &center, double radius, QList<size_t> &indexList) const;
    size_t getPointsInCircle(const Eigen::Vector3d &center, double radius, QList<size_t> &indexList) const;

    int getPointIndicesIncludingKNearestNeighbours(const Eigen::Vector3d &position, int k, double maxDist, QList<size_t> &indexList) const;

    void getIndicesWithPoints(QList<size_t> &list) const {list.append(_cells.keys());}


    // Neutralize useless Methods
    double ratioValueAtIndex(const size_t index) const override {Q_UNUSED(index); qDebug() << "This method should not be used in this context";return 0;}
    double valueAtIndexAsDouble(const size_t index) const override {Q_UNUSED(index); qDebug() << "This method should not be used in this context";return 0;}
    QString NAAsString() const override {qDebug() << "This method should not be used in this context";return nullptr;}
    double NAAsDouble() const override {qDebug() << "This method should not be used in this context";return 0;}
    double minValueAsDouble() const override {qDebug() << "This method should not be used in this context";return 0;}
    double maxValueAsDouble() const override {qDebug() << "This method should not be used in this context";return 0;}
    QString valueAtIndexAsString(const size_t index) const override {Q_UNUSED(index); qDebug() << "This method should not be used in this context";return nullptr;}
    void computeMinMax() override {qDebug() << "This method should not be used in this context";}


    CT_ITEM_COPY_IMP(CT_Image2D_Points)

private:
    QMap<size_t, QList<size_t>* >    _cells;

    QList<size_t>*                  _emptyList;
    CT_PointAccessor                _pointAccessor;

    const static CT_StandardImage2D_PointDrawManager IMAGE2D_POINT_DRAW_MANAGER;

};

#endif // CT_IMAGE2D_POINTS_H
