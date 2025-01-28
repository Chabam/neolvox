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

#include "ct_delaunaytriangle.h"

#include <stdlib.h>     /* srand, rand */

CT_DelaunayTriangle::CT_DelaunayTriangle(CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t, CT_DelaunayVertex *v3t)
{
    _v1 = v1t;
    _v2 = v2t;
    _v3 = v3t;

    _n12 = nullptr;
    _n23 = nullptr;
    _n31 = nullptr;

    _toRemove = false;

    calculateCircle();
}

CT_DelaunayTriangle::~CT_DelaunayTriangle()
{
    _v1 = nullptr;
    _v2 = nullptr;
    _v3 = nullptr;
    _n12 = nullptr;
    _n23 = nullptr;
    _n31 = nullptr;
}

void CT_DelaunayTriangle::init(CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t, CT_DelaunayVertex *v3t)
{
    _v1 = v1t;
    _v2 = v2t;
    _v3 = v3t;

    _n12 = nullptr;
    _n23 = nullptr;
    _n31 = nullptr;

    _toRemove = false;

    calculateCircle();
}

void CT_DelaunayTriangle::setNeighbor(CT_DelaunayVertex *vt1, CT_DelaunayVertex *vt2, CT_DelaunayTriangle *ngb)
{

    if        (vt1 == _v1)
    {
        if      (vt2 == _v2) {_n12 = ngb;}
        else if (vt2 == _v3) {_n31 = ngb;}

    } else if (vt1 == _v2) {

        if      (vt2 == _v1) {_n12 = ngb;}
        else if (vt2 == _v3) {_n23 = ngb;}

    } else if (vt1 == _v3) {

        if      (vt2 == _v1) {_n31 = ngb;}
        else if (vt2 == _v2) {_n23 = ngb;}

    }
}

void CT_DelaunayTriangle::calculateCircle()
{
    double dx12 = _v2->x() - _v1->x();
    double dy12 = _v2->y() - _v1->y();
    double dx23 = _v3->x() - _v2->x();
    double dy23 = _v3->y() - _v2->y();
    double x12  = (_v1->x() + _v2->x())/2.0;
    double y12  = (_v1->y() + _v2->y())/2.0;
    double x23  = (_v2->x() + _v3->x())/2.0;
    double y23  = (_v2->y() + _v3->y())/2.0;

    // vertices are aligned :
    // should not happen during adding delaunay triangulation method
    // (if two point never have same position: verified by DelaunayTriangulation.doInsertion ())
    if ((dx12*dy23 - dy12*dx23) == 0)
    {
        //qDebug() << "Vertices " << _v1 << ", " << _v2 << ", " << _v3 << " aligned";
        _ccX = NAN;
        _ccY = NAN;
        _r2 = NAN;
        return;
    }

    // (v1v2) is horizontal
    if (dy12 == 0)
    {
        _ccX = x12;
        _ccY = y23 - (_ccX - x23)*dx23/dy23;

        //  (v1v2) is oblique
    } else {
        if (dy23 == 0)
        {
            _ccX = x23;
            _ccY = y12 - (_ccX - x12)*dx12/dy12;
        } else
        {
            _ccX = (y23 - y12 + dx23*x23/dy23 - dx12*x12/dy12) / (dx23/dy23 - dx12/dy12);
            _ccY = y23 - (_ccX - x23)*dx23/dy23;
        }
    }

    // circle radius calculation
    _r2 = pow(_ccX - _v1->x(),2) + pow(_ccY - _v1->y(),2);
}

bool CT_DelaunayTriangle::circleContains(double xt, double yt)
{
    return _r2 >= (pow(_ccX - xt,2) + pow(_ccY - yt,2));
}

//bool CT_DelaunayTriangle::contains(double xt, double yt) const
//{
//    CT_DelaunayVertex* max;
//    CT_DelaunayVertex* med;
//    CT_DelaunayVertex* min;
//    double x1, x2;

//    // 1) sorting vertices by y coordinate

//    if (_v1->y() > _v2->y())
//    {
//        max = _v1;
//        med = _v2;
//    } else
//    {
//        max = _v2;
//        med = _v1;
//    }

