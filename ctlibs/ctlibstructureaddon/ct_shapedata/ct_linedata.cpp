/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_linedata.h"
#include "ct_iterator/ct_pointiterator.h"
#include "ct_itemdrawable/abstract/ct_abstractgeometricalitem.h"

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include "ct_math/ct_mathpoint.h"

#define ONE_DIV_THREE           0.33333333333333333333
#define ONE_DIV_ONE_POINT_FIVE  0.66666666666666666666
#define M_PI_MULT_2             6.28318530717958647692

CT_LineData::CT_LineData() : SuperClass(),
    _error(0),
    _n_points(0)
{
}

CT_LineData::CT_LineData(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2) : SuperClass((p1 + p2) / 2.0, p2 - p1),
    _p1(p1),
    _p2(p2),
    _error(0),
    _n_points(0)
{
}

CT_LineData::CT_LineData(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, double error, int n) : CT_LineData(p1, p2)
{
    _error = error;
    _n_points = n;
}

const Eigen::Vector3d& CT_LineData::getP1() const
{
    return _p1;
}

const Eigen::Vector3d& CT_LineData::getP2() const
{
    return _p2;
}

double CT_LineData::getError() const
{
    return _error;
}

double CT_LineData::x1() const
{
    return _p1(0);
}

double CT_LineData::y1() const
{
    return _p1(1);
}

double CT_LineData::z1() const
{
    return _p1(2);
}

double CT_LineData::x2() const
{
    return _p2(0);
}

double CT_LineData::y2() const
{
    return _p2(1);
}

double CT_LineData::z2() const
{
    return _p2(2);
}

double CT_LineData::length() const
{
    return CT_MathPoint::distance3D(_p1, _p2);
}

void CT_LineData::getBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min(0) = std::min(_p1(0), _p2(0));
    min(1) = std::min(_p1(1), _p2(1));
    min(2) = std::min(_p1(2), _p2(2));

    max(0) = std::max(_p1(0), _p2(0));
    max(1) = std::max(_p1(1), _p2(1));
    max(2) = std::max(_p1(2), _p2(2));
}


bool CT_LineData::intersectionWithRect3D  (double plan_x, double plan_y, double plan_z, Eigen::Vector3d& vect_plan, double* xi, double* yi, double* zi)
{
    // intersection segment plan : http://www.softsurfer.com/Archive/algorithm_0104/algorithm_0104B.htm#intersect3D_SegPlane()
    Eigen::Vector3d u(_p2(0) - _p1(0), _p2(1) - _p1(1), _p2(2) - _p1(2));
    Eigen::Vector3d w(_p1(0) - plan_x, _p1(1) - plan_y, _p1(2) - plan_z);

    double D = vect_plan.dot(u);
    double N = -vect_plan.dot(w);

    if(fabs(D) > 0.00000001)
    {
        double sI = N / D;

        *xi = _p1(0) + sI * u(0);
        *yi = _p1(1) + sI * u(1);
        *zi = _p1(2) + sI * u(2);

        return true;
    }

    return false;
}

CT_LineData* CT_LineData::staticCreateLineDataFromPointCloud(const CT_AbstractPointCloudIndex& pointCloudIndex)
{
    CT_PointIterator it(&pointCloudIndex);

    if(it.size() < 2) {return nullptr;}

    QList<Eigen::Vector3d> liste;

    while(it.hasNext())
    {
        it.next();
        const CT_Point& p = it.currentPoint();
        liste.append(Eigen::Vector3d(p(0), p(1), p(2)));
    }

    CT_LineData* result = CT_LineData::staticCreateLineDataFromPointCloud(liste);
    return result;
}

CT_LineData *CT_LineData::staticCreateLineDataFromItemCenters(const QList<const CT_AbstractGeometricalItem*>& items)
{
    QList<Eigen::Vector3d> liste;

    QListIterator<const CT_AbstractGeometricalItem* > it(items);
    while (it.hasNext())
    {
        const CT_AbstractGeometricalItem* item = it.next();
        const Eigen::Vector3d& coord = item->centerCoordinate();
        liste.append(coord);
    }

    return CT_LineData::staticCreateLineDataFromPointCloud(liste);
}