//    if (_v3->y() > max->y())
//    {
//        min = med;
//        med = max;
//        max = _v3;
//    } else if (_v3->y() > med->y())
//    {
//        min = med;
//        med = _v3;
//    } else
//    {
//        min = _v3;
//    }

//    // test if point is in the good y coordinate range
//    if ((yt > max->y()) || (yt < min->y()))
//    {
//        return false;
//    } else
//    {

//        // 2) caclulation of the point on max-min which split the triangle : (xs, ys)
//        double ys = med->y();
//        double xs = (ys - max->y()) * (max->x() - min->x()) / (max->y() - min->y()) + max->x();


//        // 3) top subTriangle analysis
//        if (yt > ys)
//        {
//            // x-bounds calculations for xt to be in subTriangle, with y=yt=constant
//            x1 = (yt - max->y()) * (max->x() - med->x()) / (max->y() - med->y()) + max->x();
//            x2 = (yt - max->y()) * (max->x() - xs) / (max->y() - ys) + max->x();

//            // 4) bottom subTriangle analysis
//        } else if (yt < ys)
//        {
//            // x-bounds calculations for xt to be in subTriangle, with y=yt=constant
//            x1 = (yt - med->y()) * (med->x() - min->x()) / (med->y() - min->y()) + med->x();
//            x2 = (yt - ys) * (xs - min->x()) / (ys - min->y()) + xs;

//            // 5) yt==ys
//        } else
//        {
//            // x-bounds calculations for xt to be in subTriangle, with y=yt=constant
//            x1 = xs;
//            x2 = med->x();
//        }

//        // test if xt is included between x1 and x2 bounds
//        if (x1 < x2)
//        {
//            return ( (xt >= x1) && (xt <= x2) );
//        } else
//        {
//            return ( (xt>=x2) && (xt<=x1) );
//        }

//    }
//}

bool CT_DelaunayTriangle::contains(double xt, double yt) const
{
    if ((xt < _v1->x() && xt < _v2->x() && xt < _v3->x()) ||
        (xt > _v1->x() && xt > _v2->x() && xt > _v3->x()) ||
        (yt < _v1->y() && yt < _v2->y() && yt < _v3->y()) ||
        (yt > _v1->y() && yt > _v2->y() && yt > _v3->y()))
    {
        return false;
    }

    // Calcul des vecteurs formés par les côtés du triangle
    Eigen::Vector2d vecAB(_v2->x() - _v1->x(), _v2->y() - _v1->y());
    Eigen::Vector2d vecBC(_v3->x() - _v2->x(), _v3->y() - _v2->y());
    Eigen::Vector2d vecCA(_v1->x() - _v3->x(), _v1->y() - _v3->y());

    // Calcul des vecteurs formés par le point vers les sommets du triangle
    Eigen::Vector2d vecAP(xt - _v1->x(), yt - _v1->y());
    Eigen::Vector2d vecBP(xt - _v2->x(), yt - _v2->y());
    Eigen::Vector2d vecCP(xt - _v3->x(), yt - _v3->y());

    // Calcul des produits vectoriels
    double crossABP = vecAB(0) * vecAP(1) - vecAB(1) * vecAP(0);
    double crossBCP = vecBC(0) * vecBP(1) - vecBC(1) * vecBP(0);
    double crossCAP = vecCA(0) * vecCP(1) - vecCA(1) * vecCP(0);

    // Vérification du sens de rotation
    if ((crossABP >= 0 && crossBCP >= 0 && crossCAP >= 0) ||
        (crossABP <= 0 && crossBCP <= 0 && crossCAP <= 0)) {
        return true;
    } else {
        return false;
    }
}


double CT_DelaunayTriangle::getBaryX()
{
    return (_v1->x() + _v2->x() + _v3->x()) / 3.0;
}

double CT_DelaunayTriangle::getBaryY()
{
    return (_v1->y() + _v2->y() + _v3->y()) / 3.0;
}

CT_DelaunayVertex *CT_DelaunayTriangle::getThirdVertex(CT_DelaunayVertex *vt1, CT_DelaunayVertex *vt2)
{

    if      ((vt1 == _v1) && (vt2 == _v2)) {return _v3;}
    else if ((vt1 == _v2) && (vt2 == _v3)) {return _v1;}
    else if ((vt1 == _v1) && (vt2 == _v3)) {return _v2;}
    else {return nullptr;}
}