CT_LineData* CT_LineData::staticCreateLineDataFromPointCloud(const QList<Eigen::Vector3d> &l_gp, bool computeError)
{
    if (l_gp.size() < 2) {return nullptr;}
    else if (l_gp.size() == 2)
    {
        return new CT_LineData(l_gp.at(0), l_gp.at(1), 0, 2);
    }


    // 20/12/2019 : modification of fitting method : now use Eigen method
    // Now Error = standard error (sqrt(variance))

    // copy coordinates to  matrix in Eigen format
    int l_gp_size = l_gp.size();
    double minz = std::numeric_limits<double>::max();
    double maxz = -std::numeric_limits<double>::max();

    Eigen::Vector3d offset = l_gp[0];

    Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic > centers(l_gp_size, 3);
    for (int i = 0; i < l_gp_size; ++i)
    {
        const Eigen::Vector3d& pt = l_gp[i] - offset;
        centers.row(i) = pt;
        if (pt(2) < minz) {minz = pt(2);}
        if (pt(2) > maxz) {maxz = pt(2);}
    }

    // Compute fitted line origin and direction
    Eigen::Vector3d origin = centers.colwise().mean();
    Eigen::MatrixXd centered = centers.rowwise() - origin.transpose();
    Eigen::MatrixXd cov = centered.adjoint() * centered;
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
    Eigen::Vector3d axis = eig.eigenvectors().col(2).normalized();

    Eigen::Vector3d point1 = origin + ((minz - origin(2)) / axis(2)) * axis + offset;
    Eigen::Vector3d point2 = origin + ((maxz - origin(2)) / axis(2)) * axis + offset;

    double variance = 0;

    if (computeError)
    {
        for (int i = 0; i < l_gp_size; ++i)
        {
            const Eigen::Vector3d& pt = centers.row(i);

            double dist = CT_MathPoint::distancePointLine(pt, axis, origin);
            variance += dist*dist/l_gp_size;
        }
    }


    return new CT_LineData(point1, point2, sqrt(variance), l_gp_size);


//    double x;
//    double y;
//    double z;

//    quint64 n = 0;

//    double Xm = 0;
//    double Xm_carre;

//    double Ym = 0;
//    double Ym_carre;

//    double Zm = 0;
//    double Zm_carre;

//    double Xm_m_Ym;
//    double Xm_m_Zm;
//    double Ym_m_Zm;

//    double Sxx = 0;
//    double Syy = 0;
//    double Szz = 0;
//    double Sxy = 0;
//    double Sxz = 0;
//    double Syz = 0;

//    double Sxy_carre;
//    double Sxz_carre;
//    double Syz_carre;

//    double c0;
//    double c1;
//    double c2;
//    double c3;

//    double r;
//    double s;
//    double t;
//    double p;
//    double q;
//    double R;

//    int n_a;
//    double *a_tab;

//    double a;
//    double a_carre;
//    double b;
//    double b_carre;
//    double u;
//    double v;
//    double w;

//    double temp;
//    double temp1;
//    double temp2;
//    double temp3;
//    double temp4;
//    double temp5;

//    double zmin = std::numeric_limits<double>::max();
//    double zmax = -std::numeric_limits<double>::max();
//    double xmin = std::numeric_limits<double>::max();
//    double xmax = -std::numeric_limits<double>::max();

//    Eigen::Vector3d pt1, pt2;

//    double offsetX = 0;
//    double offsetY = 0;
//    double offsetZ = 0;
//    bool first = true;


//    QListIterator<Eigen::Vector3d> it(l_gp);

//    while(it.hasNext())
//    {
//        Eigen::Vector3d point = it.next();

//        // Added 01/06/2018 : offset management to avoid error with double precision coordinates
//        if (first)
//        {
//            first = false;
//            offsetX = point(0);
//            offsetY = point(1);
//            offsetZ = point(2);
//        }
//        point(0) -= offsetX;
//        point(1) -= offsetY;
//        point(2) -= offsetZ;


//        if (n == 0) {pt1 = point;}
//        if (n == 1) {pt2 = point;}

//        x = point(0);
//        y = point(1);
//        z = point(2);

//        if (x < xmin) {xmin = x;}
//        if (x > xmax) {xmax = x;}
//        if (z < zmin) {zmin = z;}
//        if (z > zmax) {zmax = z;}

//        Xm += x;
//        Ym += y;
//        Zm += z;

//        ++n;
//    }

//    if (n < 1) {return NULL;}
//    else if (n <  2) {return new CT_LineData(pt1, Eigen::Vector3d(pt1(0), pt1(1), pt1(2) + 1.0), 0, n);}
//    else if (n == 2) {return new CT_LineData(pt1, pt2, 0, n);}


//    temp1 = n;

//    Xm /= temp1;
//    Ym /= temp1;
//    Zm /= temp1;

//    Xm_carre = Xm*Xm;
//    Ym_carre = Ym*Ym;
//    Zm_carre = Zm*Zm;

//    Xm_m_Ym = Xm*Ym;
//    Xm_m_Zm = Xm*Zm;
//    Ym_m_Zm = Ym*Zm;

//    it.toFront();

//    while(it.hasNext())
//    {
//        const Eigen::Vector3d& point = it.next();

//        // modified 01/06/2018 : offset management to avoid error with double precision coordinates
//        x = point(0) - offsetX;
//        y = point(1) - offsetY;
//        z = point(2) - offsetZ;

//        Sxx += (x*x) - (Xm_carre);
//        Syy += (y*y) - (Ym_carre);
//        Szz += (z*z) - (Zm_carre);

//        Sxy += (x*y) - (Xm_m_Ym);
//        Sxz += (x*z) - (Xm_m_Zm);
//        Syz += (y*z) - (Ym_m_Zm);

//    }

//    Sxx /= temp1;
//    Syy /= temp1;
//    Szz /= temp1;

//    Sxy /= temp1;
//    Sxz /= temp1;
//    Syz /= temp1;

//    Sxy_carre = Sxy*Sxy;
//    Sxz_carre = Sxz*Sxz;
//    Syz_carre = Syz*Syz;

//    c0 = Sxy*Sxz * (Szz-Syy) + Syz * (Sxy_carre - Sxz_carre);
//    c1 = Sxz*Syz * ((2.0*Sxx)-Syy-Szz) + Sxy * (-Sxy_carre-Sxz_carre+2*Syz_carre) + Sxy * (Szz-Sxx)*(Szz-Syy);
//    c2 = Sxy*Sxz * (Sxx+Syy-(2.0*Szz)) + Syz * (Sxz_carre+Syz_carre-(2*Sxy_carre)) + Syz * (Sxx-Syy)*(Szz-Sxx);
//    c3 = Sxz*Syz * (Syy-Sxx) + Sxy*(Sxz_carre-Syz_carre);

//    r = c2/c3;
//    s = c1/c3;
//    t = c0/c3;
//    p = s - ((r*r) / 3.0);
//    q = ((2.0 * (r*r*r)) / 27.0) - ((r*s) / 3.0) + t;
//    R = ((q*q) / 4.0) + ((p*p*p) / 27.0);

//    if(R > 0)
//    {
//        n_a = 1;
//        a_tab = new double[n_a];
//        a_tab[0] = (-r/3.0) + pow(sqrt((-q/2.0) + pow(sqrt(R), 0.5)), ONE_DIV_THREE) + pow(sqrt((-q/2.0) - pow(sqrt(R), 0.5)), ONE_DIV_THREE);
//    }
//    else
//    {
//        double ro = sqrt(-(p*p*p)/27.0);
//        double phi = acos(-q/(2.0*ro));

//        temp1 = (-r/3.0);
//        temp2 = 2 * pow(sqrt(ro), ONE_DIV_ONE_POINT_FIVE); // dans le pdf c'est 1.0/3.0 (erreur dans le pdf apparemment)

//        n_a = 3;
//        a_tab = new double[n_a];

//        a_tab[0] = temp1 + (temp2 * cos(phi/3.0));
//        a_tab[1] = temp1 + (temp2 * cos((phi + M_PI_MULT_2)/3.0));
//        a_tab[2] = temp1 + (temp2 * cos((phi + 4*M_PI)/3.0));
//    }

//    temp5 = 999999999;

//    for(int i=0; i<n_a; ++i)
//    {
//        a = a_tab[i];
//        a_carre = a*a;

//        b = ((a * (Szz-Sxx)) + (1 - a_carre) * Sxz) / (Sxy + (a * Syz));
//        b_carre = b*b;

//        temp = 1.0 / (1.0 + a_carre + b_carre);

//        double ut = temp * (((1.0 + b_carre)*Xm) - (a*b*Ym) + (a*Zm));
//        double vt = temp * ((-a*b*Xm) + ((1.0+a_carre)*Ym) + (b*Zm));
//        double wt = temp * ((a*Xm) + (b*Ym) + ((a_carre + b_carre)*Zm));

//        temp4 = 0;

//        it.toFront();


//        while(it.hasNext())
//        {
//            const Eigen::Vector3d& point = it.next();

//            // modified 01/06/2018 : offset management to avoid error with double precision coordinates
//            x = point(0) - offsetX;
//            y = point(1) - offsetY;
//            z = point(2) - offsetZ;

//            temp1 = (temp * (((1.0 + b_carre)*x) - (a*b*y) + (a*z))) - ut;
//            temp2 = (temp * ((-a*b*x) + ((1.0+a_carre)*y) + (b*z))) - vt;
//            temp3 = (temp * ((a*x) + (b*y) + ((a_carre + b_carre)*z))) - wt;

//            temp4 += (temp1*temp1) + (temp2*temp2) + (temp3*temp3);
//        }

//        if(temp4 < temp5)
//        {
//            temp5 = temp4;
//            u = ut;
//            v = vt;
//            w = wt;
//        }
//    }

//    delete [] a_tab;

//    Eigen::Vector3d p1(Xm, Ym, Zm);
//    Eigen::Vector3d p2(u, v, w);

//    Eigen::Vector3d point1;
//    Eigen::Vector3d point2;

//    Eigen::Vector3d dir = p2 - p1;
//    dir.normalize();

//    if (abs(zmax - zmin) > 0.1)
//    {
//        double tn = (zmin - p1(2)) / dir(2);

//        point1(0) = p1(0) + tn*dir(0);
//        point1(1) = p1(1) + tn*dir(1);
//        point1(2) = zmin;

//        tn = (zmax - p1(2)) / dir(2);

//        point2(0) = p1(0) + tn*dir(0);
//        point2(1) = p1(1) + tn*dir(1);
//        point2(2) = zmax;
//    } else {
//        double tn = (xmin - p1(0)) / dir(0);

//        point1(0) = xmin;
//        point1(1) = p1(1) + tn*dir(1);
//        point1(2) = p1(2) + tn*dir(2);

//        tn = (xmax - p1(0)) / dir(0);

//        point2(0) = xmax;
//        point2(1) = p1(1) + tn*dir(1);
//        point2(2) = p1(2) + tn*dir(2);
//    }

//    // modified 01/06/2018 : offset management to avoid error with double precision coordinates
//    point1(0) += offsetX;
//    point1(1) += offsetY;
//    point1(2) += offsetZ;

//    point2(0) += offsetX;
//    point2(1) += offsetY;
//    point2(2) += offsetZ;

//    return new CT_LineData(point1, point2, temp5, n);

}