CT_DelaunayVertex *CT_DelaunayTriangle::getThirdVertex(const QVector<CT_DelaunayVertex *> &vt)
{
    if      ((vt.at(0) == _v1) && (vt.at(1) == _v2)) {return _v3;}
    else if ((vt.at(0) == _v2) && (vt.at(1) == _v3)) {return _v1;}
    else if ((vt.at(0) == _v1) && (vt.at(1) == _v3)) {return _v2;}
    else                                           {return nullptr;}
}

CT_DelaunayTriangle* CT_DelaunayTriangle::getNextTriangleTo(double xt, double yt)
{
    double dx, dy;
    double a, b;

    if (_n12 != nullptr)
    {
        dx = _v2->x() - _v1->x();
        dy = _v2->y() - _v1->y();

        if (dx == 0) {
            if (((_v3->x() < _v1->x()) && (xt > _v1->x())) || ((_v3->x() > _v1->x()) && (xt < _v1->x())))
            {
                return _n12;
            }

        } else  {
            a = (_v1->y() + (dy/dx)*(_v3->x() - _v1->x()));
            b = (_v1->y() + (dy/dx)*(xt - _v1->x()));

            if (((_v3->y() < a) && (yt > b)) || ((_v3->y() > a) && (yt < b)))
            {
                return _n12;
            }
        }
    }

    if (_n23 != nullptr)
    {
        dx = _v3->x()-_v2->x();
        dy = _v3->y()-_v2->y();

        if (dx == 0)
        {
            if (((_v1->x() < _v2->x()) && (xt > _v2->x())) || ((_v1->x() > _v2->x()) && (xt < _v2->x())))
            {
                return _n23;
            }

        } else
        {
            a = (_v2->y() + (dy/dx)*(_v1->x() - _v2->x()));
            b = (_v2->y() + (dy/dx)*(xt - _v2->x()));

            if (((_v1->y() < a) && (yt > b)) || ((_v1->y() > a) && (yt < b)))
            {
                return _n23;
            }
        }
    }


    if (_n31 != nullptr)
    {
        dx = _v1->x()-_v3->x();
        dy = _v1->y()-_v3->y();

        if (dx == 0) {
            if (((_v2->x() < _v3->x()) && (xt > _v3->x())) || ((_v2->x() > _v3->x()) && (xt < _v3->x())))
            {
                return _n31;
            }

        } else
        {
            a = (_v3->y() + (dy/dx)*(_v2->x() - _v3->x()));
            b = (_v3->y() + (dy/dx)*(xt - _v3->x()));

            if (((_v2->y() < a) && (yt > b)) || ((_v2->y() > a) && (yt < b)))
            {
                return _n31;
            }
        }
    }


// The loop case is managed is calling function

//    if ((n12ok)  && (!n23ok) && (!n31ok)) {return _n12;}
//    if ((!n12ok) && (n23ok)  && (!n31ok)) {return _n23;}
//    if ((!n12ok) && (!n23ok) && (n31ok))  {return _n31;}

    // stochastic process
    // (to avoid cycles, which normally should not happen in delaunay triangulation)

//    if ((n12ok) && (n23ok)) {if ((std::rand() % 2) == 1) {return _n12;} else {return _n23;}}
//    if ((n23ok) && (n31ok)) {if ((std::rand() % 2) == 1) {return _n23;} else {return _n31;}}
//    if ((n12ok) && (n31ok)) {if ((std::rand() % 2) == 1) {return _n12;} else {return _n31;}}

    // if (n12ok==false) && (n23ok==false) && (n31ok==false) :
    // then point (xt,yt) is in the this triangle
    return this;

}

CT_Polygon2DData *CT_DelaunayTriangle::getShape()
{
    QVector<Eigen::Vector2d> vertices(3);

    vertices[0] = Eigen::Vector2d(_v1->x(), _v1->y());
    vertices[1] = Eigen::Vector2d(_v2->x(), _v2->y());
    vertices[2] = Eigen::Vector2d(_v3->x(), _v3->y());

    return new CT_Polygon2DData(vertices);
}
